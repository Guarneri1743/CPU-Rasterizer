#include "Object.hpp"
#include <string>
#include <stdint.h>
#include "Define.hpp"
#include "IdAllocator.hpp"

namespace CpuRasterizer
{
	IdAllocator<resource_id, kInvalidID, kMaxID> idalloc;

	Object::Object()
	{
		id = idalloc.alloc();
	}

	Object::~Object()
	{
		idalloc.free(id);
	}

	resource_id Object::get_id()
	{
		return id;
	}
}