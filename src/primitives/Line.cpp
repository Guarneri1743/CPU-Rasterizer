#include "Line.hpp"

namespace Guarneri
{
	Line::Line()
	{
		origin = Guarneri::Vector3();
		direction = Guarneri::Vector3();
	}

	Line::Line(const Guarneri::Vector3& origin_t, const Guarneri::Vector3& direction_t)
	{
		this->origin = origin_t;
		this->direction = direction_t;
	}

	Line::Line(const Line& other)
	{
		this->origin = other.origin;
		this->direction = other.direction;
	}

	std::string Line::str() const
	{
		std::stringstream ss;
		ss << "Line: [origin: " << this->origin << ", direction: " << this->direction << "]";
		return ss.str();
	}
}