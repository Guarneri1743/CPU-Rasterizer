#pragma once
#include <stdint.h>
#include <limits>

constexpr size_t kInvalidID = 0;
constexpr size_t kMaxID = UINT64_MAX;

// todo

template<typename T, T invalid_id, T rhs>
class IdAllocator
{
private:
	T cur;

public:
	T alloc()
	{
		T id;
		if (alloc(id))
		{
			return id;
		}
		return invalid_id;
	}

	bool alloc(T& id)
	{
		id = invalid_id;
		if (cur < rhs)
		{
			id = cur++;
			return true;
		}

		return false;
	}

	void free(T id)
	{
		UNUSED(id);
	}
};
