#include "CubeMap.hpp"
#include <assert.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Define.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "Utility.hpp"
#include "Logger.hpp"

namespace Guarneri
{
	CubeMap::CubeMap()
	{
		this->wrap_mode = WrapMode::REPEAT;
		this->filtering = Filtering::POINT;
	}

	CubeMap::CubeMap(const char* path)
	{
		deserialize(path, *this);
	}

	CubeMap::CubeMap(const char* name, const std::string& path)
	{
		this->name = name;
		this->wrap_mode = WrapMode::REPEAT;
		this->filtering = Filtering::POINT;
		reload(path);
	}

	void CubeMap::reload(const std::string& path)
	{
		texture_path = path;
		texture = Texture::load_raw(path);
		texture->wrap_mode = this->wrap_mode;
		texture->filtering = this->filtering;
	}

	std::shared_ptr<CubeMap> CubeMap::load_asset(std::string path)
	{
		std::shared_ptr<CubeMap> cubemap = std::shared_ptr<CubeMap>(new CubeMap());
		CubeMap::deserialize(path, *cubemap);
		return cubemap;
	}	

	std::shared_ptr<CubeMap> CubeMap::load_asset(const char* path)
	{
		std::shared_ptr<CubeMap> cubemap = std::shared_ptr<CubeMap>(new CubeMap());
		CubeMap::deserialize(path, *cubemap);
		return cubemap;
	}

	constexpr Vector2 kInvAtan = Vector2(0.1591f, 0.3183f);

	Vector2 CubeMap::sample_spherical_map(Vector3 dir)
	{
		dir = Vector3::normalize(dir);
		Vector2 uv = Vector2(atan2(dir.z, dir.x), asin(dir.y));
		uv = Vector2(uv.x * kInvAtan.x, uv.y * kInvAtan.y);
		uv += 0.5f;
		return uv;
	}
	 
	bool CubeMap::sample(const Vector3& dir, Color& ret)
	{
		auto uv = sample_spherical_map(dir);
		return texture->sample(uv.x, uv.y, ret);
	}

	//Vector2 CubeMap::sample(const Vector3& dir, int& index)
	//{
	//	Vector3 abs_dir = Vector3::abs(dir);
	//	float max_axis = std::max(std::max(abs_dir.x, abs_dir.y), abs_dir.z);
	//	if (abs_dir.x > abs_dir.y && abs_dir.x > abs_dir.z)
	//	{
	//		// x is the max axis 
	//		if (dir.x > 0)
	//		{
	//			index = 0;
	//			return Vector2(1.0f - (dir.z + 1.0f) / 2.0f, (dir.y + 1.0f) / 2.0f);
	//		}
	//		else 
	//		{
	//			index = 1;
	//			return Vector2((dir.z + 1.0f) / 2.0f, (dir.y + 1.0f) / 2.0f);
	//		}
	//	}
	//	else if (abs_dir.y > abs_dir.x && abs_dir.y > abs_dir.z)
	//	{
	//		// y is the max axis
	//		if (dir.y > 0)
	//		{
	//			index = 2;
	//			return Vector2((dir.x + 1.0f) / 2.0f, 1.0f - (dir.z + 1.0f) / 2.0f);
	//		}
	//		else 
	//		{
	//			index = 3;
	//			return Vector2((dir.x + 1.0f) / 2.0f, (dir.z + 1.0f) / 2.0f);
	//		}
	//	}
	//	else 
	//	{
	//		// z is the max axis
	//		if (dir.z > 0)
	//		{
	//			index = 4;
	//			return Vector2((dir.x + 1.0f) / 2.0f, (dir.y + 1.0f) / 2.0f);
	//		}
	//		else 
	//		{
	//			index = 5;
	//			return Vector2(1.0f - (dir.x + 1.0f) / 2.0f, (dir.y + 1.0f) / 2.0f);
	//		}
	//	}
	//}

	void CubeMap::serialize(const CubeMap& cube_map, std::string path)
	{
		rapidjson::Document doc;
		doc.SetObject();

		rapidjson::Value name;
		name.SetString(cube_map.name.c_str(), doc.GetAllocator());
		doc.AddMember("name", name, doc.GetAllocator());

		rapidjson::Value cube_map_path;
		cube_map_path.SetString(path.c_str(), doc.GetAllocator());
		doc.AddMember("meta_path", cube_map_path, doc.GetAllocator());

		rapidjson::Value right;
		right.SetString(cube_map.texture_path.c_str(), doc.GetAllocator());
		doc.AddMember("hdr_texture", right, doc.GetAllocator());

		doc.AddMember("wrap_mode", (int32_t)cube_map.wrap_mode, doc.GetAllocator());

		doc.AddMember("filtering", (int32_t)cube_map.filtering, doc.GetAllocator());

		std::filesystem::path abs_path(ASSETS_PATH + path);
		if (!std::filesystem::exists(abs_path.parent_path()))
		{
			std::filesystem::create_directories(abs_path.parent_path());
		}
		std::FILE* fd = fopen(abs_path.string().c_str(), "w+");
		if (fd != nullptr)
		{
			char write_buffer[256];
			rapidjson::FileWriteStream fs(fd, write_buffer, sizeof(write_buffer));
			rapidjson::PrettyWriter<rapidjson::FileWriteStream> material_writer(fs);
			doc.Accept(material_writer);
			fclose(fd);
			LOG("save cubemap: {}", path);
		}
		else
		{
			ERROR("path does not exist: {}", ASSETS_PATH + path);
		}
	}

	void CubeMap::deserialize(std::string path, CubeMap& cubemap)
	{
		std::FILE* fd = fopen((ASSETS_PATH + path).c_str(), "r");
		if (fd != nullptr)
		{
			LOG("deserialize: {}", ASSETS_PATH + path);
			char read_buffer[256];
			rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
			rapidjson::Document doc;
			doc.ParseStream(fs);
			const char* texture_path = doc["texture_path"].GetString();
			fclose(fd);
			cubemap.name = doc["name"].GetString();
			cubemap.meta_path = doc["meta_path"].GetString();
			cubemap.wrap_mode = (WrapMode)doc["wrap_mode"].GetInt();
			cubemap.filtering = (Filtering)doc["filtering"].GetInt();
			cubemap.reload(texture_path);
		}
		else
		{
			ERROR("path does not exist: {}", ASSETS_PATH + path);
		}
	}
}