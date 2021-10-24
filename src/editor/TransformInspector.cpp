#include "TransformInspector.hpp"
#include "imgui/imgui.h"
#include "tinymath/tinymath.h"

namespace CpuRasterizer
{
	void TransformInspector::on_gui(Transform& transform)
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::InputFloat3("position", position))
			{
				transform.set_local_position({ position[0], position[1], position[2] });
			}
			else
			{
				position[0] = transform.local_position.x;
				position[1] = transform.local_position.y;
				position[2] = transform.local_position.z;
			}

			if (ImGui::InputFloat3("angles", angles))
			{
				transform.set_local_rotation({ DEGREE2RAD(angles[0]), DEGREE2RAD(angles[1]), DEGREE2RAD(angles[2]) });
			}
			else
			{
				angles[0] = RAD2DEGREE(transform.local_rotation.x);
				angles[1] = RAD2DEGREE(transform.local_rotation.y);
				angles[2] = RAD2DEGREE(transform.local_rotation.z);
			}

			if (ImGui::InputFloat3("scale", scale))
			{
				transform.set_local_scale({ scale[0], scale[1], scale[2] });
			}
			else
			{
				scale[0] = transform.local_scale.x;
				scale[1] = transform.local_scale.y;
				scale[2] = transform.local_scale.z;
			}
		}
	}
}