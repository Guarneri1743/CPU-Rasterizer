#include "Object.hpp"
#include <sstream>
#include <ostream>
#include <string>
#include <stdint.h>
#include "Marcos.h"
#include "IdAllocator.hpp"

namespace Guarneri
{
	IdAllocator idalloc(INVALID_ID + 1, MAX_ID);
	#define ALLOC_ID() idalloc.alloc();
	#define FREE_ID(id) idalloc.free(id);

	Object::Object()
	{
		id = ALLOC_ID();
	}
	Object::~Object()
	{
		FREE_ID(id);
	}

	uint32_t Object::get_id()
	{
		return id;
	}

	static std::ostream& operator << (std::ostream& stream, const Object& obj)
	{
		stream << obj.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Object& obj)
	{
		stream << obj.str();
		return stream;
	}
}