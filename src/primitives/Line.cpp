#include "Line.hpp"

namespace Guarneri
{
	Line::Line()
	{
		origin = tinymath::vec3f();
		direction = tinymath::vec3f();
	}

	Line::Line(const tinymath::vec3f& origin_t, const tinymath::vec3f& direction_t)
	{
		this->origin = origin_t;
		this->direction = direction_t;
	}

	Line::Line(const Line& other)
	{
		this->origin = other.origin;
		this->direction = other.direction;
	}
}