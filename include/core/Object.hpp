#pragma once
#include "Define.hpp"
#include <string>
#include <stdint.h>

namespace CpuRasterizer
{
	class Object
	{
	public:
		Object();
		virtual ~Object();

	protected:
		resource_id id;

	public:
		resource_id get_id();
	};
}