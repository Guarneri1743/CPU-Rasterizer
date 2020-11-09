#ifndef _GRAPHICS_DEVICE_
#define _GRAPHICS_DEVICE_
#include <Guarneri.hpp>

namespace Guarneri {
	class GraphicsDevice {
	public:
		uint32_t width;
		uint32_t height;

	private:
		// use 32 bits zbuffer here, for convenience 
		std::shared_ptr<RawBuffer<float>> zbuffer;
		// 32 bits bgra framebuffer, 8-bit per channel
		std::shared_ptr<RawBuffer<color_bgra>> framebuffer;
		// 8 bits stencil buffer
		std::shared_ptr<RawBuffer<uint8_t>> stencilbuffer;

	public:
		void initialize(void* bitmap_handle_t, uint32_t width_t, uint32_t height_t) {
			this->width = width_t;
			this->height = height_t;
			zbuffer = std::make_shared<RawBuffer<float>>(width_t, height_t);
			framebuffer = std::make_shared<RawBuffer<color_bgra>>(bitmap_handle_t, width_t, height_t, [](color_bgra* ptr) { unused(ptr); /*delete[] (void*)ptr;*/ });
			stencilbuffer = std::make_shared<RawBuffer<uint8_t>>(width_t, height_t);
			zbuffer->clear(FAR_Z);
			stencilbuffer->clear(DEFAULT_STENCIL);
			framebuffer->clear(DEFAULT_COLOR);
		}

		void draw(std::shared_ptr<Material> material, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p) {
			auto object_space_frustum = Frustum::create(p * v * m);
			if ((misc_param.culling_clipping_flag & CullingAndClippingFlag::NEAR_PLANE_CLIPPING) != CullingAndClippingFlag::DISABLE) {
				auto triangles = Clipper::near_plane_clipping(object_space_frustum.near, v1, v2, v3);
				if (triangles.size() == 0) {
					return;
				}
				for (size_t idx = 0; idx < triangles.size(); idx++) {
					auto tri = triangles[idx];
					draw_triangle(material, tri[0], tri[1], tri[2], m, v, p);
				}
			}
			else {
				draw_triangle(material, v1, v2, v3, m, v, p);
			}
		}

		void clear_buffer(const BufferFlag& flag) {
			if ((flag & BufferFlag::COLOR) != BufferFlag::NONE) {
				if ((misc_param.render_flag & RenderFlag::DEPTH) != RenderFlag::DISABLE)
				{
					framebuffer->clear(DEFAULT_DEPTH_COLOR);
				}
				else {
					framebuffer->clear(DEFAULT_COLOR);
				}
			}
			if ((flag & BufferFlag::DEPTH) != BufferFlag::NONE) {
				zbuffer->clear(FAR_Z);
			}
			if ((flag & BufferFlag::STENCIL) != BufferFlag::NONE) {
				stencilbuffer->clear(DEFAULT_STENCIL);
			}
		}

	private:
		void draw_triangle(std::shared_ptr<Material> material, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p) {
			auto shader = material->target_shader;

			shader->set_mvp_matrix(m, v, p);
			shader->sync(material->name2float, material->name2float4, material->name2int, material->name2tex);
			shader->sync(material->ztest_mode, material->zwrite_mode);
			shader->sync(material->transparent, material->src_factor, material->dst_factor, material->blend_op);
			shader->sync(material->lighting_param);

			assert(shader != nullptr);

			v2f o1 = process_vertex(shader, v1);
			v2f o2 = process_vertex(shader, v2);
			v2f o3 = process_vertex(shader, v3);

			Vertex c1(o1.position, o1.world_pos, o1.color, o1.normal, o1.uv, o1.tangent, o1.bitangent);
			Vertex c2(o2.position, o2.world_pos, o2.color, o2.normal, o2.uv, o2.tangent, o2.bitangent);
			Vertex c3(o3.position, o3.world_pos, o3.color, o3.normal, o3.uv, o3.tangent, o3.bitangent);

			// clip in screen space
			//if (!material->skybox && Clipper::cvv_clipping(c1.position, c2.position, c3.position)) {
			//	return;
			//}

			// perspective division, position.
			Vertex n1 = clip2ndc(c1);
			Vertex n2 = clip2ndc(c2);
			Vertex n3 = clip2ndc(c3);

			bool double_face = material->double_face;
			bool enable_backface_culling = (misc_param.culling_clipping_flag & CullingAndClippingFlag::BACK_FACE_CULLING) != CullingAndClippingFlag::DISABLE;

			if (!double_face && enable_backface_culling && !material->skybox && Clipper::backface_culling(n1.position, n2.position, n3.position)) {
				return;
			}

			// perspective division, uv, color, normal, etc. 
			n1.perspective_division(c1.rhw);
			n2.perspective_division(c2.rhw);
			n3.perspective_division(c3.rhw);

			Vertex s1 = ndc2viewport(n1);
			Vertex s2 = ndc2viewport(n2);
			Vertex s3 = ndc2viewport(n3);

			s1.position.w = c1.position.w;
			s1.rhw = 1.0f / c1.position.w;

			s2.position.w = c2.position.w;
			s2.rhw = 1.0f / c2.position.w;

			s3.position.w = c3.position.w;
			s3.rhw = 1.0f / c3.position.w;

			Triangle tri(s1, s2, s3);

			// primitive assembly
			std::vector<Triangle> tris = tri.horizontally_split();

			// rasterization
			rasterize(tris, material);

			// wireframe
			if ((misc_param.render_flag & RenderFlag::WIREFRAME) != RenderFlag::DISABLE) {

				draw_screen_segment(tri[0].position, tri[1].position, Color(1.0f, 1.0f, 1.0f, 1.0f));
				draw_screen_segment(tri[0].position, tri[2].position, Color(1.0f, 1.0f, 1.0f, 1.0f));
				draw_screen_segment(tri[2].position, tri[1].position, Color(1.0f, 1.0f, 1.0f, 1.0f));
			}

			// wireframe & scanline
			if ((misc_param.render_flag & RenderFlag::SCANLINE) != RenderFlag::DISABLE) {

				for (auto iter = tris.begin(); iter != tris.end(); iter++) {
					auto& t = *iter;
					draw_screen_segment(t[0].position, t[1].position, Color(1.0f, 1.0f, 1.0f, 1.0f));
					draw_screen_segment(t[0].position, t[2].position, Color(1.0f, 1.0f, 1.0f, 1.0f));
					draw_screen_segment(t[2].position, t[1].position, Color(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
		}

		// per vertex processing
		v2f process_vertex(const std::shared_ptr<Shader>& shader, const Vertex& vert) {
			a2v input;
			input.position = vert.position;
			input.uv = vert.uv;
			input.color = vert.color;
			input.normal = vert.normal;
			return shader->vertex_shader(input);
		}

		// rasterization
		void rasterize(std::vector<Triangle>& tris, const std::shared_ptr<Material>& mat) {
			for (auto iter = tris.begin(); iter != tris.end(); iter++) {
				auto& tri = *iter;
				bool flip = tri.flip;
				int top_idx = flip ? 2 : 0;
				int bottom_idx = flip ? 0 : 2;
				int top = CEIL(tri[top_idx].position.y);
				int bottom = CEIL(tri[bottom_idx].position.y);
				top = CLAMP_INT(top, 0, this->height);
				bottom = CLAMP_INT(bottom, 0, this->height);
				assert(bottom >= top);

				// interpolate vertically
				for (int row = top; row < bottom; row++) {
					Vertex lhs;
					Vertex rhs;
					tri.interpolate((float)row + 0.5f, lhs, rhs);

					// screen space clipping
					bool enable_screen_clipping = (misc_param.culling_clipping_flag & CullingAndClippingFlag::SCREEN_CLIPPING) != CullingAndClippingFlag::DISABLE;
					if (enable_screen_clipping) {
						Clipper::screen_clipping(lhs, rhs, this->width);
					}
					
					int left = CEIL(lhs.position.x);
					left = CLAMP_INT(left, 0, this->width);
					int right = CEIL(rhs.position.x);
					right = CLAMP_INT(right, 0, this->width);
					assert(right >= left);

					// interpolate horizontally
					for (int col = left; col < right; col++) {
						process_fragment(lhs, row, col, mat);
						auto dx = Vertex::differential(lhs, rhs);
						lhs = Vertex::intagral(lhs, dx);
					}
				}
			}
		}

		// per fragment processing
		void process_fragment(Vertex& v, const uint32_t& row, const uint32_t& col, std::shared_ptr<Material>  mat) {
			bool enable_scissor_test = (misc_param.persample_op_flag & PerSampleOperation::SCISSOR_TEST) != PerSampleOperation::DISABLE;
			bool enable_alpha_test = (misc_param.persample_op_flag & PerSampleOperation::ALPHA_TEST) != PerSampleOperation::DISABLE;
			bool enable_stencil_test = (misc_param.persample_op_flag & PerSampleOperation::STENCIL_TEST) != PerSampleOperation::DISABLE;
			bool enable_depth_test = (misc_param.persample_op_flag & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE;

			PerSampleOperation op_pass = PerSampleOperation::SCISSOR_TEST | PerSampleOperation::ALPHA_TEST | PerSampleOperation::STENCIL_TEST | PerSampleOperation::DEPTH_TEST;

			auto s = mat->target_shader;
			float z = v.position.z;

			ZTest ztest_mode = s->ztest_mode;
			ZWrite zwrite_mode = s->zwrite_mode;
			BlendFactor src_factor = s->src_factor;
			BlendFactor dst_factor = s->dst_factor;
			BlendOp blend_op = s->blend_op;

			bool enable_blending = (misc_param.persample_op_flag & PerSampleOperation::BLENDING) != PerSampleOperation::DISABLE && s->transparent;

			color_bgra pixel_color;

			// fragment shader
			Color fragment_result;
			if ((misc_param.render_flag & RenderFlag::SHADED) != RenderFlag::DISABLE && s != nullptr) {
				v2f v_out;
				float w = 1.0f / v.rhw;
				v_out.position = v.position;
				v_out.world_pos = v.world_pos * w;
				v_out.color = v.color * w;
				v_out.normal = v.normal * w;
				v_out.uv = v.uv * w;
				fragment_result = s->fragment_shader(v_out);
				pixel_color = Color::encode_bgra(fragment_result);
			}

			// todo: scissor test
			if (enable_scissor_test) 
			{
				
			}

			// todo: alpha test
			if (enable_alpha_test) 
			{ 
				
			}

			// todo: stencil test
			if (enable_stencil_test) 
			{
				
			}

			// depth test
			if (enable_depth_test) {
				if (!depth_test(ztest_mode, zwrite_mode, row, col, z)) {
					op_pass &= ~PerSampleOperation::DEPTH_TEST;
				}
			}

			// blending
			if (enable_blending && s != nullptr && s->transparent) {
				color_bgra dst;
				if (framebuffer->read(row, col, dst)) {
					Color dst_color = Color::decode(dst);
					Color src_color = fragment_result;
					Color blended_color = blend(src_color, dst_color, src_factor, dst_factor, blend_op);
					pixel_color = Color::encode_bgra(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
				}
			}

			if (validate_fragment(op_pass)) {
				// write to color buffer
				framebuffer->write(row, col, pixel_color);
			}

			if ((op_pass & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE) {
				// write z buffer 
				if (zwrite_mode == ZWrite::ON || !enable_blending) {
					zbuffer->write(row, col, z);
				}
			}

			// depth buffer visualization
			if ((misc_param.render_flag & RenderFlag::DEPTH) != RenderFlag::DISABLE) {
				float cur_depth;
				if (zbuffer->read(row, col, cur_depth)) {
					float linear_depth = linearize_depth(cur_depth, misc_param.cam_near, misc_param.cam_far);
					Vector3 depth_color = Vector3::ONE * linear_depth / 30.0f;
					color_bgra c = Color::encode_bgra(depth_color.x, depth_color.y, depth_color.z, 1.0f);
					framebuffer->write(row, col, c);
				}
			}
		}

		bool validate_fragment(PerSampleOperation op_pass) {
			if ((op_pass & PerSampleOperation::SCISSOR_TEST) == PerSampleOperation::DISABLE) return false;
			if ((op_pass & PerSampleOperation::ALPHA_TEST) == PerSampleOperation::DISABLE) return false;
			if ((op_pass & PerSampleOperation::STENCIL_TEST) == PerSampleOperation::DISABLE) return false;
			if ((op_pass & PerSampleOperation::DEPTH_TEST) == PerSampleOperation::DISABLE) return false;
			return true;
		}

		// todo: alpha factor
		static Color blend(const Color& src_color, const Color& dst_color, const BlendFactor& src_factor, const BlendFactor& dst_factor, const BlendOp& op) {
			Color lhs, rhs;
			switch (src_factor) {
			case BlendFactor::ONE:
				lhs = src_color;
				break; 
			case BlendFactor::SRC_ALPHA:
				lhs = src_color * src_color.a;
				break;
			case BlendFactor::SRC_COLOR:
				lhs = src_color * src_color;
				break;
			case BlendFactor::ONE_MINUS_SRC_ALPHA:
				lhs = src_color * (1.0f - src_color);
				break;
			case BlendFactor::ONE_MINUS_SRC_COLOR:
				lhs = src_color * (1.0f - dst_color);
				break;
			case BlendFactor::DST_ALPHA:
				lhs = src_color * dst_color.a;
				break;
			case BlendFactor::DST_COLOR:
				lhs = src_color * dst_color;
				break;
			case BlendFactor::ONE_MINUS_DST_ALPHA:
				lhs = src_color * (1.0f - dst_color.a);
				break;
			case BlendFactor::ONE_MINUS_DST_COLOR:
				lhs = src_color * (1.0f - dst_color);
				break;
			}

			switch (dst_factor) {
			case BlendFactor::ONE:
				rhs = src_color;
				break;
			case BlendFactor::SRC_ALPHA:
				rhs = dst_color * src_color.a;
				break;
			case BlendFactor::SRC_COLOR:
				rhs = dst_color * src_color;
				break;
			case BlendFactor::ONE_MINUS_SRC_ALPHA:
				rhs = dst_color * (1.0f - src_color);
				break;
			case BlendFactor::ONE_MINUS_SRC_COLOR:
				rhs = dst_color * (1.0f - dst_color);
				break;
			case BlendFactor::DST_ALPHA:
				rhs = dst_color * dst_color.a;
				break;
			case BlendFactor::DST_COLOR:
				rhs = dst_color * dst_color;
				break;
			case BlendFactor::ONE_MINUS_DST_ALPHA:
				rhs = dst_color * (1.0f - dst_color.a);
				break;
			case BlendFactor::ONE_MINUS_DST_COLOR:
				rhs = dst_color * (1.0f - dst_color);
				break;
			}

			switch (op) {
			case BlendOp::ADD:
				return lhs + rhs;
			case BlendOp::SUB:
				return lhs - rhs;
			}
			return lhs + rhs;
		}

		bool depth_test(const ZTest& ztest_mode, const ZWrite& zwrite_mode, const uint32_t& row, const uint32_t& col, const float& z) {
			float depth;
			bool pass = false;
			if (zbuffer->read(row, col, depth)) {
				pass = z <= depth;
				switch (ztest_mode) {
				case ZTest::ALWAYS:
					pass = true;
					break;
				case ZTest::EQUAL:
					pass = EQUALS(z, depth); // percision concern
					break;
				case ZTest::GREATER:
					pass = z > depth;
					break;
				case ZTest::LEQUAL:
					pass = z <= depth;
					break;
				case ZTest::NOT_EQUAL:
					pass = z != depth;
					break;
				case ZTest::GEQUAL:
					pass = z >= depth;
					break;
				case ZTest::LESS:
					pass = z < depth;
					break;
				}
			}
			return pass;
		}

		Vertex clip2ndc(const Vertex& v) {
			Vertex ret = v;
			ret.position = clip2ndc(v.position);
			float w = ret.position.w;
			if (w == 0.0f) {
				w = EPSILON;
			}
			ret.rhw = 1.0f / w;
			return ret;
		}

		Vector4 clip2ndc(const Vector4& v) {
			Vector4 ndc_pos = v;
			float w = v.w;
			if (w == 0.0f) {
				w += EPSILON;
			}
			float rhw = 1.0f / w;
			ndc_pos *= rhw;
			ndc_pos.w = 1.0f;
			return ndc_pos;
		}

		Vertex ndc2viewport(const Vertex& v) {
			Vertex ret = v;
			ret.position = ndc2viewport(v.position);
			ret.rhw = 1.0f;
			return ret;
		}

		Vector4 ndc2viewport(const Vector4& v) {
			Vector4 viewport_pos;
			viewport_pos.x = (v.x + 1.0f) * this->width * 0.5f;
			viewport_pos.y = (v.y + 1.0f) * this->height * 0.5f;
			viewport_pos.z = v.z;
			viewport_pos.w = 1.0f;
			return viewport_pos;
		}

		float linearize_depth(const float& depth, const float& near, const float& far)
		{
			float ndc_z = depth * 2.0f - 1.0f;  // [0, 1] -> [-1, 1] (GL)
			return (2.0f * near * far) / (far + near - ndc_z * (far - near));
		}

	public:
		void draw_segment(const Vector3& start, const Vector3& end, const Color& col, const Matrix4x4& v, const Matrix4x4& p, const Vector2& screen_translation) {
			Vector4 clip_start = p * v * Vector4(start);
			Vector4 clip_end = p * v * Vector4(end);

			Vector4 n1 = clip2ndc(clip_start);
			Vector4 n2 = clip2ndc(clip_end);

			Vector4 s1 = ndc2viewport(n1);
			Vector4 s2 = ndc2viewport(n2);

			Matrix4x4 translation = Matrix4x4::translation(Vector3(screen_translation));

			s1 = translation * s1;
			s2 = translation * s2;

			SegmentDrawer::bresenham(framebuffer, (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, Color::encode_bgra(col));
		}

		void draw_screen_segment(const Vector4& start, const Vector4& end, const Color& col) {
			SegmentDrawer::bresenham(framebuffer, (int)start.x, (int)start.y, (int)end.x, (int)end.y, Color::encode_bgra(col));
		}

		void draw_segment(const Vector3& start, const Vector3& end, const Color& col, const Matrix4x4& v, const Matrix4x4& p) {
			Vector4 clip_start = p * v * Vector4(start);
			Vector4 clip_end = p * v * Vector4(end);

			Vector4 n1 = clip2ndc(clip_start);
			Vector4 n2 = clip2ndc(clip_end);

			Vector4 s1 = ndc2viewport(n1);
			Vector4 s2 = ndc2viewport(n2);

			SegmentDrawer::bresenham(framebuffer, (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, Color::encode_bgra(col));
		}

		void draw_coordinates(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& right, const Matrix4x4& v, const Matrix4x4& p, const Vector2& offset) {
			Graphics().draw_segment(pos, pos + forward, Color::BLUE, v, p, offset);
			Graphics().draw_segment(pos, pos + right, Color::RED, v, p, offset);
			Graphics().draw_segment(pos, pos + up, Color::GREEN, v, p, offset);
		}

		void draw_coordinates(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& right, const Matrix4x4& v, const Matrix4x4& p) {
			Graphics().draw_segment(pos, pos + forward, Color::BLUE, v, p);
			Graphics().draw_segment(pos, pos + right, Color::RED, v, p);
			Graphics().draw_segment(pos, pos + up, Color::GREEN, v, p);
		}
	};
}
#endif