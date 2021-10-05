#include "Object.hpp"
#include <string>
#include <stdint.h>
#include "Define.hpp"
#include "IdAllocator.hpp"

namespace CpuRasterizor
{
	IdAllocator idalloc(kInvalidID + 1, kMaxID);

	Object::Object()
	{
		id = idalloc.alloc();
	}

	Object::~Object()
	{
		idalloc.free(id);
	}

	uint32_t Object::get_id()
	{
		return id;
	}
}