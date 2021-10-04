#pragma once
#include <unordered_map>
#include "Define.hpp"
#include "tinymath.h"
#include "Texture.hpp"
#include "CubeMap.hpp"
#include "RenderTexture.hpp"

namespace Guarneri
{
	class ShaderPropertyMap
	{
	public:
		std::unordered_map<property_name, float> name2float;
		std::unordered_map<property_name, tinymath::vec4f> name2float4;
		std::unordered_map<property_name, int> name2int;
		std::unordered_map<property_name, std::shared_ptr<Texture>> name2tex;
		std::unordered_map<property_name, std::shared_ptr<CubeMap>> name2cubemap;
		std::unordered_map<property_name, std::string> keywords;
		std::unordered_map<property_name, std::shared_ptr<RenderTexture>> name2framebuffer;

		ShaderPropertyMap() {}
		ShaderPropertyMap(const ShaderPropertyMap& other);
		ShaderPropertyMap& operator=(const ShaderPropertyMap& other);

		bool has_int(const property_name& name) const;
		bool has_float4(const property_name& name) const;
		bool has_float(const property_name& name) const;
		bool has_texture(const property_name& name) const;
		bool has_cubemap(const property_name& name) const;
		bool has_framebuffer(const property_name& name) const;
		void set_int(const property_name& name, const int& val);
		void set_float4(const property_name& name, const tinymath::vec4f& val);
		void set_float(const property_name& name, const float& val);
		void set_texture(const property_name& name, std::shared_ptr<Texture> tex);
		void set_cubemap(const property_name& name, std::shared_ptr<CubeMap> cubemap);
		void set_framebuffer(const property_name& name, std::shared_ptr<RenderTexture> buffer);
		int get_int(const property_name& name) const;
		tinymath::vec4f get_float4(const property_name& name) const;
		float get_float(const property_name& name) const;
		std::shared_ptr<Texture> get_texture(const property_name& name) const;
		std::shared_ptr<CubeMap> get_cubemap(const property_name& name) const;
		std::shared_ptr<RenderTexture> get_framebuffer(const property_name& name) const;

	private:
		void copy(const ShaderPropertyMap& other);
	};
}