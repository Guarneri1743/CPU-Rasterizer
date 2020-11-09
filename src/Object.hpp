#ifndef _OBJECT_
#define _OBJECT_
#include <IdAllocator.hpp>
#include <Utility.hpp>

namespace Guarneri {
	class Object {
	public:
		Object() { id = ALLOC_ID(); }
		virtual ~Object() { FREE_ID(id); }

	protected:
		uint32_t id;

	public:
		uint32_t get_id() {
			return id;
		}

		virtual std::string str() const = 0;
	};

	static std::ostream& operator << (std::ostream& stream, const Object& obj) {
		stream << obj.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const Object& obj) {
		stream << obj.str();
		return stream;
	}
}
#endif