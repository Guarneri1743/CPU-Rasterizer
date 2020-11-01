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
		float3 direction;
	};

	struct spot_light : light {
		float3 position;
		float3 spot_direction;
		float phi;
		float theta;
	};
}