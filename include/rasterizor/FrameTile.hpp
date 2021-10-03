#pragma once
#include <vector>
#include "Triangle.hpp"
#include "Shader.hpp"
#include "SafeQueue.hpp"

namespace Guarneri
{
	typedef struct
	{
		size_t tile_size;
		size_t row_tile_count;
		size_t col_tile_count;
		size_t tile_length;
	} TileInfo;

	struct TileTask
	{
	public:
		Triangle triangle;
		const Shader* shader;

	public:
		TileTask();
		TileTask(const Triangle& triangle, const Shader& shader);
	};

	struct FrameTile
	{
	public:
		size_t tile_idx;
		size_t row_start;
		size_t row_end;
		size_t col_start;
		size_t col_end;
		SafeQueue<TileTask> tasks;

	public:
		FrameTile();
		~FrameTile();
		void push_task(const Triangle& tri, const Shader& shader);
		bool pop_task(TileTask& task);
		bool is_task_empty();
		size_t task_size();
		static size_t coord2index(const size_t& row, const size_t& col, const size_t& col_tile_count);
		static void pixel2tile(const size_t& prow, const size_t& pcol, size_t& trow, size_t& tcol, const size_t& tile_size);
		static void build_tiles(
			FrameTile* tiles,
			const size_t& tile_size,
			const size_t& row_tile_count, const size_t& col_tile_count,
			const size_t& row_rest, const size_t& col_rest);
		static void push_draw_task(
			FrameTile* tiles,
			const size_t& row_tile_count,
			const size_t& col_tile_count,
			const Triangle& tri,
			const Shader& shader,
			const size_t& w, const size_t& h,
			const size_t& tile_size);
	};
}