#ifndef _SEGMENT_DRAWER_
#define _SEGMENT_DRAWER_
#include <RawBuffer.hpp>

namespace Guarneri
{
	class SegmentDrawer
	{
	public:
		template<typename T>
		static void dda(RawBuffer<T>* buffer, const int& x0, const int& y0, const int& x1, const int& y1, const T& c);
		template<typename T>
		static void bresenham(RawBuffer<T>* buffer, const int& x0, const int& y0, const int& x1, const int& y1, const T& c);
	};


	template<typename T>
	void SegmentDrawer::dda(RawBuffer<T>* buffer, const int& x0, const int& y0, const int& x1, const int& y1, const T& c)
	{
		assert(buffer != nullptr);
		int dx = std::abs(x1 - x0);
		int dy = std::abs(y1 - y0);
		float sx, sy, length;
		if (dx >= dy)
		{
			length = dx;
		}
		else
		{
			length = dy;
		}
		sx = (float)(x1 - x0) / length;
		sy = (float)(y1 - y0) / length;
		for (float i = 1, x = x0, y = y0; i <= length; i++)
		{
			buffer->write((int)(y + 0.5f), (int)(x + 0.5f), c);
			x += sx;
			y += sy;
		}
	}

	template<typename T>
	void SegmentDrawer::bresenham(RawBuffer<T>* buffer, const int& x0, const int& y0, const int& x1, const int& y1, const T& c)
	{
		assert(buffer != nullptr);
		int dx = std::abs(x1 - x0);
		int dy = std::abs(y1 - y0);
		int sx = x0 < x1 ? 1 : -1;
		int sy = y0 < y1 ? 1 : -1;
		int bias = dx > dy ? dx : -dy;
		int dx2 = 2 * dx;
		int dy2 = 2 * dy;
		int xi = x0;
		int yi = y0;
		while (buffer->write((uint32_t)yi, (uint32_t)xi, c), xi != x1 || yi != y1)
		{
			int e = bias;
			if (e > -dx2)
			{
				bias -= dy2; xi += sx;
			}
			if (e < dy2)
			{
				bias += dx2; yi += sy;
			}
		}
	}
}
#endif