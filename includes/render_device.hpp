#pragma once
#include <raw_buffer.hpp>
#include <framebuffer.hpp>
#include <float4.hpp>
#include <transform.hpp>
#include <shader.hpp>
#include <trapezoid.hpp>
#include <scanline.hpp>
#include <material.hpp>
#include <aabb2d.hpp>
#include <triangle.hpp>
#include <shader_manager.hpp>
#include <texture_manager.hpp>
#include <line_drawer.hpp>

namespace guarneri {
	enum class render_state {
		wire_frame,
		shaded
	};

	class render_device {
	public:
		render_device(void* fb, int width, int height) {
			this->width = width;
			this->height = height;
			this->background_color = 0;
			this->wire_frame_color = encode_color(1.0f, 1.0f, 1.0f);
			state = render_state::shaded;
			zbuffer = new raw_buffer<float>(width, height);
			frame_buffer = new framebuffer(fb, width, height);
		}

		~render_device() {
			delete zbuffer;
			delete frame_buffer;
			zbuffer = nullptr;
			frame_buffer = nullptr;
		}

	public:
		int width;
		int height;
		color_t background_color;
		color_t wire_frame_color;
		render_state state;

	private:
		raw_buffer<float>* zbuffer;
		framebuffer* frame_buffer;

	public:
		void update_global_uniforms() {

		}

		void draw_primitive(material& material, const vertex& v1, const vertex& v2, const vertex& v3, const mat4& m, const mat4& v, const mat4& p) {
			if (culling(v1.position, v2.position, v3.position, m, v, p)) {
				return;
			}
			shader* shader = shader_manager::singleton()->get_shader(material.get_shader_id());

			assert(shader != nullptr);

			v_output o1 = process_vertex(*shader, v1, m, v, p);
			v_output o2 = process_vertex(*shader, v2, m, v, p);
			v_output o3 = process_vertex(*shader, v3, m, v, p);

			float4 c1 = o1.position;
			float4 c2 = o2.position;
			float4 c3 = o3.position;

			float4 n1 = clip2ndc(c1);
			float4 n2 = clip2ndc(c2);
			float4 n3 = clip2ndc(c3);

			float4 s1 = ndc2viewport(n1);
			float4 s2 = ndc2viewport(n2);
			float4 s3 = ndc2viewport(n3);

			std::vector<trapezoid> trapezoids;
			trapezoids.reserve(2);
			trapezoids.resize(2);

			vertex p1 = v1;
			p1.position = float4(s1.xyz(), c1.w);
			p1 = p1.perspective_division();

			vertex p2 = v2;
			p2.position = float4(s2.xyz(), c2.w);
			p2 = p2.perspective_division();

			vertex p3 = v3;
			p3.position = float4(s3.xyz(), c3.w);
			p3 = p3.perspective_division();


			int n = trapezoid::generate_trapezoid(p1, p2, p3, trapezoids);

			if (n >= 1) render_trapezoid(material, trapezoids[0]);
			if (n >= 2) render_trapezoid(material, trapezoids[1]);

			line_drawer::bresenham(*this->frame_buffer, (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, this->wire_frame_color);
			line_drawer::bresenham(*this->frame_buffer, (int)s1.x, (int)s1.y, (int)s3.x, (int)s3.y, this->wire_frame_color);
			line_drawer::bresenham(*this->frame_buffer, (int)s3.x, (int)s3.y, (int)s2.x, (int)s2.y, this->wire_frame_color);
		}

		void render_trapezoid(material& material, trapezoid& trapezoid) {
			scanline scanline;
			int row;
			int top = CEIL(trapezoid.top);
			int bottom = CEIL(trapezoid.bottom);
			for (row = top; row < bottom; row++) {
				if (row >= 0 && row < this->height) {
					trapezoid.interpolate_lr_edge((float)row + 0.5f);
					scanline.next_step(trapezoid, row);
					scan(scanline, material);
				}
				else if (row >= this->height) {
					break;
				}
			}
		}

		void scan(scanline& scanline, material& mat) {
			int row = scanline.row;
			int col = scanline.col;
			int w = scanline.w;
			id_t id = mat.get_shader_id();
			shader* s = shader_manager::singleton()->get_shader(id);
			for (; w > 0; col++, w--) {
				if (col >= 0 && col < width) {
					float rhw = scanline.v.rhw;
					float depth = 0.0f;
					if (read_zbuffer(row, col, depth)) {
						// z-test pass
						if (rhw >= depth) {
							float original_w = 1.0f / rhw;
							// write z buffer
							write_zbuffer(row, col, rhw);
							// fragment shader
							if (s != nullptr) {
								v_output v_out;
								v_out.position = scanline.v.position;
								v_out.color = scanline.v.color;
								v_out.normal = scanline.v.normal;
								v_out.uv = scanline.v.uv;
								float4 ret = s->fragment_shader(v_out);
								color_t c = encode_color(ret.x, ret.y, ret.z);
								write_color_buffer(row, col, c);
							}
							else {
								color_t c = encode_color(scanline.v.color.x, scanline.v.color.y, scanline.v.color.z);
								write_color_buffer(row, col, c);
							}
						}
					}
				}
				scanline.v = vertex::add(scanline.v, scanline.step);
				if (col >= width) break;
			}
		}

		v_output process_vertex(shader& shader, const vertex& vert, const mat4& m, const mat4& v, const mat4& p) {
			v_input input;
			input.position = vert.position;
			input.uv = vert.uv;
			input.color = vert.color;
			input.normal = vert.normal;
			shader.set_model_matrix(m);
			shader.set_vp_matrix(v, p);
			return shader.vertex_shader(input);
		}

		float4 clip2ndc(const float4& v) {
			float4 ndc_pos = v;
			float rhw = 1.0f / v.w;
			ndc_pos *= rhw;
			ndc_pos.w = 1.0f;
			return ndc_pos;
		}

		float4 ndc2viewport(const float4& v) {
			float4 viewport_pos;
			viewport_pos.x = (v.x + 1.0f) * this->width * 0.5f;
			viewport_pos.y = (1.0f - v.y) * this->height * 0.5f;
			viewport_pos.z = v.z;
			viewport_pos.w = 1.0f;
			return viewport_pos;
		}

		bool culling(const float4& v1, const float4& v2, const float4& v3, const mat4& m, const mat4& v, const mat4& p) {
			auto mvp = p * v * m;
			float4 c1 = mvp * v1;
			float4 c2 = mvp * v2;
			float4 c3 = mvp * v3;

			if (cvv_culling(c1) != 0) return true;
			if (cvv_culling(c2) != 0) return true;
			if (cvv_culling(c3) != 0) return true;

			return false;
		}

		int cvv_culling(const float4& v) {
			float w = v.w;
			int check = 0;
			// z: [-w, w](GL) [0, w](DX)
			// x: [-w, w]
			// y: [-w, w]
			if (v.z < -w) check |= 1;
			if (v.z > w) check |= 2;
			if (v.x < -w) check |= 4;
			if (v.x > w) check |= 8;
			if (v.y < -w) check |= 16;
			if (v.y > w) check |= 32;
			return check;
		}

		bool read_zbuffer(const int& x, const int& y, float& ret) {
			return zbuffer->read(x, y, ret);
		}

		bool write_zbuffer(const int& x, const int& y, const float& ret) {
			return zbuffer->write(x, y, ret);
		}

		void clear_zbuffer() {
			zbuffer->clear();
		}

		bool read_color_buffer(const int& x, const int& y, color_t& ret) {
			return frame_buffer->read(x, y, ret);
		}

		bool write_color_buffer(const int& x, const int& y, const color_t& ret) {
			return frame_buffer->write(x, y, ret);
		}

		void clear_color_buffer() {
			frame_buffer->clear();
		}

		void clear() {
			zbuffer->clear();
			frame_buffer->clear();
		}
	};
}