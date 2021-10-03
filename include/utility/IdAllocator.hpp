#pragma once
#include <stdint.h>
#include <limits>

constexpr uint32_t kInvalidID = 0;
constexpr uint32_t kMaxID = UINT_MAX;

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
