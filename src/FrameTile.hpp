#ifndef _FRAME_TILE_HPP_
#define _FRAME_TILE_HPP_

namespace Guarneri {
	struct TiledTask {
		TiledTask() { shader = nullptr;  }

		TiledTask(const Triangle& triangle, Shader* shader) 
		{
			this->triangle = triangle;
			this->shader = shader;
		}

		Triangle triangle;
		Shader* shader;
	};

	struct FrameTile {
	public:
		FrameTile() {
			tile_idx = 0;
			row_start = 0;
			row_end = 0;
			col_start = 0;
			col_end = 0;
		}

	public:
		uint32_t tile_idx;
		uint32_t row_start;
		uint32_t row_end;
		uint32_t col_start;
		uint32_t col_end;
		SafeQueue<TiledTask> tasks;

	public:
		void push_task(const Triangle& tri, Shader* shader) 
		{
			tasks.push(TiledTask(tri, shader));
		}

		bool pop_task(TiledTask& task) 
		{
			return tasks.try_pop(task);
		}

		auto task_size()
		{
			return tasks.size();
		}

		static int coord2index(
			const int& row, 
			const int& col, 
			const int& col_tile_count) 
		{
			return row * col_tile_count + col;
		}

		static void pixel2tile(
			const int& prow, const int& pcol, 
			int& trow, int& tcol, 
			const int& tile_size) 
		{
			assert(tile_size != 0);
			trow = prow / tile_size;
			tcol = pcol / tile_size;
		}

		static void build_tiles(
			FrameTile* tiles, 
			const int& tile_size, 
			const int& row_tile_count, const int& col_tile_count, 
			const int& row_rest, const int& col_rest) 
		{
			for (int row = 0; row < row_tile_count; row++) {
				for (int col = 0; col < col_tile_count; col++) {
					bool last = false;
					int tidx = coord2index(row, col, col_tile_count);
					int rs = row * tile_size;
					tiles[tidx].row_start = rs;
					last = row == row_tile_count - 1 ? true : false;
					tiles[tidx].row_end = last ? rs + row_rest : (row + 1) * tile_size;

					int cs = col * tile_size;
					tiles[tidx].col_start = cs;
					last = col == col_tile_count - 1 ? true : false;
					tiles[tidx].col_end = last ? cs + col_rest : (col + 1) * tile_size;

					tiles[tidx].tile_idx = tidx;
				}
			}
		}

		static void dispatch_render_task(
			FrameTile* tiles,
			const Triangle& tri, 
			Shader* shader, 
			const int& w, const int& h,
			const int& tile_size,
			const int& col_tile_count) 
		{
			auto bounds = BoundingBox2D(tri[0].position, tri[1].position, tri[2].position);
			int row_start = (int)(bounds.min().y + 0.5f) - 1;
			int row_end = (int)(bounds.max().y + 0.5f) + 1;
			int col_start = (int)(bounds.min().x + 0.5f) - 1;
			int col_end = (int)(bounds.max().x + 0.5f) + 1;

			row_start = CLAMP_INT(row_start, 0, h);
			row_end = CLAMP_INT(row_end, 0, h);
			col_start = CLAMP_INT(col_start, 0, w);
			col_end = CLAMP_INT(col_end, 0, w);

			int tile_row_start, tile_row_end;
			int tile_col_start, tile_col_end;

			pixel2tile(row_start, col_start, tile_row_start, tile_col_start, tile_size);
			pixel2tile(row_end, col_end, tile_row_end, tile_col_end, tile_size);

			for (int row = tile_row_start; row <= tile_row_end; row++) {
				for (int col = tile_col_start; col <= tile_col_end; col++) {
					int tile_idx = coord2index(row, col, col_tile_count);
					tiles[tile_idx].push_task(tri, shader);
				}
			}
		}
	};
}
#endif