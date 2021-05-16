#include "IdAllocator.hpp"
#include <assert.h>
#include "Marcos.h"

namespace Guarneri
{
	IdAllocator::IdAllocator(const uint32_t& _lhs, const uint32_t& _rhs)
	{
		assert(_lhs > INVALID_ID);
		assert(_lhs <= _rhs);
		this->lhs = _lhs;
		this->rhs = _rhs;
		this->cur = _lhs;
	}

	uint32_t IdAllocator::alloc()
	{
		uint32_t id;
		if (alloc(id))
		{
			return id;
		}
		return INVALID_ID;
	}

	bool IdAllocator::alloc(uint32_t& id)
	{
		id = INVALID_ID;
		if (cur < rhs)
		{
			id = cur++;
			return true;
		}
		return false;
	}

	// todo: use blanced binary tree tree to reuse id
	void IdAllocator::free(const uint32_t& id)
	{
		UNUSED(id);
		// do nothing
	}
}