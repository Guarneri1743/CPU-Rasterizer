#ifndef _OBJECT_
#define _OBJECT_
#include <string>
#include <stdint.h>

namespace Guarneri
{
	class Object
	{
	public:
		Object();
		virtual ~Object();

	protected:
		uint32_t id;

	public:
		uint32_t get_id();
		virtual std::string str() const = 0;
	};
}
#endif