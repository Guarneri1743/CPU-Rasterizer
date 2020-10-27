#pragma once
#include <common.hpp>
#include <framebuffer.hpp>

namespace guarneri {
	class line_drawer {
	public:
		static void dda(framebuffer& buffer, int x0, int y0, int x1, int y1, color_t c){
			int dx = std::abs(x1 - x0);
			int dy = std::abs(y1 - y0);
			float sx, sy, length;
			if (dx >= dy) {
				length = dx;
			}
			else {
				length = dy;
			}
			sx = (float)(x1 - x0) / length;
			sy = (float)(y1 - y0) / length;
			for (float i = 1, x = x0, y = y0; i <= length; i++) {
				buffer.write((int)(y + 0.5f), (int)(x + 0.5f), c);
				x += sx;
				y += sy;
			}
		}

		static void bresenham(framebuffer& buffer, int x0, int y0, int x1, int y1, color_t c){
			int dx = std::abs(x1 - x0);
			int dy = std::abs(y1 - y0);
			int sx = x0 < x1 ? 1 : -1;
			int sy = y0 < y1 ? 1 : -1;
			int bias = dx > dy ? dx : -dy;
			int dx2 = 2 * dx;
			int dy2 = 2 * dy;
			while (buffer.write(y0, x0, c), x0 != x1 || y0 != y1) {
				int e = bias;
				if (e > -dx2) { bias -= dy2; x0 += sx; }
				if (e < dy2)  { bias += dx2; y0 += sy; }
			}
		}
	};
}