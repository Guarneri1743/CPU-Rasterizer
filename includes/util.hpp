#pragma once
#include <raw_buffer.hpp>

namespace guarneri {
	class util {
	public:
		template<typename T>
		static void draw_line(raw_buffer<T>& buffer, int x1, int y1, int x2, int y2, T data) {
			int x, y, rem = 0;
			if (x1 == x2 && y1 == y2) {
				buffer.write(x1, y1, data);
			}
			else if (x1 == x2) {
				int inc = (y1 <= y2) ? 1 : -1;
				for (y = y1; y != y2; y += inc) buffer.write(x1, y, data);
				buffer.write(x2, y2, data);
			}
			else if (y1 == y2) {
				int inc = (x1 <= x2) ? 1 : -1;
				for (x = x1; x != x2; x += inc) buffer.write(x, y1, data);
				buffer.write(x2, y2, data);
			}
			else {
				int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
				int dy = (y1 < y2) ? y2 - y1 : y1 - y2;
				if (dx >= dy) {
					if (x2 < x1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
					for (x = x1, y = y1; x <= x2; x++) {
						buffer.write(x, y, data);
						rem += dy;
						if (rem >= dx) {
							rem -= dx;
							y += (y2 >= y1) ? 1 : -1;
							buffer.write(x, y, data);
						}
					}
					buffer.write(x2, y2, data);
				}
				else {
					if (y2 < y1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
					for (x = x1, y = y1; y <= y2; y++) {
						buffer.write(x, y, data);
						rem += dx;
						if (rem >= dy) {
							rem -= dy;
							x += (x2 >= x1) ? 1 : -1;
							buffer.write(x, y, data);
						}
					}
					buffer.write(x2, y2, data);
				}
			}
		}
	};
}