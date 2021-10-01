#include "LogHelper.h"
#include "tinymath.h"

std::ostream& operator<<(std::ostream& stream, const tinymath::vec2f& v)
{
	return stream;
}

std::stringstream& operator<<(std::stringstream& stream, const tinymath::vec2f& v)
{
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const tinymath::vec3f& v)
{
	return stream;
}

std::stringstream& operator<<(std::stringstream& stream, const tinymath::vec3f& v)
{
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const tinymath::vec4f& v)
{
	return stream;
}

std::stringstream& operator<<(std::stringstream& stream, const tinymath::vec4f& v)
{
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const tinymath::mat3x3& v)
{
	return stream;
}

std::stringstream& operator<<(std::stringstream& stream, const tinymath::mat3x3& v)
{
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const tinymath::mat4x4& v)
{
	return stream;
}

std::stringstream& operator<<(std::stringstream& stream, const tinymath::mat4x4& v)
{
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Camera& cam)
{
	//std::stringstream ss;
	//ss << "Camera[" << "  pos: " << cam.transform->world_position() << "]";
	//return ss.str();
	return stream;
}

std::stringstream& operator<<(std::stringstream& stream, const Camera& cam)
{
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Vertex& cam)
{
	//std::stringstream ss;
	//ss << "Vertex: [pos: " << this->position.str() << ", Color: " << this->color.str() << ", uv: " << uv.str() << "]";
	//return ss.str();
	return stream;
}

std::stringstream& operator<<(std::stringstream& stream, const Vertex& cam)
{
	return stream;
}