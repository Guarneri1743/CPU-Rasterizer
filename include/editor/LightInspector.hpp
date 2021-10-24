#pragma once
#include <memory>
#include "BaseInspector.hpp"
#include "Light.hpp"

namespace CpuRasterizer
{
	class LightInspector : public BaseInspector
	{
	public:
		LightInspector()
		{
			std::fill(std::begin(ambient), std::end(ambient), 0.0f);
			std::fill(std::begin(diffuse), std::end(diffuse), 0.0f);
			std::fill(std::begin(specular), std::end(specular), 0.0f);
			yaw = 0.0f;
			pitch = 0.0f;
			l = -10.0f;
			r = 10.0f;
			b = -10.0f;
			t = 10.0f;
			n = 0.01f;
			f = 100.0f;
			distance = 10.0f;
		}

		void on_gui(DirectionalLight& light);

	private:
		float ambient[4];
		float diffuse[4];
		float specular[4];
		float yaw;
		float pitch;
		float l, r, b, t, n, f;
		float distance;
	};
}