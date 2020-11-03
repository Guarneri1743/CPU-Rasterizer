#pragma once
#include <guarneri.hpp>

namespace guarneri {
	enum class render_flag {
		disable = 0,
		wire_frame = 1 << 0,
		shaded = 1 << 1,
		depth = 1 << 2,
		uv = 1 << 3,
		vertex_color = 1 << 4,
		scanline = 1 << 5
	};

	class render_device {
	public:
		uint32_t width;
		uint32_t height;
		render_flag r_flag;

	private:
		std::shared_ptr<raw_buffer<float>> zbuffer;
		std::shared_ptr<raw_buffer<color_bgra>> framebuffer;

	public:
		void initialize(void* bitmap_handle_t, uint32_t width_t, uint32_t height_t) {
			this->width = width_t;
			this->height = height_t;
			r_flag = render_flag::shaded;
			zbuffer = std::make_shared<raw_buffer<float>>(width_t, height_t);
			framebuffer = std::make_shared<raw_buffer<color_bgra>>(bitmap_handle_t, width_t, height_t, [](color_bgra* ptr) { unused(ptr); /*delete[] (void*)ptr;*/ });
			zbuffer->clear(1.0f);
		}

		// =============================================================================================================Pipeline======================================================================================================================
		// *																																																										*
		// *												 model matrix					view matrix							projection matrix                clipping, clip2ndc					    screen mapping								*
		// *  vertex data ------------>local(model) space  ----------------> world space ----------------> view(camera) space ---------------------> clip space ---------------------> ndc(cvv) space --------------------> screen space(viewport)  *
		// *																																																										*
		// *						  primitive assembly/rasterization																																												*
		// *  screen space vertices ------------------------------------->  fragments ------> scissor test -------> alpha test -------> stencil test ---------> depth test -------> blending --------> framebuffer									*
		// *																																																										*
		// ===========================================================================================================================================================================================================================================
		void draw_primitive(std::shared_ptr<material>  material, const vertex& v1, const vertex& v2, const vertex& v3, const mat4& m, const mat4& v, const mat4& p) {
			// early clip
			if (clipping(v1.position, v2.position, v3.position, m, v, p)) {
				return;
			}

			auto shader = material->get_shader();

			shader->set_mvp_matrix(m, v, p);
			shader->sync(material->name2float, material->name2float4, material->name2int, material->name2tex);
			shader->sync(material->ztest_mode, material->zwrite_mode);
			shader->sync(material->transparent, material->src_factor, material->dst_factor, material->blend_op);

			assert(shader != nullptr);

			v2f o1 = process_vertex(shader, v1);
			v2f o2 = process_vertex(shader, v2);
			v2f o3 = process_vertex(shader, v3);

			vertex c1(o1.position, o1.color, o1.normal, o1.uv, o1.tangent, o1.bitangent);
			vertex c2(o2.position, o2.color, o2.normal, o2.uv, o2.tangent, o2.bitangent);
			vertex c3(o3.position, o3.color, o3.normal, o3.uv, o3.tangent, o3.bitangent);

			vertex n1 = clip2ndc(c1);
			vertex n2 = clip2ndc(c2);
			vertex n3 = clip2ndc(c3);

			vertex s1 = ndc2viewport(n1);
			vertex s2 = ndc2viewport(n2);
			vertex s3 = ndc2viewport(n3);

			s1.position.w = c1.position.w;
			s1.rhw = c1.rhw;

			s2.position.w = c2.position.w;
			s2.rhw = c2.rhw;

			s3.position.w = c3.position.w;
			s3.rhw = c3.rhw;

			triangle tri(s1, s2, s3);

			// primitive assembly
			std::vector<triangle> tris = tri.horizontal_split();

			// wireframe
			if (((int)r_flag & (int)render_flag::wire_frame) != 0) {

				line_drawer::bresenham(framebuffer, (int)tri[0].position.x, (int)tri[0].position.y, (int)tri[1].position.x, (int)tri[1].position.y, color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
				line_drawer::bresenham(framebuffer, (int)tri[0].position.x, (int)tri[0].position.y, (int)tri[2].position.x, (int)tri[2].position.y, color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
				line_drawer::bresenham(framebuffer, (int)tri[2].position.x, (int)tri[2].position.y, (int)tri[1].position.x, (int)tri[1].position.y, color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
			}

			// wireframe & scanline
			if (((int)r_flag & (int)render_flag::scanline) != 0) {

				for (auto iter = tris.begin(); iter != tris.end(); iter++) {
					auto& t = *iter;
					line_drawer::bresenham(framebuffer, (int)t[0].position.x, (int)t[0].position.y, (int)t[1].position.x, (int)t[1].position.y, color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
					line_drawer::bresenham(framebuffer, (int)t[0].position.x, (int)t[0].position.y, (int)t[2].position.x, (int)t[2].position.y, color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
					line_drawer::bresenham(framebuffer, (int)t[2].position.x, (int)t[2].position.y, (int)t[1].position.x, (int)t[1].position.y, color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}

			// rasterization
			if (((int)r_flag & (int)render_flag::shaded) != 0 || ((int)r_flag & (int)render_flag::depth) != 0) {
				rasterize(tris, material);
			}
		}

		v2f process_vertex(const std::shared_ptr<shader>& shader, const vertex& vert) {
			a2v input;
			input.position = vert.position;
			input.uv = vert.uv;
			input.color = vert.color;
			input.normal = vert.normal;
			return shader->vertex_shader(input);
		}

		void rasterize(std::vector<triangle>& tris, const std::shared_ptr<material>& mat) {
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
				for (int row = top; row < bottom; row++) {
					vertex lhs;
					vertex rhs;
					tri.interpolate((float)row + 0.5f, lhs, rhs);
					int left = CEIL(lhs.position.x);
					int right = CEIL(rhs.position.x);
					left = CLAMP_INT(left, 0, this->width);
					right = CLAMP_INT(right, 0, this->width);
					assert(right >= left);
					for (int col = left; col < right; col++) {
						process_fragment(lhs, row, col, mat);
						auto dx = vertex::differential(lhs, rhs);
						lhs = vertex::intagral(lhs, dx);
					}
				}
			}
		}

		void process_fragment(vertex& v, const uint32_t& row, const uint32_t& col, std::shared_ptr<material>  mat) {
			auto s = mat->get_shader();
			float z = v.position.z;

			ztest ztest_mode = s->ztest_mode;
			zwrite zwrite_mode = s->zwrite_mode;
			blend_factor src_factor = s->src_factor;
			blend_factor dst_factor = s->dst_factor;
			blend_operator blend_op = s->blend_op;

			color_bgra pixel_color;

			// fragment shader
			color fragment_result;
			if (((int)r_flag & (int)render_flag::shaded) != 0 && s != nullptr) {
				v2f v_out;
				v_out.position = v.position;
				v_out.color = v.color;
				v_out.normal = v.normal;
				v_out.uv = v.uv;
				fragment_result = s->fragment_shader(v_out);
				pixel_color = color::encode_bgra(fragment_result);
			}
			else if (((int)r_flag & (int)render_flag::uv) != 0) {
				pixel_color = color::encode_bgra(v.uv);
			}
			else if (((int)r_flag & (int)render_flag::vertex_color) != 0) {
				pixel_color = color::encode_bgra(v.color);
			}

			// todo: scissor test

			// alpha blend
			if (s != nullptr && s->transparent) {
				color_bgra dst;
				if (framebuffer->read(row, col, dst)) {
					color dst_color = color::decode(dst);
					color src_color = fragment_result;
					color blended_color = color::blend(src_color, dst_color, src_factor, dst_factor, blend_op);
					pixel_color = color::encode_bgra(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
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
			if (((int)r_flag & (int)render_flag::depth) != 0) {
				float cur_depth;
				if (zbuffer->read(row, col, cur_depth)) {
					float linear_depth = linearize_depth(cur_depth, camera::main_camera->near, camera::main_camera->far);
					float3 depth_color = float3::ONE * linear_depth / 20.0f;
					color_bgra c = color::encode_bgra(depth_color.x, depth_color.y, depth_color.z, 1.0f);
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
					if (zwrite_mode == zwrite::on) {
						zbuffer->write(row, col, z);
					}
				}
			}
			return pass;
		}

		vertex clip2ndc(const vertex& v) {
			vertex ret = v;
			ret.position = clip2ndc(v.position);
			ret.rhw = 1.0f / ret.position.w;
			return ret;
		}

		float4 clip2ndc(const float4& v) {
			float4 ndc_pos = v;
			float rhw = 1.0f / v.w;
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

		float4 ndc2viewport(const float4& v) {
			float4 viewport_pos;
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

		bool clipping(const float4& v1, const float4& v2, const float4& v3, const mat4& m, const mat4& v, const mat4& p) {
			auto mvp = p * v * m;
			float4 c1 = mvp * v1; float4 c2 = mvp * v2; float4 c3 = mvp * v3;
			return clipping(c1) && clipping(c2) && clipping(c3);
		}

		bool clipping(const float4& v) {
			// z: [-w, w](GL) [0, w](DX)
			// x: [-w, w]
			// y: [-w, w]
			float w = v.w; float x = v.x; float y = v.y; float z = v.y;
			return (z < -w) || (z > w) || (x < -w) || (x > w) || (y < -w) || (y > w);
		}

		// todo
		void generate_frustum(mat4 v, mat4 p)
		{
			REF(p);
			REF(v);
		}

		bool in_frustum(const triangle& tri) {
			REF(tri);
			return false;
		}

		void clear_buffer() {
			zbuffer->clear(1.0f);
			if (((int)r_flag & (int)render_flag::depth) != 0)
			{
				framebuffer->clear(color::encode_bgra(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else {
				framebuffer->clear(color::encode_bgra(0.0f, 0.0f, 0.0f, 0.0f));
			}
		}
	};
}