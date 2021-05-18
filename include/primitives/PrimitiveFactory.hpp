#ifndef _PRIMITIVE_FACTORY_
#define _PRIMITIVE_FACTORY_
#include <stdint.h>
#include <memory>
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Model.hpp"
#include "Vertex.hpp"

namespace Guarneri {
	class PrimitiveFactory {
	public:
		static Vector3 cal_tangent(const Vertex& v1, const Vertex& v2, const Vertex& v3);
		static std::shared_ptr<Model> plane(std::shared_ptr<Material> material);
		static std::shared_ptr<Model> cube(std::shared_ptr<Material> material);
		static std::shared_ptr<Model> skybox(std::shared_ptr<Material> material);
	};
}
#endif