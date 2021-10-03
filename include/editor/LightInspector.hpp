#pragma once
#include <memory>
#include "BaseInspector.hpp"
#include "Light.hpp"

namespace Guarneri
{
	class LightInspector : public BaseInspector
	{
	public:
		LightInspector()
		{
			std::fill(std::begin(main_light_ambient), std::end(main_light_ambient), 0.0f);
			std::fill(std::begin(main_light_diffuse), std::end(main_light_diffuse), 0.0f);
			std::fill(std::begin(main_light_specular), std::end(main_light_specular), 0.0f);
			main_light_yaw = 0.0f;
			main_light_pitch = 0.0f;
		}

		void on_gui(DirectionalLight& light);

	private:
		float main_light_ambient[4];
		float main_light_diffuse[4];
		float main_light_specular[4];
		float main_light_yaw;
		float main_light_pitch;
	};
}