#include "TileBasedManager.hpp"
#include <execution>
#include <algorithm>

namespace CpuRasterizer
{
	TileBasedManager::TileBasedManager(size_t w, size_t h) :
		width(w), height(h), tile_tasks(nullptr),
		row_tile_resolution(0), col_tile_resolution(0), last_row_tile_size(0), last_col_tile_size(0)
	{
		resize(w, h);
	}

	TileBasedManager::~TileBasedManager()
	{
		if (tile_tasks != nullptr)
		{
			delete[] tile_tasks;
		}
	}

	void TileBasedManager::resize(size_t w, size_t h)
	{
		bool size_changed = width != w || height != h;
		if (size_changed || tiles.size() == 0)
		{
			width = w;
			height = h;
			last_row_tile_size = h % kTileSize;
			last_col_tile_size = w % kTileSize;
			row_tile_resolution = h / kTileSize + (last_row_tile_size > 0 ? 1 : 0);
			col_tile_resolution = w / kTileSize + (last_col_tile_size > 0 ? 1 : 0);
			rebuild_tiles(row_tile_resolution, col_tile_resolution, last_row_tile_size, last_col_tile_size);
		}
	}

	void TileBasedManager::foreach_tile(std::function<void(tinymath::Rect& tile, SafeQueue<TileTask>& task_queue)> func)
	{
		std::for_each(
			std::execution::par_unseq,
			tiles.begin(),
			tiles.end(),
			[this, &func](auto&& tile)
		{
			func(tile.rect, tile_tasks[tile.index]);
		});
	}

	size_t TileBasedManager::coord2index(
		size_t row,
		size_t col,
		size_t col_tile_count)
	{
		return row * col_tile_count + col;
	}

	void TileBasedManager::pixel2tile(
		size_t prow, size_t pcol,
		size_t& trow, size_t& tcol,
		size_t tile_size)
	{
		assert(tile_size != 0);
		trow = prow / tile_size;
		tcol = pcol / tile_size;
	}

	void TileBasedManager::rebuild_tiles(
		size_t row_res,
		size_t col_res,
		size_t last_row_size,
		size_t last_col_size)
	{
		size_t length = row_res * col_res;
		tiles.resize(length);
		tiles.shrink_to_fit();
		
		if (tile_tasks != nullptr)
		{
			delete[] tile_tasks;
		}

		tile_tasks = new SafeQueue<TileTask>[length];

		for (size_t row = 0; row < row_res; row++)
		{
			for (size_t col = 0; col < col_res; col++)
			{
				size_t tidx = coord2index(row, col, col_res);
				size_t rs = row * kTileSize;
				size_t cs = col * kTileSize;
				bool last_row = row == row_res - 1ull ? true : false;
				bool last_col = col == col_res - 1ull ? true : false;
				size_t row_start = rs;
				size_t row_size = last_row && last_row_size > 0ull ? last_row_size : kTileSize;
				size_t col_start = cs;
				size_t col_size = last_col && last_col_size > 0ull ? last_col_size : kTileSize;
				auto rect = tinymath::Rect((int)col_start, (int)row_start, (int)col_size, (int)row_size);
				tiles[tidx] = { tidx, rect };
			}
		}
	}

	void TileBasedManager::push_draw_task(const Triangle& tri, const Shader& shader)
	{
		auto bounds = tri.get_bounds();
		int padding = kBoundsPadding;
		int row_start = bounds.min().y - padding;
		int row_end = bounds.max().y + padding;
		int col_start = bounds.min().x - padding;
		int col_end = bounds.max().x + padding;

		int w = (int)width;
		int h = (int)height;

		row_start = tinymath::clamp(row_start, 0, (int)h);
		row_end = tinymath::clamp(row_end, 0, (int)h);
		col_start = tinymath::clamp(col_start, 0, (int)w);
		col_end = tinymath::clamp(col_end, 0, (int)w);

		size_t tile_row_start, tile_row_end;
		size_t tile_col_start, tile_col_end;

		pixel2tile(row_start, col_start, tile_row_start, tile_col_start, kTileSize);
		pixel2tile(row_end, col_end, tile_row_end, tile_col_end, kTileSize);

		tile_row_start = tinymath::max(0ull, tile_row_start);
		tile_col_start = tinymath::max(0ull, tile_col_start);
		tile_row_end = tinymath::min(tile_row_end, row_tile_resolution - 1);
		tile_col_end = tinymath::min(tile_col_end, col_tile_resolution - 1);

		for (size_t row = tile_row_start; row <= tile_row_end; row++)
		{
			for (size_t col = tile_col_start; col <= tile_col_end; col++)
			{
				size_t tile_idx = coord2index(row, col, col_tile_resolution);
				tile_tasks[tile_idx].produce({ tri, &shader });
			}
		}
	}
}