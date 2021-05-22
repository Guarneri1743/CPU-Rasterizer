#include "CubeMap.hpp"
#include <assert.h>
#include <iostream>
#include "Define.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "Utility.hpp"

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

	CubeMap::CubeMap(const char* name, const std::vector<std::string>& paths)
	{
		this->name = name;
		this->wrap_mode = WrapMode::REPEAT;
		this->filtering = Filtering::POINT;
		reload(paths);
	}

	void CubeMap::reload(const std::vector<std::string>& paths)
	{
		assert(paths.size() == 6);
		texture_paths = paths;
		auto right = Texture::load_raw(paths[0]);
		auto left = Texture::load_raw(paths[1]);
		auto top = Texture::load_raw(paths[2]);
		auto bottom = Texture::load_raw(paths[3]);
		auto front = Texture::load_raw(paths[4]);
		auto back = Texture::load_raw(paths[5]);
		textures.emplace_back(right);
		textures.emplace_back(left);
		textures.emplace_back(top);
		textures.emplace_back(bottom);
		textures.emplace_back(front);
		textures.emplace_back(back);
		for (auto& tex : textures)
		{
			tex->wrap_mode = this->wrap_mode;
			tex->filtering = this->filtering;
		}
	}

	std::shared_ptr<CubeMap> CubeMap::create()
	{
		return std::shared_ptr<CubeMap>(new CubeMap());
	}

	std::shared_ptr<CubeMap> CubeMap::create(std::string path)
	{
		std::shared_ptr<CubeMap> cubemap = std::shared_ptr<CubeMap>(new CubeMap());
		CubeMap::deserialize(path, *cubemap);
		return cubemap;
	}	

	std::shared_ptr<CubeMap> CubeMap::create(const char* path)
	{
		std::shared_ptr<CubeMap> cubemap = std::shared_ptr<CubeMap>(new CubeMap());
		CubeMap::deserialize(path, *cubemap);
		return cubemap;
	}

	std::shared_ptr<CubeMap> CubeMap::create(const char* name, std::vector<std::string> paths)
	{
		return std::shared_ptr<CubeMap>(new CubeMap(name, paths));
	}

	std::shared_ptr<CubeMap> CubeMap::create(std::string name, std::vector<std::string> paths)
	{
		return std::shared_ptr<CubeMap>(new CubeMap(name.c_str(), paths));
	}
	 
	bool CubeMap::sample(const Vector3& dir, Color& ret)
	{
		int index = -1;
		auto uv = sample(dir, index);
		if (index >= 0 && index < textures.size())
		{
			return textures[index]->sample(uv.x, uv.y, ret);
		}
		return false;
	}

	Vector2 CubeMap::sample(const Vector3& dir, int& index)
	{
		Vector3 abs_dir = Vector3::abs(dir);
		float max_axis = std::max(std::max(abs_dir.x, abs_dir.y), abs_dir.z);
		Vector3 normalized_dir = dir / max_axis;
		if (abs_dir.x > abs_dir.y && abs_dir.x > abs_dir.z)
		{
			if (dir.x >= 0)
			{
				index = 0;
				return Vector2(1.0f - (normalized_dir.z + 1.0f) / 2.0f, (normalized_dir.y + 1.0f) / 2.0f);
			}
			else
			{
				index = 1;
				return Vector2((normalized_dir.z + 1.0f) / 2.0f, (normalized_dir.y + 1.0f) / 2.0f);
			}
		}
		else if (abs_dir.y > abs_dir.x && abs_dir.y > abs_dir.z)
		{
			if (dir.y >= 0)
			{
				index = 2;
				return Vector2((normalized_dir.x + 1.0f) / 2.0f, 1.0f - (normalized_dir.z + 1.0f) / 2.0f);
			}
			else
			{
				index = 3;
				return Vector2((normalized_dir.x + 1.0f) / 2.0f, (normalized_dir.z + 1.0f) / 2.0f);
			}
		}
		else
		{
			if (dir.z >= 0)
			{
				index = 4;
				return Vector2((normalized_dir.x + 1.0f) / 2.0f, (normalized_dir.y + 1.0f) / 2.0f);
			}
			else
			{
				index = 5;
				return Vector2(1.0f - (normalized_dir.x + 1.0f) / 2.0f, (normalized_dir.y + 1.0f) / 2.0f);
			}
		}
	}

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
		right.SetString(cube_map.texture_paths[0].c_str(), doc.GetAllocator());
		doc.AddMember("right", right, doc.GetAllocator());

		rapidjson::Value left;
		left.SetString(cube_map.texture_paths[1].c_str(), doc.GetAllocator());
		doc.AddMember("left", left, doc.GetAllocator());

		rapidjson::Value top;
		top.SetString(cube_map.texture_paths[2].c_str(), doc.GetAllocator());
		doc.AddMember("top", top, doc.GetAllocator());

		rapidjson::Value bottom;
		bottom.SetString(cube_map.texture_paths[3].c_str(), doc.GetAllocator());
		doc.AddMember("bottom", bottom, doc.GetAllocator());

		rapidjson::Value front;
		front.SetString(cube_map.texture_paths[4].c_str(), doc.GetAllocator());
		doc.AddMember("front", front, doc.GetAllocator());

		rapidjson::Value back;
		back.SetString(cube_map.texture_paths[5].c_str(), doc.GetAllocator());
		doc.AddMember("back", back, doc.GetAllocator());

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
			std::cout << "save cubemap: " << path << std::endl;
		}
		else
		{
			std::cout << "path does not exist: " << ASSETS_PATH + path << std::endl;
		}
	}

	void CubeMap::deserialize(std::string path, CubeMap& cubemap)
	{
		std::FILE* fd = fopen((ASSETS_PATH + path).c_str(), "r");
		if (fd != nullptr)
		{
			std::cout << "deserialize: " << ASSETS_PATH + path << std::endl;
			char read_buffer[256];
			rapidjson::FileReadStream fs(fd, read_buffer, sizeof(read_buffer));
			rapidjson::Document doc;
			doc.ParseStream(fs);
			const char* right = doc["right"].GetString();
			const char* left = doc["left"].GetString();
			const char* top = doc["top"].GetString();
			const char* bottom = doc["bottom"].GetString();
			const char* front = doc["front"].GetString();
			const char* back = doc["back"].GetString();
			fclose(fd);
			std::vector<std::string> paths;
			paths.push_back(right);
			paths.push_back(left);
			paths.push_back(top);
			paths.push_back(bottom);
			paths.push_back(front);
			paths.push_back(back);
			cubemap.name = doc["name"].GetString();
			cubemap.meta_path = doc["meta_path"].GetString();
			cubemap.wrap_mode = (WrapMode)doc["wrap_mode"].GetInt();
			cubemap.filtering = (Filtering)doc["filtering"].GetInt();
			cubemap.reload(paths);
		}
		else
		{
			std::cout << "path does not exist: " << ASSETS_PATH + path << std::endl;
		}
	}
}