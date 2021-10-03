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

	size_t FrameTile::coord2index(
		const size_t& row,
		const size_t& col,
		const size_t& col_tile_count)
	{
		return row * col_tile_count + col;
	}

	void FrameTile::pixel2tile(
		const size_t& prow, const size_t& pcol,
		size_t& trow, size_t& tcol,
		const size_t& tile_size)
	{
		assert(tile_size != 0);
		trow = prow / tile_size;
		tcol = pcol / tile_size;
	}

	void FrameTile::build_tiles(
		FrameTile* tiles,
		const size_t& tile_size,
		const size_t& row_tile_count, const size_t& col_tile_count,
		const size_t& row_rest, const size_t& col_rest)
	{
		for (size_t row = 0; row < row_tile_count; row++)
		{
			for (size_t col = 0; col < col_tile_count; col++)
			{
				bool last = false;
				size_t tidx = coord2index(row, col, col_tile_count);
				size_t rs = row * tile_size;
				tiles[tidx].row_start = rs;
				last = row == row_tile_count - 1ull ? true : false;
				tiles[tidx].row_end = last && row_rest > 0ull ? rs + row_rest : (row + 1ull) * tile_size;

				size_t cs = col * tile_size;
				tiles[tidx].col_start = cs;
				last = col == col_tile_count - 1ull ? true : false;
				tiles[tidx].col_end = last && col_rest > 0ull ? cs + col_rest : (col + 1ull) * tile_size;

				tiles[tidx].tile_idx = tidx;
			}
		}
	}

	void FrameTile::push_draw_task(
		FrameTile* tiles,
		const size_t& row_tile_count,
		const size_t& col_tile_count,
		const Triangle& tri,
		const Shader& shader,
		const size_t& w, const size_t& h,
		const size_t& tile_size)
	{
		auto bounds = tinymath::Rect(tri[0].position.xy, tri[1].position.xy, tri[2].position.xy);
		int row_start = (int)(bounds.min().y) - 1;
		int row_end = (int)(bounds.max().y) + 1;
		int col_start = (int)(bounds.min().x) - 1;
		int col_end = (int)(bounds.max().x) + 1;

		row_start = std::clamp(row_start, 0, (int)h);
		row_end = std::clamp(row_end, 0, (int)h);
		col_start = std::clamp(col_start, 0, (int)w);
		col_end = std::clamp(col_end, 0, (int)w);

		size_t tile_row_start, tile_row_end;
		size_t tile_col_start, tile_col_end;

		pixel2tile(row_start, col_start, tile_row_start, tile_col_start, tile_size);
		pixel2tile(row_end, col_end, tile_row_end, tile_col_end, tile_size);

		tile_row_start = std::max(0ull, tile_row_start);
		tile_col_start = std::max(0ull, tile_col_start);
		tile_row_end = std::min(tile_row_end, row_tile_count - 1);
		tile_col_end = std::min(tile_col_end, col_tile_count - 1);

		for (size_t row = tile_row_start; row <= tile_row_end; row++)
		{
			for (size_t col = tile_col_start; col <= tile_col_end; col++)
			{
				size_t tile_idx = coord2index(row, col, col_tile_count);
				tiles[tile_idx].push_task(tri, shader);
			}
		}
	}
}