#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	class GraphicsDevice {
	public:
		uint32_t width;
		uint32_t height;

	private:
		std::shared_ptr<RawBuffer<float>> zbuffer;
		std::shared_ptr<RawBuffer<color_bgra>> framebuffer;

	public:
		void initialize(void* bitmap_handle_t, uint32_t width_t, uint32_t height_t) {
			this->width = width_t;
			this->height = height_t;
			zbuffer = std::make_shared<RawBuffer<float>>(width_t, height_t);
			framebuffer = std::make_shared<RawBuffer<color_bgra>>(bitmap_handle_t, width_t, height_t, [](color_bgra* ptr) { unused(ptr); /*delete[] (void*)ptr;*/ });
			zbuffer->clear(1.0f);
		}

		void update() {
			
		}

		// =============================================================================================================Pipeline======================================================================================================================
		// *																																																										*
		// *												 Model matrix					view matrix							Projection matrix                clipping, clip2ndc					    screen mapping								*
		// *  Vertex data ------------>local(Model) space  ----------------> world space ----------------> view(Camera) space ---------------------> clip space ---------------------> ndc(cvv) space --------------------> screen space(viewport)  *
		// *																																																										*
		// *						  primitive assembly/rasterization																																												*
		// *  screen space vertices ------------------------------------->  fragments ------> scissor test -------> alpha test -------> stencil test ---------> depth test -------> blending --------> framebuffer									*
		// *																																																										*
		// ===========================================================================================================================================================================================================================================
		void draw_primitive(std::shared_ptr<Material> material, const Vertex& v1, const Vertex& v2, const Vertex& v3, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p) {
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

			if (!material->skybox && clipping(c1.position, c2.position, c3.position)) {
				return;
			}

			// position perspective division
			Vertex n1 = clip2ndc(c1);
			Vertex n2 = clip2ndc(c2);
			Vertex n3 = clip2ndc(c3);

			bool double_face = material->double_face;

			if (!double_face && !material->skybox && backface_culling(n1.position, n2.position, n3.position)) {
				return;
			}

			// other channels perspective division
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
			std::vector<Triangle> tris = tri.horizontal_split();

			// rasterization
			rasterize(tris, material);

			// wireframe
			if (((int)misc_param.flag & (int)RenderFlag::WIREFRAME) != 0) {

				SegmentDrawer::bresenham(framebuffer, (int)tri[0].position.x, (int)tri[0].position.y, (int)tri[1].position.x, (int)tri[1].position.y, Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
				SegmentDrawer::bresenham(framebuffer, (int)tri[0].position.x, (int)tri[0].position.y, (int)tri[2].position.x, (int)tri[2].position.y, Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
				SegmentDrawer::bresenham(framebuffer, (int)tri[2].position.x, (int)tri[2].position.y, (int)tri[1].position.x, (int)tri[1].position.y, Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
			}

			// wireframe & scanline
			if (((int)misc_param.flag & (int)RenderFlag::SCANLINE) != 0) {

				for (auto iter = tris.begin(); iter != tris.end(); iter++) {
					auto& t = *iter;
					SegmentDrawer::bresenham(framebuffer, (int)t[0].position.x, (int)t[0].position.y, (int)t[1].position.x, (int)t[1].position.y, Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
					SegmentDrawer::bresenham(framebuffer, (int)t[0].position.x, (int)t[0].position.y, (int)t[2].position.x, (int)t[2].position.y, Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
					SegmentDrawer::bresenham(framebuffer, (int)t[2].position.x, (int)t[2].position.y, (int)t[1].position.x, (int)t[1].position.y, Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
		}

		void draw_line(const Vector3& start, const Vector3& end, const Color& col, const Matrix4x4& v, const Matrix4x4& p, const Vector2& screen_translation) {
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

		void draw_coordinates(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& right, const Matrix4x4& v, const Matrix4x4& p, const Vector2& offset) {
			graphics().draw_line(pos, pos + forward, Color::BLUE, v, p, offset);
			graphics().draw_line(pos, pos + right, Color::RED, v, p, offset);
			graphics().draw_line(pos, pos + up, Color::GREEN, v, p, offset);
		}

		void draw_coordinates(const Vector3& pos, const Vector3& forward, const Vector3& up, const Vector3& right, const Matrix4x4& v, const Matrix4x4& p) {
			graphics().draw_line(pos, pos + forward, Color::BLUE, v, p);
			graphics().draw_line(pos, pos + right, Color::RED, v, p);
			graphics().draw_line(pos, pos + up, Color::GREEN, v, p);
		}

		void draw_line(const Vector3& start, const Vector3& end, const Color& col, const Matrix4x4& v, const Matrix4x4& p) {
			Vector4 clip_start = p * v * Vector4(start);
			Vector4 clip_end = p * v * Vector4(end);

			Vector4 n1 = clip2ndc(clip_start);
			Vector4 n2 = clip2ndc(clip_end);

			Vector4 s1 = ndc2viewport(n1);
			Vector4 s2 = ndc2viewport(n2);

			SegmentDrawer::bresenham(framebuffer, (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, Color::encode_bgra(col));
		}

		v2f process_vertex(const std::shared_ptr<Shader>& shader, const Vertex& vert) {
			a2v input;
			input.position = vert.position;
			input.uv = vert.uv;
			input.color = vert.color;
			input.normal = vert.normal;
			return shader->vertex_shader(input);
		}

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

					// clipping
					if (lhs.position.x <= 0.0f) {
						float t = -lhs.position.x / (rhs.position.x - lhs.position.x);
						lhs = Vertex::interpolate(lhs, rhs, t);
					}
					int left = CEIL(lhs.position.x);
					left = CLAMP_INT(left, 0, this->width);

					if (rhs.position.x >= this->width) {
						float t = ((float)this->width - lhs.position.x) / (rhs.position.x - lhs.position.x);
						rhs = Vertex::interpolate(lhs, rhs, t);
					}
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

		void process_fragment(Vertex& v, const uint32_t& row, const uint32_t& col, std::shared_ptr<Material>  mat) {
			auto s = mat->target_shader;
			float z = v.position.z;

			ZTest ztest_mode = s->ztest_mode;
			ZWrite zwrite_mode = s->zwrite_mode;
			BlendFactor src_factor = s->src_factor;
			BlendFactor dst_factor = s->dst_factor;
			BlendOp blend_op = s->blend_op;

			color_bgra pixel_color;

			// fragment Shader
			Color fragment_result;
			if (((int)misc_param.flag & (int)RenderFlag::SHADED) != 0 && s != nullptr) {
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

			// alpha blend
			if (s != nullptr && s->transparent && ((int)misc_param.flag & (int)RenderFlag::SEMI_TRANSPARENT) != 0) {
				color_bgra dst;
				if (framebuffer->read(row, col, dst)) {
					Color dst_color = Color::decode(dst);
					Color src_color = fragment_result;
					Color blended_color = Color::blend(src_color, dst_color, src_factor, dst_factor, blend_op);
					pixel_color = Color::encode_bgra(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
				}
			}

			// todo: stencil test

			// depth-test
			bool z_pass = depth_test(ztest_mode, zwrite_mode, row, col, z);

			if (z_pass) {
				// write to color buffer
				framebuffer->write(row, col, pixel_color);
			}

			// depth buffer visualization
			if (((int)misc_param.flag & (int)RenderFlag::DEPTH) != 0) {
				float cur_depth;
				if (zbuffer->read(row, col, cur_depth)) {
					float linear_depth = linearize_depth(cur_depth, misc_param.cam_near, misc_param.cam_far);
					Vector3 depth_color = Vector3::ONE * linear_depth / 30.0f;
					color_bgra c = Color::encode_bgra(depth_color.x, depth_color.y, depth_color.z, 1.0f);
					framebuffer->write(row, col, c);
				}
			}
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

				if (pass) {
					// write z buffer 
					if (zwrite_mode == ZWrite::ON || ((int)misc_param.flag & (int)RenderFlag::SEMI_TRANSPARENT) == 0) {
						zbuffer->write(row, col, z);
					}
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

		bool backface_culling(const Vector4& v1, const Vector4& v2, const Vector4& v3) {
			auto v1v2 = v2 - v2;
			auto v1v3 = v3 - v1;
			float ndv = Vector3::dot(Vector3::cross(v1v2.xyz(), v1v3.xyz()), Vector3::BACK);
			return ndv < 0; // front face: ndv > 0
		}

		bool clipping(const Vector4& c1, const Vector4& c2, const Vector4& c3) {
			// z: [-w, w](GL) [0, w](DX)
			// x: [-w, w]
			// y: [-w, w]

			// clip near and far planes here, clip left, right, top and bottom planes at scanline stage
			if (c1.w <= 0.0f || c2.w <= 0.0f || c3.w <= 0.0f) {
				std::cerr << c1 << ", " << c2 << ", " << c3 << std::endl;
				return true;
			}
			if (c1.z > c1.w || c2.z > c2.w || c3.z > c3.w) {
				return true;
			}
			if (c1.z < -c1.w || c2.z < -c2.w || c3.z < -c3.w) {
				return true;
			}
			return false;
		}

		bool frustum_culling(const BoundingBox& aabb, const Matrix4x4& v, const Matrix4x4& p) {
			auto frustum = Frustum::create(v, p);
			auto bmin = aabb.min();
			auto bmax = aabb.max();
			Vector3 corners[8];
			corners[0] = Vector3(bmin.x, bmin.y, bmin.z);
			corners[1] = Vector3(bmin.x, bmin.y, bmax.z);
			corners[2] = Vector3(bmin.x, bmax.y, bmin.z);
			corners[3] = Vector3(bmin.x, bmax.y, bmax.z);
			corners[4] = Vector3(bmax.x, bmin.y, bmin.z);
			corners[5] = Vector3(bmax.x, bmin.y, bmax.z);
			corners[6] = Vector3(bmax.x, bmax.y, bmin.z);
			corners[7] = Vector3(bmax.x, bmax.y, bmax.z);
			for (int i = 0; i < 6; i++) {
				auto plane = frustum[i];
				for (int j = 0; j < 8; j++) {
					if (plane.distance(corners[8]) > 0) return false;
				}
			}
			return true;
		}

		void clear_buffer() {
			zbuffer->clear(1.0f);
			if (((int)misc_param.flag & (int)RenderFlag::DEPTH) != 0)
			{
				framebuffer->clear(Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else {
				framebuffer->clear(Color::encode_bgra(0.0f, 0.0f, 0.0f, 0.0f));
			}
		}
	};
}