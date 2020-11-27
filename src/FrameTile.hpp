#ifndef _FRAME_TILE_HPP_
#define _FRAME_TILE_HPP_

namespace Guarneri {
	struct TiledTask {
		TiledTask() { shader = nullptr;  }
		TiledTask(const Triangle& triangle, Shader* shader) {
			this->triangle = triangle;
			this->shader = shader;
		}

		Triangle triangle;
		Shader* shader;
	};

	struct FrameTile {
		int start_x;
		int start_y;
		int end_x;
		int end_y;
		SafeQueue<TiledTask> tasks;

	public:
		void push_task(const Triangle& tri, Shader* shader) {
			tasks.push(TiledTask(tri, shader));
		}

		bool pop_task(TiledTask& task) {
			return tasks.try_pop(task);
		}

		int task_size() {
			return tasks.size();
		}

		static int coord2index(const int& row, const int& col, const int& col_tile_count) {
			return row * col_tile_count + col;
		}

		static void pixel2tile(const int& prow, const int& pcol, int& trow, int& tcol, const int& tile_size) {
			assert(tile_size != 0);
			trow = prow / tile_size;
			tcol = pcol / tile_size;
		}

		static std::vector<int> dispatch_render_task(FrameTile* tiles, const Triangle& tri, Shader* shader, const int& w, const int& h, const int& tile_size, const int& col_tile_count) {
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

			for (int row = tile_row_start; row < tile_row_end; row++) {
				for (int col = tile_col_start; col < tile_col_end; col++) {
					int tile_idx = coord2index(row, col, col_tile_count);
					tiles[tile_idx].push_task(tri, shader);
				}
			}
		}
	};
}
#endif