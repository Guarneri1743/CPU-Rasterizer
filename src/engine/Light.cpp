#include "Light.hpp"
#include "Serialization.hpp"

namespace Guarneri
{
	Light::Light()
	{
		ambient = tinymath::kColorBlack;
		diffuse = tinymath::kColorWhite;
		specular = tinymath::kColorWhite;
		intensity = 1.0f;
	}

	DirectionalLight::DirectionalLight() : Light()
	{
		this->yaw = 0.0f;
		this->pitch = -45.0f;
		//this->p = tinymath::mat4x4::perspective(45.0f, 800.0f/600.0f, 0.5f, 500.0f);
		this->p = tinymath::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.01f, 200.0f);
		this->position = tinymath::vec3f(10.0f, 10.0f, 10.0f);
		update_rotation();
	}

	tinymath::mat4x4 DirectionalLight::light_space() const
	{
		auto v = tinymath::lookat(position, position + forward, tinymath::kVec3fUp);
		auto ret = p * v;
		return ret;
	}

	tinymath::mat4x4 DirectionalLight::view_matrix() const
	{
		return tinymath::lookat(position, position + forward, tinymath::kVec3fUp);
	}

	tinymath::mat4x4 DirectionalLight::projection_matrix() const
	{
		return p;
	}

	void DirectionalLight::rotate(const float& yaw_offset, const float& pitch_offset)
	{
		this->yaw -= yaw_offset;
		this->pitch -= pitch_offset;
		this->pitch = std::clamp(this->pitch, -89.0f, 89.0f);
		update_rotation();
	}

	void DirectionalLight::update_rotation()
	{
		forward.x = cos(DEGREE2RAD(this->yaw)) * cos(DEGREE2RAD(this->pitch));
		forward.y = sin(DEGREE2RAD(this->pitch));
		forward.z = sin(DEGREE2RAD(this->yaw)) * cos(DEGREE2RAD(this->pitch));
		forward = tinymath::normalize(forward);
		tinymath::calculate_right_up(forward, right, up);
	}

	PointLight::PointLight() : Light()
	{
		constant = 1.0f;
		linear = 0.1f;
		quadratic = 0.03f;
		position = tinymath::kVec3fZero;
	}
}