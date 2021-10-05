#pragma once
#include <memory>
#include "BaseInspector.hpp"
#include "Transform.hpp"

namespace CpuRasterizor
{
	class TransformInspector : public BaseInspector
	{
	public:
		TransformInspector() {
			std::fill(std::begin(position), std::end(position), 0.0f);
			std::fill(std::begin(angles), std::end(angles), 0.0f);
			std::fill(std::begin(scale), std::end(scale), 0.0f);
		}

		void on_gui(Transform& transform);

	private:
		float position[3];
		float angles[3];
		float scale[3];
	};
}