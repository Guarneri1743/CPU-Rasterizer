#pragma once
#include "tinymath.h"
#include "Object.hpp"

namespace CpuRasterizor {
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
		float distance;
		tinymath::FrustumParam frustum_param;
		tinymath::mat4x4 projection;

		DirectionalLight();
		tinymath::mat4x4 light_space() const;
		tinymath::mat4x4 view_matrix() const;
		tinymath::mat4x4 projection_matrix() const;
		void rotate(float yaw_offset, float pitch_offset);
		void update_rotation();
		void update_projection();
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