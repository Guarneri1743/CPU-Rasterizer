#pragma once
#include <string>
#include <memory>
#include "BaseInspector.hpp"
#include "Material.hpp"

namespace CpuRasterizer
{
	class MaterialInspector : public BaseInspector
	{
	public:
		MaterialInspector() {
			std::fill(std::begin(tint_color), std::end(tint_color), 0.0f);
			roughness_multiplier = 0.0f;
			roughness_offset = 0.0f;
			metallic_multiplier = 0.0f;
			metallic_offset = 0.0f;
		}

		void on_gui(Material& material);

	private:
		std::string target_shader;
		float tint_color[4];
		float roughness_multiplier;
		float roughness_offset;
		float metallic_multiplier;
		float metallic_offset;
	};
}