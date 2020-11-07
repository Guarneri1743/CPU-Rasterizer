#pragma once
#include <Guarneri.hpp>

namespace Guarneri {
	struct Light {
		Color ambient;
		Color diffuse;
		Color specular;
		float intensity;
	};

	struct directional_light : Light {
		Vector3 direction;
	};

	struct spot_light : Light {
		Vector3 position;
		Vector3 spot_direction;
		float phi;
		float theta;
	};
}