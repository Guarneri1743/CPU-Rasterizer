#include "MaterialInspector.hpp"
#include <filesystem>
#include "Define.hpp"
#include "Utility.hpp"
#include "Serialization.hpp"
#include "imgui/imgui.h"
#include "tinymath/tinymath.h"
#include "ShaderProgram.hpp"

namespace CpuRasterizer
{
	void draw_stencil_op(const char* button, const char* popup, StencilOp& op, int& selected_op)
	{
		const StencilOp funcs[] =
		{
			StencilOp::kDecrement,
			StencilOp::kDecrementWrap,
			StencilOp::kIncrement,
			StencilOp::kIncrementWrap,
			StencilOp::kInvert,
			StencilOp::kKeep,
			StencilOp::kReplace,
			StencilOp::kZero
		};

		const char* func_names[] =
		{
			"kDecrement",
			"kDecrementWrap",
			"kIncrement",
			"kIncrementWrap",
			"kInvert",
			"kKeep",
			"kReplace",
			"kZero"
		};

		if (ImGui::Button(button))
			ImGui::OpenPopup(popup);
		ImGui::SameLine();
		ImGui::TextUnformatted(selected_op == -1 ? "<None>" : func_names[selected_op]);
		if (ImGui::BeginPopup(popup))
		{
			ImGui::Separator();
			for (int i = 0; i < IM_ARRAYSIZE(func_names); i++)
				if (ImGui::Selectable(func_names[i]))
				{
					selected_op = i;
					op = funcs[selected_op];
				}
			ImGui::EndPopup();
		}
		else
		{
			for (int i = 0; i < IM_ARRAYSIZE(func_names); ++i)
			{
				if (funcs[i] == op)
				{
					selected_op = i;
					break;
				}
			}
		}
	}

	void draw_compare_func(const char* button, const char* popup, CompareFunc& func, int& selected_compare_func)
	{
		const CompareFunc funcs[] =
		{
			CompareFunc::kAlways,
			CompareFunc::kEqual,
			CompareFunc::kGEqual,
			CompareFunc::kGreater,
			CompareFunc::kLEqual,
			CompareFunc::kLess,
			CompareFunc::kNever,
			CompareFunc::kNotEqual
		};

		const char* func_names[] =
		{
			"Always",
			"Equal",
			"GEqual",
			"Greater",
			"LEqual",
			"Less",
			"Never",
			"NotEqual"
		};

		if (ImGui::Button(button))
			ImGui::OpenPopup(popup);
		ImGui::SameLine();
		ImGui::TextUnformatted(selected_compare_func == -1 ? "<None>" : func_names[selected_compare_func]);
		if (ImGui::BeginPopup(popup))
		{
			ImGui::Separator();
			for (int i = 0; i < IM_ARRAYSIZE(func_names); i++)
				if (ImGui::Selectable(func_names[i]))
				{
					selected_compare_func = i;
					func = funcs[selected_compare_func];
				}
			ImGui::EndPopup();
		}
		else
		{
			for (int i = 0; i < IM_ARRAYSIZE(func_names); ++i)
			{
				if (funcs[i] == func)
				{
					selected_compare_func = i;
					break;
				}
			}
		}
	}

	void draw_blend_factor(const char* button, const char* popup, BlendFactor& factor, int& selected_factor)
	{
		const BlendFactor factors[] =
		{
			BlendFactor::kSrcAlpha,
			BlendFactor::kSrcColor,
			BlendFactor::kOneMinusSrcAlpha,
			BlendFactor::kOneMinusSrcColor,
			BlendFactor::kOne,
			BlendFactor::kDstAlpha,
			BlendFactor::kDstColor,
			BlendFactor::kOneMinusDstAlpha,
			BlendFactor::kOneMinusDstColor
		};

		const char* factor_names[] =
		{
			"SrcAlpha",
			"SrcColor",
			"OneMinusSrcAlpha",
			"OneMinusSrcColor",
			"One",
			"DstAlpha",
			"DstColor",
			"OneMinusDstAlpha",
			"OneMinusDstColor"
		};

		if (ImGui::Button(button))
			ImGui::OpenPopup(popup);
		ImGui::SameLine();
		ImGui::TextUnformatted(selected_factor == -1 ? "<None>" : factor_names[selected_factor]);
		if (ImGui::BeginPopup(popup))
		{
			ImGui::Separator();
			for (int i = 0; i < IM_ARRAYSIZE(factor_names); i++)
				if (ImGui::Selectable(factor_names[i]))
				{
					selected_factor = i;
					factor = factors[selected_factor];
				}
			ImGui::EndPopup();
		}
		else
		{
			for (int i = 0; i < IM_ARRAYSIZE(factor_names); ++i)
			{
				if (factors[i] == factor)
				{
					selected_factor = i;
					break;
				}
			}
		}
	}

	void draw_blend_func(const char* button, const char* popup, BlendFunc& func, int& selected_blend_func)
	{
		const BlendFunc funcs[] = { BlendFunc::kAdd, BlendFunc::kSub };
		const char* blend_modes[] = { "Add", "Sub" };
		if (ImGui::Button(button))
			ImGui::OpenPopup(popup);
		ImGui::SameLine();
		ImGui::TextUnformatted(selected_blend_func == -1 ? "<None>" : blend_modes[selected_blend_func]);
		if (ImGui::BeginPopup(popup))
		{
			ImGui::Separator();
			for (int i = 0; i < IM_ARRAYSIZE(blend_modes); i++)
				if (ImGui::Selectable(blend_modes[i]))
				{
					selected_blend_func = i;
					func = funcs[selected_blend_func];
				}
			ImGui::EndPopup();
		}
		else
		{
			for (int i = 0; i < IM_ARRAYSIZE(blend_modes); ++i)
			{
				if (funcs[i] == func)
				{
					selected_blend_func = i;
					break;
				}
			}
		}
	}

	void MaterialInspector::on_gui(Material& material)
	{
		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// shader selections
			{
				const char* shaders[] = { "default", "pbr_shader" };
				static int selected_shader = 0;
				if (ImGui::Button("Shaders.."))
					ImGui::OpenPopup("shaders");
				ImGui::SameLine();
				ImGui::TextUnformatted(selected_shader == -1 ? "<None>" : shaders[selected_shader]);
				if (ImGui::BeginPopup("shaders"))
				{
					ImGui::Separator();
					for (int i = 0; i < IM_ARRAYSIZE(shaders); i++)
						if (ImGui::Selectable(shaders[i]))
						{
							selected_shader = i;
							auto shader = ShaderLab::get_shader(shaders[selected_shader]);
							material.target_shader = shader;
						}
					ImGui::EndPopup();
				}
			}

			if (ImGui::ColorEdit4("TintColor", tint_color))
			{
				material.local_properties.set_float4(tint_color_prop, tinymath::vec4f(tint_color[0], tint_color[1], tint_color[2], tint_color[3]));
			}
			else
			{
				tint_color[0] = material.local_properties.get_float4(tint_color_prop).x;
				tint_color[1] = material.local_properties.get_float4(tint_color_prop).y;
				tint_color[2] = material.local_properties.get_float4(tint_color_prop).z;
				tint_color[3] = material.local_properties.get_float4(tint_color_prop).w;
			}

			if (ImGui::SliderFloat("roughness_multiplier", &roughness_multiplier, 0.0f, 2.0f))
			{
				material.local_properties.set_float(roughness_multiplier_prop, roughness_multiplier);
			}
			else
			{
				roughness_multiplier = material.local_properties.get_float(roughness_multiplier_prop);
			}

			if (ImGui::SliderFloat("roughness_offset", &roughness_offset, -1.0f, 1.0f))
			{
				material.local_properties.set_float(roughness_offset_prop, roughness_offset);
			}
			else
			{
				roughness_offset = material.local_properties.get_float(roughness_offset_prop);
			}

			if (ImGui::SliderFloat("metallic_multiplier", &metallic_multiplier, 0.0f, 2.0f))
			{
				material.local_properties.set_float(metallic_multiplier_prop, metallic_multiplier);
			}
			else
			{
				metallic_multiplier = material.local_properties.get_float(metallic_multiplier_prop);
			}

			if (ImGui::SliderFloat("metallic_offset", &metallic_offset, -1.0f, 1.0f))
			{
				material.local_properties.set_float(metallic_offset_prop, metallic_offset);
			}
			else
			{
				metallic_offset = material.local_properties.get_float(metallic_offset_prop);
			}

			ImGui::Checkbox("StencilTest", &material.stencil_on);

			static int stencilfunc, stencilpass, stencilfail, stencilzfail, depthfunc, blendfunc, blendsrc, blenddst;
			draw_compare_func("StencilFunc", "stencilfunc", material.stencil_func, stencilfunc);
			draw_stencil_op("StencilPassOp", "stencilpass", material.stencil_pass_op, stencilpass);
			draw_stencil_op("StencilFailOp", "stencilfail", material.stencil_fail_op, stencilfail);
			draw_stencil_op("StencilZFailOp", "stencilzfail", material.stencil_zfail_op, stencilzfail);

			static int stencil_ref, stencil_readmask, stencil_writemask;
			if (ImGui::InputInt("StencilRef", &stencil_ref))
			{
				material.stencil_ref_val = (stencil_t)stencil_ref;
			}
			else
			{
				stencil_ref = (int)material.stencil_ref_val;
			}

			if(ImGui::InputInt("StencilReadMask", &stencil_readmask))
			{
				material.stencil_read_mask = (stencil_t)stencil_readmask;
			}
			else
			{
				stencil_readmask = (int)material.stencil_read_mask;
			}

			if (ImGui::InputInt("StencilWriteMask", &stencil_writemask))
			{
				material.stencil_write_mask = (stencil_t)stencil_writemask;
			}
			else
			{
				stencil_writemask = (int)material.stencil_write_mask;
			}


			draw_compare_func("DepthFunc", "depthfunc", material.ztest_func, depthfunc);

			// ZWrite
			ImGui::Checkbox("Zwrite", &material.zwrite_on);

			// Cast Shadow
			ImGui::Checkbox("Cast Shadow", &material.cast_shadow);

			// face culling
			ImGui::Checkbox("Double Face", &material.double_face);

			// blend on/off
			ImGui::Checkbox("Blend On", &material.transparent);

			draw_blend_func("BlendFunc", "blendfunc", material.blend_op, blendfunc);

			draw_blend_factor("BlendSrcFactor", "blendsrcfactor", material.src_factor, blendsrc);

			draw_blend_factor("BlendDstFactor", "blenddstfactor", material.dst_factor, blenddst);

			ImGui::InputInt("RenderQueue", &material.render_queue);
			
			if (ImGui::Button("Save"))
			{
				std::filesystem::path path(ASSETS_PATH + material.meta_path);
				Serializer::serialize(material, material.meta_path);
			}
		}
	}
}