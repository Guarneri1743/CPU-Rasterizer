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
			direction = Vector3(1.0f, 1.0f, 1.0f);
			light_proj = Matrix4x4::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.05f, 50.0f);
			light_view = Matrix4x4::lookat(-direction * 10.0f, Vector3(), Vector3::UP);
			light_space = light_proj * light_view;
		}

		void update_direction(const Vector3& dir) {
			direction = dir;
			light_proj = Matrix4x4::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.05f, 50.0f);
			light_view = Matrix4x4::lookat(-direction * 10.0f, Vector3(), Vector3::UP);
			light_space = light_proj * light_view;
		}

		Vector3 direction;
		Matrix4x4 light_view;
		Matrix4x4 light_proj;
		Matrix4x4 light_space;
	};

	struct SpotLight : Light {
		Vector3 position;
		Vector3 spot_direction;
		float phi;
		float theta;
	};
}
#endif