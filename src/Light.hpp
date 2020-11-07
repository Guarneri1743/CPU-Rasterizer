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