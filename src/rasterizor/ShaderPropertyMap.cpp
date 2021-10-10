#include "ShaderPropertyMap.hpp"

namespace CpuRasterizor
{
	ShaderPropertyMap ShaderPropertyMap::global_shader_properties;

	ShaderPropertyMap::ShaderPropertyMap(const ShaderPropertyMap& other)
	{
		this->copy(other);
	}

	ShaderPropertyMap& ShaderPropertyMap::operator=(const ShaderPropertyMap& other)
	{
		this->copy(other);
		return *this;
	}

	bool ShaderPropertyMap::has_int(property_name name) const
	{
		return name2int.count(name) > 0;
	}

	bool ShaderPropertyMap::has_float4(property_name name) const
	{
		return name2float4.count(name) > 0;
	}

	bool ShaderPropertyMap::has_float(property_name name) const
	{
		return name2float.count(name) > 0;
	}

	bool ShaderPropertyMap::has_texture(property_name name) const
	{
		return name2tex.count(name) > 0;
	}

	bool ShaderPropertyMap::has_cubemap(property_name name) const
	{
		return name2cubemap.count(name) > 0;
	}

	bool ShaderPropertyMap::has_framebuffer(property_name name) const
	{
		return name2framebuffer.count(name) > 0;
	}

	bool ShaderPropertyMap::has_mat4x4(property_name name) const
	{
		return name2mat4x4.count(name) > 0;
	}

	void ShaderPropertyMap::set_int(property_name name, int val)
	{
		name2int[name] = val;
	}

	void ShaderPropertyMap::set_float4(property_name name, const tinymath::vec4f& val)
	{
		name2float4[name] = val;
	}

	void ShaderPropertyMap::set_float(property_name name, float val)
	{
		name2float[name] = val;
	}

	void ShaderPropertyMap::set_mat4x4(property_name name, const tinymath::mat4x4& val)
	{
		name2mat4x4[name] = val;
	}

	void ShaderPropertyMap::set_texture(property_name name, std::shared_ptr<Texture> tex)
	{
		if (tex == nullptr)
		{
			return;
		}
		name2tex[name] = tex;
	}

	void ShaderPropertyMap::set_cubemap(property_name name, std::shared_ptr<CubeMap> cubemap)
	{
		if (cubemap == nullptr)
		{
			return;
		}
		name2cubemap[name] = cubemap;
	}

	void ShaderPropertyMap::set_framebuffer(property_name name, std::shared_ptr<RenderTexture> buffer)
	{
		if (buffer == nullptr)
		{
			return;
		}
		name2framebuffer[name] = buffer;
	}

	int ShaderPropertyMap::get_int(property_name name) const
	{
		if (name2int.count(name) > 0)
		{
			return name2int.at(name);
		}
		return 0;
	}

	tinymath::vec4f ShaderPropertyMap::get_float4(property_name name) const
	{
		if (name2float4.count(name) > 0)
		{
			return name2float4.at(name);
		}
		return 0;
	}

	float ShaderPropertyMap::get_float(property_name name) const
	{
		if (name2float.count(name) > 0)
		{
			return name2float.at(name);
		}
		return 0;
	}

	tinymath::mat4x4 ShaderPropertyMap::get_mat4x4(property_name name) const
	{
		if (name2mat4x4.count(name) > 0)
		{
			return name2mat4x4.at(name);
		}
		return tinymath::kMat4x4Identity;
	}

	std::shared_ptr<Texture> ShaderPropertyMap::get_texture(property_name name) const
	{
		if (name2tex.count(name) > 0)
		{
			return name2tex.at(name);
		}
		return nullptr;
	}

	std::shared_ptr<CubeMap> ShaderPropertyMap::get_cubemap(property_name name) const
	{
		if (name2cubemap.count(name) > 0)
		{
			return name2cubemap.at(name);
		}
		return nullptr;
	}

	std::shared_ptr<RenderTexture> ShaderPropertyMap::get_framebuffer(property_name name) const
	{
		if (name2framebuffer.count(name) > 0)
		{
			return name2framebuffer.at(name);
		}
		return nullptr;
	}

	void ShaderPropertyMap::copy(const ShaderPropertyMap& other)
	{
		this->name2float = other.name2float;
		this->name2float4 = other.name2float4;
		this->name2mat4x4 = other.name2mat4x4;
		this->name2int = other.name2int;
		this->name2tex = other.name2tex;
		this->name2cubemap = other.name2cubemap;
		this->name2framebuffer = other.name2framebuffer;
		this->keywords = other.keywords;
	}
}