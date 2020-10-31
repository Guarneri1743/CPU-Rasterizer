#pragma once
#include <raw_buffer.hpp>
#include <float4.hpp>
#include <transform.hpp>
#include <shader.hpp>
#include <material.hpp>
#include <aabb2d.hpp>
#include <triangle.hpp>
#include <shader_manager.hpp>
#include <texture_manager.hpp>
#include <line_drawer.hpp>
#include <color.hpp>

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
		render_device(void* fb, unsigned int width, unsigned int height) {
			this->width = width;
			this->height = height;
			this->background_color = 0;
			this->wire_frame_color = color::encode(1.0f, 1.0f, 1.0f, 1.0f);
			r_flag = render_flag::shaded;
			zbuffer = new raw_buffer<float>(width, height);
			zbuffer->clear(1.0f);
			frame_buffer = new raw_buffer<color_t>(fb, width, height);
		}

		~render_device() {
			delete zbuffer;
			delete frame_buffer;
			zbuffer = nullptr;
			frame_buffer = nullptr;
		}

	public:
		unsigned int width;
		unsigned int height;
		color_t background_color;
		color_t wire_frame_color;
		render_flag r_flag;

	private:
		raw_buffer<float>* zbuffer;
		raw_buffer<color_t>* frame_buffer;

	public:
		// =============================================================================================================Pipeline======================================================================================================================
		// *																																																										*
	    // *												 model matrix					view matrix							projection matrix                clipping, clip2ndc					    screen mapping								*
		// *  vertex data ------------>local(model) space  ----------------> world space ----------------> view(camera) space ---------------------> clip space ---------------------> ndc(cvv) space --------------------> screen space(viewport)  *
		// *																																																										*
		// *						  primitive assembly/rasterization																																												*
		// *  screen space vertices ------------------------------------->  fragments ------> scissor test -------> alpha test -------> stencil test ---------> depth test -------> blending --------> framebuffer									*
		// *																																																										*
		// ===========================================================================================================================================================================================================================================
		void draw_primitive(material& material, const vertex& v1, const vertex& v2, const vertex& v3, const mat4& m, const mat4& v, const mat4& p) {
			// early clip
			if (clipping(v1.position, v2.position, v3.position, m, v, p)) {
				return;
			}

			shader* shader = material.get_shader();

			shader->set_mvp_matrix(m, v, p);
			shader->sync(material.name2float, material.name2float4, material.name2int, material.name2tex);
			shader->sync(material.ztest_mode, material.zwrite_mode);
			shader->sync(material.transparent, material.src_factor, material.dst_factor, material.blend_op);

			assert(shader != nullptr);

			v2f o1 = process_vertex(*shader, v1, m, v, p);
			v2f o2 = process_vertex(*shader, v2, m, v, p);
			v2f o3 = process_vertex(*shader, v3, m, v, p);

			vertex c1(o1.position, o1.color, o1.normal, o1.uv, o1.tangent);
			vertex c2(o2.position, o2.color, o2.normal, o2.uv, o2.tangent);
			vertex c3(o3.position, o3.color, o3.normal, o3.uv, o3.tangent);

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
			
				line_drawer::bresenham(*this->frame_buffer, (int)tri[0].position.x, (int)tri[0].position.y, (int)tri[1].position.x, (int)tri[1].position.y, color::encode(1.0f, 1.0f, 1.0f, 1.0f));
				line_drawer::bresenham(*this->frame_buffer, (int)tri[0].position.x, (int)tri[0].position.y, (int)tri[2].position.x, (int)tri[2].position.y, color::encode(1.0f, 1.0f, 1.0f, 1.0f));
				line_drawer::bresenham(*this->frame_buffer, (int)tri[2].position.x, (int)tri[2].position.y, (int)tri[1].position.x, (int)tri[1].position.y, color::encode(1.0f, 1.0f, 1.0f, 1.0f));
			}

			// wireframe & scanline
			if (((int)r_flag & (int)render_flag::scanline) != 0) {

				for (auto iter = tris.begin(); iter != tris.end(); iter++) {
					auto& tri = *iter;
					line_drawer::bresenham(*this->frame_buffer, (int)tri[0].position.x, (int)tri[0].position.y, (int)tri[1].position.x, (int)tri[1].position.y, color::encode(1.0f, 1.0f, 1.0f, 1.0f));
					line_drawer::bresenham(*this->frame_buffer, (int)tri[0].position.x, (int)tri[0].position.y, (int)tri[2].position.x, (int)tri[2].position.y, color::encode(1.0f, 1.0f, 1.0f, 1.0f));
					line_drawer::bresenham(*this->frame_buffer, (int)tri[2].position.x, (int)tri[2].position.y, (int)tri[1].position.x, (int)tri[1].position.y, color::encode(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}

			// rasterization
			if (((int)r_flag & (int)render_flag::shaded) != 0 || ((int)r_flag & (int)render_flag::depth) != 0) {
				rasterize(tris, material);
			}
		}

		v2f process_vertex(shader& shader, const vertex& vert, const mat4& m, const mat4& v, const mat4& p) {
			a2v input;
			input.position = vert.position;
			input.uv = vert.uv;
			input.color = vert.color;
			input.normal = vert.normal;
			return shader.vertex_shader(input);
		}

		void rasterize(std::vector<triangle>& tris, material& mat) {
			bool flip = false;
			for (auto iter = tris.begin(); iter != tris.end(); iter++) {
				auto& tri = *iter;
				int top_idx = flip ? 2 : 0;
				int bottom_idx = flip ? 0 : 2;
				int top = CEIL(tri[top_idx].position.y);
				int bottom = CEIL(tri[bottom_idx].position.y);
				top = CLAMP(top, 0, this->height);
				bottom = CLAMP(bottom, 0, this->height);
				assert(bottom >= top);
				for (int row = top; row < bottom; row++) {
					vertex lhs;
					vertex rhs;
					tri.interpolate((float)row + 0.5f, lhs, rhs, flip);
					int left = CEIL(lhs.position.x);
					int right = CEIL(rhs.position.x);
					left = CLAMP(left, 0, this->width);
					right = CLAMP(right, 0, this->width);
					assert(right >= left);
					for (int col = left; col < right; col++) {
						process_fragment(lhs, row, col, mat);
						auto& dx = vertex::differential(lhs, rhs);
						lhs = vertex::intagral(lhs, dx);
					}
				}
				flip = true;
			}
		}

		void process_fragment(vertex& v, const unsigned int& row, const unsigned int& col, material& mat) {
			shader* s = mat.get_shader();
			float rhw = v.rhw;
			float z = v.position.z;
			float depth;

			ztest ztest_mode = s->ztest_mode;
			zwrite zwrite_mode = s->zwrite_mode;
			blend_factor src_factor = s->src_factor;
			blend_factor dst_factor = s->dst_factor;
			blend_operator blend_op = s->blend_op;

			color_t pixel_color = 0;

			// fragment shader
			color fragment_result;
			if (((int)r_flag & (int)render_flag::shaded) != 0 && s != nullptr) {
				v2f v_out;
				v_out.position = v.position;
				v_out.color = v.color;
				v_out.normal = v.normal;
				v_out.uv = v.uv;
				fragment_result = s->fragment_shader(v_out);
				pixel_color = color::encode(fragment_result);
			}else if (((int)r_flag & (int)render_flag::uv) != 0) {
				pixel_color = color::encode(v.uv);
			}
			else if (((int)r_flag & (int)render_flag::vertex_color) != 0) {
				pixel_color = color::encode(v.color);
			}

			// todo: scissor test

			// alpha blend
			if (s != nullptr && s->transparent) {
				color_t dst;
				if (frame_buffer->read(row, col, dst)) {
					color dst_color = color::decode(dst);
					color src_color = fragment_result;
					color blended_color = color::blend(src_color, dst_color, src_factor, dst_factor, blend_op);
					pixel_color = color::encode(blended_color.r, blended_color.g, blended_color.b, blended_color.a);
					if (dst != 0 && dst != -1) {
						int a = 0;
					}
				}
			}

			// todo: stencil test

			// depth-test
			bool z_pass = depth_test(ztest_mode, zwrite_mode, row, col, z);

			if (z_pass) {
				// write to color buffer
				frame_buffer->write(row, col, pixel_color);
			}

			// depth buffer visualization
			if (((int)r_flag & (int)render_flag::depth) != 0) {
				float depth;
				if (zbuffer->read(row, col, depth)) {
					float linear_depth = linearize_depth(depth, misc_params.camera_near, misc_params.camera_far);
					float3 depth_color = float3::ONE * linear_depth / 20.0f;
					color_t c = color::encode(depth_color.x, depth_color.y, depth_color.z, 1.0f);
					frame_buffer->write(row, col, c);
				}
			}
		}

		bool depth_test(const ztest& ztest_mode, const zwrite& zwrite_mode, const unsigned int& row, const unsigned int& col,const float& z) {
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
			auto& mvp = p * v * m;
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
			
		}

		bool in_frustum(const triangle& tri) {
			return false;
		}

		void clear_buffer() {
			zbuffer->clear(1.0f);
			if (((int)r_flag & (int)render_flag::depth) != 0)
			{
				frame_buffer->clear(color::encode(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else {
				frame_buffer->clear();
			}
		}
	};
}