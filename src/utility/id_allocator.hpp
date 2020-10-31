#pragma once
#include <common.hpp>
#include <set>

namespace guarneri {
	struct id_spin {
	public:
		id_spin(int left, int right) :left_id(left), right_id(right) {}
		int left()const { return left_id; }
		int right()const { return right_id; }

		bool operator < (const id_spin& rhs)const {
			return left_id < rhs.left() && right_id < rhs.left();
		}
	private:
		int left_id;
		int right_id;
	};

	class id_allocator {
	public:
		id_allocator(int min_id, int max_id)
		{
			assert(min_id >= 0);
			assert(min_id <= max_id);
			this->min_id = min_id;
			this->max_id = max_id;
			free_ids.insert(id_spin(min_id, max_id));
		}

	private:
		int min_id;
		int max_id;
		std::set<id_spin> free_ids;

	public:
		bool alloc(int& iid)
		{
			auto first = free_ids.begin();
			if (first == free_ids.end()) {
				return false;
			}
			auto ileft = first->left();
			auto iright = first->right();
			auto next = ileft + 1;
			free_ids.erase(first);
			if (next <= iright) {
				free_ids.insert(id_spin(next, iright));
			}
			iid = ileft;
			return true;
		}

		void free(int iid)
		{
			if (iid > max_id || iid < min_id) {
				return;
			}
			auto current = id_spin(iid, iid);
			int left, right;
			auto iterator = free_ids.upper_bound(current);
			if (iterator == free_ids.end()) {
				auto last = free_ids.rbegin();
				if (last != free_ids.rend() && last->right() + 1 == iid) {
					left = last->left();
					free_ids.erase(std::next(last).base());
					free_ids.insert(id_spin(left, iid));
				}
				else {
					free_ids.insert(id_spin(iid, iid));
				}
			}
			else {
				auto next = iid + 1;
				if (next == iterator->left()) {
					if (iterator == free_ids.begin()) {
						right = iterator->right();
						free_ids.erase(iterator);
						free_ids.insert(id_spin(iid, right));
					}
					else {
						auto prev = iterator;
						--prev;
						if (prev->right() + 1 == iid) {
							left = prev->left();
							right = iterator->right();
							free_ids.erase(iterator);
							free_ids.erase(prev);
							free_ids.insert(id_spin(left, right));
						}
						else {
							right = iterator->right();
							free_ids.erase(iterator);
							free_ids.insert(id_spin(iid, right));
						}
					}
				}
				else {
					if (iterator != free_ids.begin()) {
						auto prev = iterator;
						--prev;
						if (prev->right() + 1 == iid) {
							left = prev->left();
							free_ids.erase(prev);
							free_ids.insert(id_spin(left, iid));
							return;
						}
					}
					free_ids.insert(current);
				}
			}
		}
	};
}
