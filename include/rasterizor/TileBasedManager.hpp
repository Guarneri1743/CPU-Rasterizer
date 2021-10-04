#pragma once
#include <memory>
#include <functional>
#include "Define.hpp"
#include "tinymath/tinymath.h"
#include "RawBuffer.hpp"
#include "SafeQueue.hpp"
#include "Triangle.hpp"
#include "Shader.hpp"

namespace Guarneri
{
	constexpr size_t kTileSize = 16;

	struct TileTask
	{
		Triangle triangle;
		const Shader* shader;
	};

	struct Tile
	{
		size_t index;
		tinymath::Rect rect;
	};

	class TileBasedManager
	{
	public:
		TileBasedManager(const size_t& w, const size_t& h);
		~TileBasedManager();

		void resize(const size_t& w, const size_t& h);
		void foreach_tile(std::function<void(tinymath::Rect& tile, SafeQueue<TileTask>& task_queue)> func);
		void push_draw_task(const Triangle& tri, const Shader& shader);

	private:
		void rebuild_tiles(
			const size_t& row_tile_count,
			const size_t& col_tile_count,
			const size_t& row_rest,
			const size_t& col_rest);

		static size_t coord2index(const size_t& row, const size_t& col, const size_t& col_tile_count);
		static void pixel2tile(const size_t& prow, const size_t& pcol, size_t& trow, size_t& tcol, const size_t& tile_size);

	private:
		// tile based
		size_t row_tile_resolution;
		size_t col_tile_resolution;
		size_t last_row_tile_size;
		size_t last_col_tile_size;
		std::vector<Tile> tiles;
		SafeQueue<TileTask>* tile_tasks;
		size_t width;
		size_t height;
	};
}