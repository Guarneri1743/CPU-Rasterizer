#pragma once
#include "tinymath.h"
#include "Object.hpp"

namespace Guarneri {
	struct Light {
		tinymath::Color ambient;
		tinymath::Color diffuse;
		tinymath::Color specular;
		float intensity;
		Light();
	};

	struct DirectionalLight : Light {
		float yaw;
		float pitch;
		tinymath::vec3f forward;
		tinymath::vec3f right;
		tinymath::vec3f up;
		tinymath::vec3f position;
		tinymath::mat4x4 p;

		DirectionalLight();
		tinymath::mat4x4 light_space() const;
		tinymath::mat4x4 view_matrix() const;
		tinymath::mat4x4 projection_matrix() const;
		void rotate(const float& yaw_offset, const float& pitch_offset);
		void update_rotation();
	};

	struct PointLight : Light {
		float constant;
		float linear;
		float quadratic;
		tinymath::vec3f position;
		PointLight();
	};

	struct SpotLight : Light {
		tinymath::vec3f position;
		tinymath::vec3f spot_direction;
		float phi;
		float theta;
	};
}