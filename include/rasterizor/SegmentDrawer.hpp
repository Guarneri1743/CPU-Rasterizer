#pragma once
#include "RawBuffer.hpp"

namespace CpuRasterizor
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
			buffer->write((size_t)(y + 0.5f), (size_t)(x + 0.5f), c);
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
		int col = x0;
		int row = y0;
		int w = (int)buffer->width;
		int h = (int)buffer->height;
		while (((row >= 0 && row < h &&  col >= 0 && col < w) && buffer->write((size_t)row, (size_t)col, c)), (col != x1 || row != y1))
		{
			int e = bias;
			if (e > -dx2)
			{
				bias -= dy2; col += sx;
			}
			if (e < dy2)
			{
				bias += dx2; row += sy;
			}
		}
	}
}