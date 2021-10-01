#pragma once
#include <string>
#include "tinymath.h"

namespace Guarneri
{
	struct Ray
	{
	public:
		tinymath::vec3f origin;
		tinymath::vec3f direction;
		tinymath::vec3f inversed_direction;

	public:
		Ray();
		Ray(const tinymath::vec3f& origin, const tinymath::vec3f& direction);
		Ray(const Ray& other);
	};
}