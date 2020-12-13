#ifndef _ID_ALLOC_
#define _ID_ALLOC_

namespace Guarneri
{
#define INVALID_ID 0

	class IdAllocator
	{
	private:
		uint32_t cur;
		uint32_t lhs;
		uint32_t rhs;

	public:
		IdAllocator(const uint32_t& _lhs, const uint32_t& _rhs);
		uint32_t alloc();
		bool alloc(uint32_t& id);
		void free(const uint32_t& id);
	};


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
		REF(id)
		// do nothing
	}
}
#endif