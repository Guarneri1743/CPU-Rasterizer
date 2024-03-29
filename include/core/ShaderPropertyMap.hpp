#pragma once
#include <unordered_map>
#include <memory>
#include "Define.hpp"
#include "tinymath.h"

namespace CpuRasterizer
{
	class Texture;
	class CubeMap;
	class RenderTexture;
	class ShaderPropertyMap
	{
	public:
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, tinymath::vec4f> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, tinymath::mat4x4> name2mat4x4;
		std::unordered_map<property_name, std::string> keywords;
		std::unordered_map<property_name, std::shared_ptr<Texture>> name2tex;
		std::unordered_map<property_name, std::shared_ptr<CubeMap>> name2cubemap;
		std::unordered_map<property_name, std::shared_ptr<RenderTexture>> name2rendertexture;

		// runtime
		std::unordered_map<property_name, uint32_t> name2tex_id;
		std::unordered_map<property_name, uint32_t> name2cubemap_id;
		std::unordered_map<property_name, uint32_t> name2rendertexture_id;

		ShaderPropertyMap() {}
		ShaderPropertyMap(const ShaderPropertyMap& other);
		ShaderPropertyMap& operator=(const ShaderPropertyMap& other);

		bool has_int(property_name name) const;
		bool has_float4(property_name name) const;
		bool has_float(property_name name) const;
		bool has_texture(property_name name) const;
		bool has_cubemap(property_name name) const;
		bool has_rendertexture(property_name name) const;
		bool has_mat4x4(property_name name) const;
		void set_int(property_name name, int val);
		void set_float4(property_name name, const tinymath::vec4f& val);
		void set_float(property_name name, float val);
		void set_mat4x4(property_name name, const tinymath::mat4x4& val);
		void set_texture(property_name name, std::shared_ptr<Texture> tex);
		void set_cubemap(property_name name, std::shared_ptr<CubeMap> cubemap);
		void set_rendertexture(property_name name, std::shared_ptr<RenderTexture> buffer);
		int get_int(property_name name) const;
		tinymath::vec4f get_float4(property_name name) const;
		float get_float(property_name name) const;
		tinymath::mat4x4 get_mat4x4(property_name name) const;
		std::shared_ptr<Texture> get_texture(property_name name) const;
		std::shared_ptr<CubeMap> get_cubemap(property_name name) const;
		std::shared_ptr<RenderTexture> get_framebuffer(property_name name) const;

		static ShaderPropertyMap global_shader_properties;

	private:
		void copy(const ShaderPropertyMap& other);
	};
}