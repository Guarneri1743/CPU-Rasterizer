#ifndef _ID_ALLOC_
#define _ID_ALLOC_
#include <Guarneri.hpp>

namespace Guarneri {
	#define INVALID_ID 0
	struct IdSpin {
	public:
		IdSpin(uint32_t left, uint32_t right) :left_id(left), right_id(right) {}
		uint32_t left()const { return left_id; }
		uint32_t right()const { return right_id; }

		bool operator < (const IdSpin& rhs)const {
			return left_id < rhs.left() && right_id < rhs.left();
		}
	private:
		uint32_t left_id;
		uint32_t right_id;
	};

	class IdAllocator {
	public:
		IdAllocator(uint32_t min_id, uint32_t max_id)
		{
			assert(min_id > INVALID_ID);
			assert(min_id <= max_id);
			this->min_id = min_id;
			this->max_id = max_id;
			free_ids.insert(IdSpin(min_id, max_id));
		}

	private:
		uint32_t min_id;
		uint32_t max_id;
		std::set<IdSpin> free_ids;

	public:
		uint32_t alloc() {
			uint32_t id;
			if (alloc(id)) {
				return id;
			}
			return INVALID_ID;
		}

		bool alloc(uint32_t& iid)
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
				free_ids.insert(IdSpin(next, iright));
			}
			iid = ileft;
			return true;
		}

		void free(uint32_t iid)
		{
			if (iid > max_id || iid < min_id) {
				return;
			}
			auto current = IdSpin(iid, iid);
			uint32_t left, right;
			auto iterator = free_ids.upper_bound(current);
			if (iterator == free_ids.end()) {
				auto last = free_ids.rbegin();
				if (last != free_ids.rend() && last->right() + 1 == iid) {
					left = last->left();
					free_ids.erase(std::next(last).base());
					free_ids.insert(IdSpin(left, iid));
				}
				else {
					free_ids.insert(IdSpin(iid, iid));
				}
			}
			else {
				auto next = iid + 1;
				if (next == iterator->left()) {
					if (iterator == free_ids.begin()) {
						right = iterator->right();
						free_ids.erase(iterator);
						free_ids.insert(IdSpin(iid, right));
					}
					else {
						auto prev = iterator;
						--prev;
						if (prev->right() + 1 == iid) {
							left = prev->left();
							right = iterator->right();
							free_ids.erase(iterator);
							free_ids.erase(prev);
							free_ids.insert(IdSpin(left, right));
						}
						else {
							right = iterator->right();
							free_ids.erase(iterator);
							free_ids.insert(IdSpin(iid, right));
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
							free_ids.insert(IdSpin(left, iid));
							return;
						}
					}
					free_ids.insert(current);
				}
			}
		}
	};
}
#endif