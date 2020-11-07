#pragma once
#include <guarneri.hpp>

namespace guarneri {
	struct light {
		color ambient;
		color diffuse;
		color specular;
		float intensity;
	};

	struct directional_light : light {
		Vector3 direction;
	};

	struct spot_light : light {
		Vector3 position;
		Vector3 spot_direction;
		float phi;
		float theta;
	};
}