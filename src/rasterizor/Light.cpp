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

		l = -20.0f;
		r = 20.0f;
		b = -20.0f;
		t = 20.0f;
		n = 0.01f;
		f = 200.0f;

		update_projection();
		update_rotation();
	}

	tinymath::mat4x4 DirectionalLight::light_space() const
	{
		auto view = view_matrix();
		auto ret = projection * view;
		return ret;
	}

	tinymath::mat4x4 DirectionalLight::view_matrix() const
	{
		return tinymath::lookat(-forward * distance, tinymath::kVec3fZero, tinymath::kVec3fUp);
	}

	tinymath::mat4x4 DirectionalLight::projection_matrix() const
	{
		return projection;
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

	void DirectionalLight::update_projection()
	{
		this->projection = tinymath::ortho(l, r, b, t, n, f);
	}

	PointLight::PointLight() : Light()
	{
		constant = 1.0f;
		linear = 0.1f;
		quadratic = 0.03f;
		position = tinymath::kVec3fZero;
	}
}