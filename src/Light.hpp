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
		}
		Vector3 direction;
	};

	struct SpotLight : Light {
		Vector3 position;
		Vector3 spot_direction;
		float phi;
		float theta;
	};
}
#endif