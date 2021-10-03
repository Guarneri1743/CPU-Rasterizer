#include "FrameTile.hpp"
#include <assert.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include "Marcos.h"
#include "tinymath/primitives/Rect.h"

namespace Guarneri
{
	TileTask::TileTask()
	{
		shader = nullptr;
	}

	TileTask::TileTask(const Triangle& triangle, const Shader& shader)
	{
		this->triangle = triangle;
		this->shader = &shader;
	}

	FrameTile::FrameTile()
	{
		tile_idx = 0;
		row_start = 0;
		row_end = 0;
		col_start = 0;
		col_end = 0;
	}

	FrameTile::~FrameTile(){}

	void FrameTile::push_task(const Triangle& tri, const Shader& shader)
	{
		tasks.produce(TileTask(tri, shader));
	}

	bool FrameTile::pop_task(TileTask& task)
	{
		return tasks.try_consume(task);
	}

	bool FrameTile::is_task_empty()
	{
		return tasks.empty();
	}

	size_t FrameTile::task_size()
	{
		return tasks.size();
	}

	int FrameTile::coord2index(
		const int& row,
		const int& col,
		const int& col_tile_count)
	{
		return row * col_tile_count + col;
	}

	void FrameTile::pixel2tile(
		const int& prow, const int& pcol,
		int& trow, int& tcol,
		const int& tile_size)
	{
		assert(tile_size != 0);
		trow = prow / tile_size;
		tcol = pcol / tile_size;
	}

	void FrameTile::build_tiles(
		FrameTile* tiles,
		const int& tile_size,
		const int& row_tile_count, const int& col_tile_count,
		const int& row_rest, const int& col_rest)
	{
		for (int row = 0; row < row_tile_count; row++)
		{
			for (int col = 0; col < col_tile_count; col++)
			{
				bool last = false;
				int tidx = coord2index(row, col, col_tile_count);
				int rs = row * tile_size;
				tiles[tidx].row_start = rs;
				last = row == row_tile_count - 1 ? true : false;
				tiles[tidx].row_end = last && row_rest > 0 ? rs + row_rest : (row + 1) * tile_size;

				int cs = col * tile_size;
				tiles[tidx].col_start = cs;
				last = col == col_tile_count - 1 ? true : false;
				tiles[tidx].col_end = last && col_rest > 0 ? cs + col_rest : (col + 1) * tile_size;

				tiles[tidx].tile_idx = tidx;
			}
		}
	}

	void FrameTile::push_draw_task(
		FrameTile* tiles,
		const uint32_t& row_tile_count,
		const uint32_t& col_tile_count,
		const Triangle& tri,
		const Shader& shader,
		const int& w, const int& h,
		const int& tile_size)
	{
		auto bounds = tinymath::Rect(tri[0].position.xy, tri[1].position.xy, tri[2].position.xy);
		int row_start = (int)(bounds.min().y) - 1;
		int row_end = (int)(bounds.max().y) + 1;
		int col_start = (int)(bounds.min().x) - 1;
		int col_end = (int)(bounds.max().x) + 1;

		row_start = std::clamp(row_start, 0, h);
		row_end = std::clamp(row_end, 0, h);
		col_start = std::clamp(col_start, 0, w);
		col_end = std::clamp(col_end, 0, w);

		int tile_row_start, tile_row_end;
		int tile_col_start, tile_col_end;

		pixel2tile(row_start, col_start, tile_row_start, tile_col_start, tile_size);
		pixel2tile(row_end, col_end, tile_row_end, tile_col_end, tile_size);

		tile_row_start = tile_row_start >= 0 ? tile_row_start : 0;
		tile_col_start = tile_col_start >= 0 ? tile_col_start : 0;
		tile_row_end = tile_row_end < (int)row_tile_count ? tile_row_end : (int)row_tile_count - 1;
		tile_col_end = tile_col_end < (int)col_tile_count ? tile_col_end : (int)col_tile_count - 1;

		for (int row = tile_row_start; row <= tile_row_end; row++)
		{
			for (int col = tile_col_start; col <= tile_col_end; col++)
			{
				int tile_idx = coord2index(row, col, col_tile_count);
				tiles[tile_idx].push_task(tri, shader);
			}
		}
	}
}