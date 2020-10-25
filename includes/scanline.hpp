#pragma once
#include <vertex.hpp>
#include <trapezoid.hpp>
#include <raw_buffer.hpp>
#include <material.hpp>

namespace guarneri {
	struct scanline {
		vertex v;
		vertex step;
		int x;
		int y;
		int w;

	public:
		void next_step(const trapezoid& trap, int offset) {
			float width = trap.right.v.position.x - trap.left.v.position.x;
			this->x = CEIL(trap.left.v.position.x);
			this->w = CEIL(trap.right.v.position.x) - this->x;
			this->y = offset;
			this->v = trap.left.v;
			if (trap.left.v.position.x >= trap.right.v.position.x){
				this->w = 0;
			}
			this->step = vertex::perspective_division(trap.left.v, trap.right.v, width);
		}
	};
}