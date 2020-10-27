#pragma once
#include <vertex.hpp>
#include <trapezoid.hpp>
#include <raw_buffer.hpp>
#include <material.hpp>

namespace guarneri {
	struct scanline {
		vertex v;
		vertex step;
		int row;
		int col;
		int w;

	public:
		void next_step(const trapezoid& trap, int r) {
			float width = trap.right.v.position.x - trap.left.v.position.x;
			this->col = CEIL(trap.left.v.position.x);
			this->w = CEIL(trap.right.v.position.x) - this->col;
			this->row = r;
			this->v = trap.left.v;
			if (trap.left.v.position.x >= trap.right.v.position.x){
				this->w = 0;
			}
			this->step = vertex::perspective_division(trap.left.v, trap.right.v, width);
		}
	};
}