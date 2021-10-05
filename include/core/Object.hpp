#pragma once
#include <string>
#include <stdint.h>

namespace CpuRasterizor
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
	};
}