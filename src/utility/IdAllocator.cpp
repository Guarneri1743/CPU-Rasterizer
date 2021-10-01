#include "IdAllocator.hpp"
#include <assert.h>
#include "Marcos.h"

namespace Guarneri
{
	IdAllocator::IdAllocator(const uint32_t& _lhs, const uint32_t& _rhs)
	{
		assert(_lhs > kInvalidID);
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
		return kInvalidID;
	}

	bool IdAllocator::alloc(uint32_t& id)
	{
		id = kInvalidID;
		if (cur < rhs)
		{
			id = cur++;
			return true;
		}
		return false;
	}

	void IdAllocator::free(const uint32_t& id)
	{
		UNUSED(id);
	}
}