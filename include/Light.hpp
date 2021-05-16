#ifndef _LIGHT_
#define _LIGHT_
#include "Color.hpp"
#include "Vector3.hpp"
#include "Matrix4x4.hpp"

namespace Guarneri {
	struct Light {
		Color ambient;
		Color diffuse;
		Color specular;
		float intensity;
		Light();
	};

	struct DirectionalLight : Light {
		float yaw;
		float pitch;
		Vector3 forward;
		Vector3 right;
		Vector3 up;
		Vector3 position;
		Matrix4x4 p;

		DirectionalLight();
		Matrix4x4 light_space() const;
		Matrix4x4 view_matrix() const;
		Matrix4x4 projection_matrix() const;
		void rotate(const float& yaw_offset, const float& pitch_offset);
		void update_rotation();
	};

	struct PointLight : Light {
		float constant;
		float linear;
		float quadratic;
		Vector3 position;
		PointLight();
	};

	struct SpotLight : Light {
		Vector3 position;
		Vector3 spot_direction;
		float phi;
		float theta;
	};
}
#endif