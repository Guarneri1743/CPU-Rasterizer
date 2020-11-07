#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	class render_device {
	public:
		uint32_t width;
		uint32_t height;

	private:
		std::shared_ptr<raw_buffer<float>> zbuffer;
		std::shared_ptr<raw_buffer<color_bgra>> framebuffer;

	public:
		void initialize(void* bitmap_handle_t, uint32_t width_t, uint32_t height_t) {
			this->width = width_t;
			this->height = height_t;
			zbuffer = std::make_shared<raw_buffer<float>>(width_t, height_t);
			framebuffer = std::make_shared<raw_buffer<color_bgra>>(bitmap_handle_t, width_t, height_t, [](color_bgra* ptr) { unused(ptr); /*delete[] (void*)ptr;*/ });
			zbuffer->clear(1.0f);
		}

		void update() {
			
		}

		// =============================================================================================================Pipeline======================================================================================================================
		// *																																																										*
		// *												 Model matrix					view matrix							projection matrix                clipping, clip2ndc					    screen mapping								*
		// *  vertex data ------------>local(Model) space  ----------------> world space ----------------> view(Camera) space ---------------------> clip space ---------------------> ndc(cvv) space --------------------> screen space(viewport)  *
		// *																																																										*
		// *						  primitive assembly/rasterization																																												*
		// *  screen space vertices ------------------------------------->  fragments ------> scissor test -------> alpha test -------> stencil test ---------> depth test -------> blending --------> framebuffer									*
		// *																																																										*
		// ===========================================================================================================================================================================================================================================
		void draw_primitive(std::shared_ptr<Material>  Material, const vertex& v1, const vertex& v2, const vertex& v3, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p) {
			// early clip
			/*if (clipping(v1.position, v2.position, v3.position, m, v, p)) {
				return;
			}*/

			auto shader = Material->get_shader();

			shader->set_mvp_matrix(m, v, p);
			shader->sync(Material->name2float, Material->name2float4, Material->name2int, Material->name2tex);
			shader->sync(Material->ztest_mode, Material->zwrite_mode);
			shader->sync(Material->transparent, Material->src_factor, Material->dst_factor, Material->blend_op);
			shader->sync(Material->lighting_param);

			assert(shader != nullptr);

			v2f o1 = process_vertex(shader, v1);
			v2f o2 = process_vertex(shader, v2);
			v2f o3 = process_vertex(shader, v3);

			vertex c1(o1.position, o1.world_pos, o1.Color, o1.normal, o1.uv, o1.tangent, o1.bitangent);
			vertex c2(o2.position, o2.world_pos, o2.Color, o2.normal, o2.uv, o2.tangent, o2.bitangent);
			vertex c3(o3.position, o3.world_pos, o3.Color, o3.normal, o3.uv, o3.tangent, o3.bitangent);

			// position perspective division
			vertex n1 = clip2ndc(c1);
			vertex n2 = clip2ndc(c2);
			vertex n3 = clip2ndc(c3);

			// other channels perspective division
			n1.perspective_division(c1.rhw);
			n2.perspective_division(c2.rhw);
			n3.perspective_division(c3.rhw);

			vertex s1 = ndc2viewport(n1);
			vertex s2 = ndc2viewport(n2);
			vertex s3 = ndc2viewport(n3);

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
			rasterize(tris, Material);

			// wireframe
			if (((int)misc_param.flag & (int)render_flag::wire_frame) != 0) {

				SegmentDrawer::bresenham(framebuffer, (int)tri[0].position.x, (int)tri[0].position.y, (int)tri[1].position.x, (int)tri[1].position.y, Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
				SegmentDrawer::bresenham(framebuffer, (int)tri[0].position.x, (int)tri[0].position.y, (int)tri[2].position.x, (int)tri[2].position.y, Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
				SegmentDrawer::bresenham(framebuffer, (int)tri[2].position.x, (int)tri[2].position.y, (int)tri[1].position.x, (int)tri[1].position.y, Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
			}

			// wireframe & scanline
			if (((int)misc_param.flag & (int)render_flag::scanline) != 0) {

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

		v2f process_vertex(const std::shared_ptr<shader>& shader, const vertex& vert) {
			a2v input;
			input.position = vert.position;
			input.uv = vert.uv;
			input.Color = vert.Color;
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
					vertex lhs;
					vertex rhs;
					tri.interpolate((float)row + 0.5f, lhs, rhs);
					// todo: clip Triangle to polygons in order to avoid these two steps
					if (lhs.position.x <= 0.0f) {
						float t = -lhs.position.x / (rhs.position.x - lhs.position.x);
						lhs = vertex::interpolate(lhs, rhs, t);
					}
					int left = CEIL(lhs.position.x);
					left = CLAMP_INT(left, 0, this->width);

					if (rhs.position.x >= this->width) {
						float t = ((float)this->width - lhs.position.x) / (rhs.position.x - lhs.position.x);
						rhs = vertex::interpolate(lhs, rhs, t);
					}
					int right = CEIL(rhs.position.x);
					right = CLAMP_INT(right, 0, this->width);

					assert(right >= left);

					// interpolate horizontally
					for (int col = left; col < right; col++) {
						process_fragment(lhs, row, col, mat);
						auto dx = vertex::differential(lhs, rhs);
						lhs = vertex::intagral(lhs, dx);
					}
				}
			}
		}

		void process_fragment(vertex& v, const uint32_t& row, const uint32_t& col, std::shared_ptr<Material>  mat) {
			auto s = mat->get_shader();
			float z = v.position.z;

			ztest ztest_mode = s->ztest_mode;
			zwrite zwrite_mode = s->zwrite_mode;
			blend_factor src_factor = s->src_factor;
			blend_factor dst_factor = s->dst_factor;
			blend_operator blend_op = s->blend_op;

			color_bgra pixel_color;

			// fragment shader
			Color fragment_result;
			if (((int)misc_param.flag & (int)render_flag::shaded) != 0 && s != nullptr) {
				v2f v_out;
				float w = 1.0f / v.rhw;
				v_out.position = v.position;
				v_out.world_pos = v.world_pos * w;
				v_out.Color = v.Color * w;
				v_out.normal = v.normal * w;
				v_out.uv = v.uv * w;
				fragment_result = s->fragment_shader(v_out);
				pixel_color = Color::encode_bgra(fragment_result);
			}

			// todo: scissor test

			// alpha blend
			if (s != nullptr && s->transparent && ((int)misc_param.flag & (int)render_flag::transparent) != 0) {
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
				// write to Color buffer
				framebuffer->write(row, col, pixel_color);
			}

			// depth buffer visualization
			if (((int)misc_param.flag & (int)render_flag::depth) != 0) {
				float cur_depth;
				if (zbuffer->read(row, col, cur_depth)) {
					float linear_depth = linearize_depth(cur_depth, misc_param.cam_near, misc_param.cam_far);
					Vector3 depth_color = Vector3::ONE * linear_depth / 20.0f;
					color_bgra c = Color::encode_bgra(depth_color.x, depth_color.y, depth_color.z, 1.0f);
					framebuffer->write(row, col, c);
				}
			}
		}

		bool depth_test(const ztest& ztest_mode, const zwrite& zwrite_mode, const uint32_t& row, const uint32_t& col, const float& z) {
			float depth;
			bool pass = false;
			if (zbuffer->read(row, col, depth)) {
				pass = z <= depth;
				switch (ztest_mode) {
				case ztest::always:
					pass = true;
					break;
				case ztest::equal:
					pass = EQUALS(z, depth); // percision concern
					break;
				case ztest::greater:
					pass = z > depth;
					break;
				case ztest::less_equal:
					pass = z <= depth;
					break;
				case ztest::not_equal:
					pass = z != depth;
					break;
				case ztest::greater_equal:
					pass = z >= depth;
					break;
				case ztest::less:
					pass = z < depth;
					break;
				}

				if (pass) {
					// write z buffer 
					if (zwrite_mode == zwrite::on || ((int)misc_param.flag & (int)render_flag::transparent) == 0) {
						zbuffer->write(row, col, z);
					}
				}
			}
			return pass;
		}

		vertex clip2ndc(const vertex& v) {
			vertex ret = v;
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

		vertex ndc2viewport(const vertex& v) {
			vertex ret = v;
			ret.position = ndc2viewport(v.position);
			ret.rhw = 1.0f;
			return ret;
		}

		Vector4 ndc2viewport(const Vector4& v) {
			Vector4 viewport_pos;
			viewport_pos.x = (v.x + 1.0f) * this->width * 0.5f;
			viewport_pos.y = (1.0f - v.y) * this->height * 0.5f;
			viewport_pos.z = v.z;
			viewport_pos.w = 1.0f;
			return viewport_pos;
		}

		float linearize_depth(const float& depth, const float& near, const float& far)
		{
			float ndc_z = depth * 2.0f - 1.0f;  // [0, 1] -> [-1, 1] (GL)
			return (2.0f * near * far) / (far + near - ndc_z * (far - near));
		}

		bool clipping(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Matrix4x4& m, const Matrix4x4& v, const Matrix4x4& p) {
			auto mvp = p * v * m;
			Vector4 c1 = mvp * v1; Vector4 c2 = mvp * v2; Vector4 c3 = mvp * v3;
			return clipping(c1) && clipping(c2) && clipping(c3);
		}

		bool clipping(const Vector4& v) {
			// z: [-w, w](GL) [0, w](DX)
			// x: [-w, w]
			// y: [-w, w]
			float w = v.w; float x = v.x; float y = v.y; float z = v.y;
			return (z < -w) || (z > w) || (x < -w) || (x > w) || (y < -w) || (y > w);
		}

		// todo
		void generate_frustum(Matrix4x4 v, Matrix4x4 p)
		{
			REF(p);
			REF(v);
		}

		bool in_frustum(const Triangle& tri) {
			REF(tri);
			return false;
		}

		void clear_buffer() {
			zbuffer->clear(1.0f);
			if (((int)misc_param.flag & (int)render_flag::depth) != 0)
			{
				framebuffer->clear(Color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else {
				framebuffer->clear(Color::encode_bgra(0.0f, 0.0f, 0.0f, 0.0f));
			}
		}
	};
}