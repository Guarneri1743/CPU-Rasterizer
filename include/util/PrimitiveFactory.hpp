#pragma once
#include <stdint.h>
#include <memory>
#include "tinymath.h"
#include "Model.hpp"
#include "RasterAttributes.hpp"

namespace CpuRasterizor {
	class PrimitiveFactory {
	public:
		static std::shared_ptr<Model> sphere() { return sphere(nullptr); }
		static std::shared_ptr<Model> plane() { return plane(nullptr); }
		static std::shared_ptr<Model> cube() { return cube(nullptr); }
		static std::shared_ptr<Model> skybox() { return skybox(nullptr); }

		static std::shared_ptr<Model> sphere(std::shared_ptr<Material> material);
		static std::shared_ptr<Model> plane(std::shared_ptr<Material> material);
		static std::shared_ptr<Model> cube(std::shared_ptr<Material> material);
		static std::shared_ptr<Model> skybox(std::shared_ptr<Material> material);
	};
}