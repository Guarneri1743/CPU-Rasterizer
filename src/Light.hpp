#ifndef _LIGHT_
#define _LIGHT_
#include <Guarneri.hpp>

namespace Guarneri {
	struct Light {
		Color ambient;
		Color diffuse;
		Color specular;
		float intensity;
	};

	struct DirectionalLight : Light {
		DirectionalLight() {
			ambient = Color::BLACK;
			diffuse = Color::WHITE;
			specular = Color::WHITE;
			intensity = 1.0f;
			this->yaw = -210.0f;
			this->pitch = 70.0f;
			//this->p = Matrix4x4::perspective(45.0f, 800.0f/600.0f, 0.5f, 500.0f);
			this->p = Matrix4x4::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
			update_rotation();
		}

		float yaw;
		float pitch;
		Vector3 forward;
		Vector3 right;
		Vector3 up;
		Vector3 position;
		Matrix4x4 p;

		Matrix4x4 light_space() const {
			auto v = Matrix4x4::lookat(position, position + forward, Vector3::UP);
			auto ret = p * v;
			return ret;
		}

		void rotate(const float& yaw_offset, const float& pitch_offset) {
			this->yaw -= yaw_offset;
			this->pitch -= pitch_offset;
			this->pitch = std::clamp(this->pitch, -89.0f, 89.0f);
			update_rotation();
		}

		void update_rotation() {
			forward.x = cos(DEGREE2RAD(this->yaw)) * cos(DEGREE2RAD(this->pitch));
			forward.y = sin(DEGREE2RAD(this->pitch));
			forward.z = sin(DEGREE2RAD(this->yaw)) * cos(DEGREE2RAD(this->pitch));
			forward = Vector3::normalize(forward);
			Vector3::calculate_right_up(forward, right, up);
		}
	};

	struct SpotLight : Light {
		Vector3 position;
		Vector3 spot_direction;
		float phi;
		float theta;
	};
}
#endif