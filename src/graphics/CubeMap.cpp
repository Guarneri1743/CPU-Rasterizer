#include "CubeMap.hpp"
#include <assert.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <execution>
#include "Define.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "Utility.hpp"
#include "Logger.hpp"
#include "Cache.hpp"
#include "stb_image.h"
#include "Sampling.hpp"

namespace Guarneri
{
	constexpr uint32_t kMaxPrefilteredMapCount = 4;

	CubeMap::CubeMap()
	{
		this->wrap_mode = WrapMode::REPEAT;
		this->filtering = Filtering::POINT;
	}

	CubeMap::CubeMap(const char* path)
	{
		std::shared_ptr<CubeMap> map;
		spawn(path, map);
		this->copy_from(*map);
	}

	void CubeMap::reload(const std::string& path)
	{
		texture_path = path;
		texture = Texture::load_raw(path);
		texture->wrap_mode = this->wrap_mode;
		texture->filtering = this->filtering;
		precompute_ibl_textures();
	}

	std::shared_ptr<CubeMap> CubeMap::load_asset(std::string path)
	{
		std::shared_ptr<CubeMap> map;
		spawn(path, map);
		return map;
	}	

	void CubeMap::copy_from(const CubeMap& cubemap)
	{
		this->name = cubemap.name;
		this->meta_path = cubemap.meta_path;
		this->texture_path = cubemap.texture_path;
		this->texture = cubemap.texture;
		this->filtering = cubemap.filtering;
		this->wrap_mode = cubemap.wrap_mode;
		this->irradiance_map = cubemap.irradiance_map;
		this->prefiltered_maps = cubemap.prefiltered_maps;
		this->brdf_lut = cubemap.brdf_lut;
	}

	std::shared_ptr<CubeMap> CubeMap::load_asset(const char* path)
	{
		std::shared_ptr<CubeMap> map;
		spawn(path, map);
		return map;
	}
	 
	bool CubeMap::sample(const Vector3& dir, Color& ret)
	{
		auto uv = spherical_coord_to_uv(dir);
		return texture->sample(uv.x, uv.y, ret);
	}

	bool CubeMap::sample_irradiance_map(const Vector3& dir, Color& ret)
	{
		auto uv = spherical_coord_to_uv(dir);
		return irradiance_map->sample(uv.x, uv.y, ret);
	}

	bool CubeMap::sample_prefilter_map(const Vector3& dir, Color& ret)
	{
		auto uv = spherical_coord_to_uv(dir);
		return prefiltered_maps[0]->sample(uv.x, uv.y, ret);
	}

	bool CubeMap::sample_prefilter_map_lod(const Vector3& dir, const float& lod, Color& ret)
	{
		// todo: mipmap
		auto uv = spherical_coord_to_uv(dir);
		uint32_t mip = std::clamp((uint32_t)(lod * (float)kMaxPrefilteredMapCount), 0u, kMaxPrefilteredMapCount - 1);
		return prefiltered_maps[mip]->sample(uv.x, uv.y, ret);
	}

	bool CubeMap::sample_brdf(const Vector2& uv, Color& ret)
	{
		return brdf_lut->sample(uv.x, uv.y, ret);
	}

	void CubeMap::precompute_ibl_textures()
	{
		if (texture != nullptr)
		{
			precompute_irradiance_map();

			prefiltered_maps.clear();
			for (uint32_t mip = 0; mip < kMaxPrefilteredMapCount; mip++) 
			{
				precompute_prefilter_map(mip);
			}

			precompute_brdf_lut();
		}
		else
		{
			LOG("no original texture");
		}
	}

	constexpr float kSampleStep = 0.1f;

	void CubeMap::precompute_irradiance_map()
	{
		size_t begin = texture_path.find(".hdr");
		std::string irradiance_path = texture_path;
		irradiance_path = irradiance_path.replace(begin, begin + 4, "_irradiance.hdr");

		std::filesystem::path abs_path(RES_PATH + irradiance_path);
		if (std::filesystem::exists(abs_path))
		{
			irradiance_map = Texture::load_raw(irradiance_path);
			return;
		}

		irradiance_map = std::make_shared<Texture>(texture->width, texture->height, TextureFormat::rgb16f);
		irradiance_map->filtering = Filtering::POINT;

		std::vector<std::pair<uint32_t, uint32_t>> indexers;
		indexers.reserve((size_t)texture->width * (size_t)texture->height);
		for (uint32_t row = 0; row < texture->height; row++)
		{
			for (uint32_t col = 0; col < texture->width; col++)
			{
				indexers.push_back({ row, col });
			}
		}

		std::for_each(
			std::execution::par_unseq,
			indexers.begin(),
			indexers.end(),
			[this](auto&& coord)
		{
			uint32_t row = coord.first;
			uint32_t col = coord.second;
			float u, v;
			pixel2uv(texture->width, texture->height, row, col, u, v);
			Vector2 uv = { u, v };
			Vector3 normal = uv_to_spherical_coord(uv);

			Color irradiance = Color(0.0f);
			float step = kSampleStep;
			float samples = 0.0f;

			// brute force sampling (whole sphere) 
			for (float phi = 0.0f; phi < TWO_PI; phi += step)
			{
				for (float theta = 0.0f; theta < PI; theta += step)
				{
					Vector3 w_i = radian_to_spherical_coord(theta, phi);
					float ndl = Vector3::dot(w_i, normal);
					if (ndl > 0.0f)
					{
						Color color;
						sample(w_i, color);
						irradiance += color * ndl;
					}
					samples += 1.0f;
				}
			}
			
			irradiance *= PI;
			irradiance /= samples;
			irradiance_map->write(texture->height - 1 - row, col, irradiance);
		});

		Texture::export_image(*irradiance_map, irradiance_path);
	}

	constexpr uint32_t kSampleCount = 1024u;

	void CubeMap::precompute_prefilter_map(const uint32_t& mip)
	{
		float roughness = mip / (float)(kMaxPrefilteredMapCount - 1);
		size_t begin = texture_path.find(".hdr");
		std::string prefilter_map_path = texture_path;
		prefilter_map_path = prefilter_map_path.replace(begin, begin + 4, "_prefilter_" + std::to_string(mip) +".hdr");

		if (std::filesystem::exists(RES_PATH + prefilter_map_path))
		{
			prefiltered_maps.push_back(Texture::load_raw(prefilter_map_path));
			return;
		}

		auto prefilter_map = std::make_shared<Texture>(texture->width, texture->height, TextureFormat::rgb16f);
		prefilter_map->filtering = Filtering::POINT;

		std::vector<std::pair<uint32_t, uint32_t>> indexers;
		indexers.reserve((size_t)texture->width * (size_t)texture->height);
		for (uint32_t row = 0; row < texture->height; row++)
		{
			for (uint32_t col = 0; col < texture->width; col++)
			{
				indexers.push_back({ row, col });
			}
		}

		float r = roughness;
		std::for_each(
			std::execution::par_unseq,
			indexers.begin(),
			indexers.end(),
			[this, r, prefilter_map](auto&& coord)
		{
			uint32_t row = coord.first;
			uint32_t col = coord.second;
			float u, v;
			pixel2uv(texture->width, texture->height, row, col, u, v);
			Vector2 uv = { u, v };
			Vector3 normal = uv_to_spherical_coord(uv);

			// Epic approximation
			Vector3 reflect_dir = normal;
			Vector3 view_dir = reflect_dir;

			float total_weight = 0.0f;
			Color prefilter_color = Color(0.0f);

			for (uint32_t sample_index = 0u; sample_index < kSampleCount; sample_index++)
			{
				Vector2 random_dir = random_vec2_01(kSampleCount);
				Vector3 half_way = importance_sampling(random_dir, normal, r);
				Vector3 w_i = Vector3::normalize(2.0f * std::max(Vector3::dot(view_dir, half_way), 0.0f) * half_way - view_dir);
				float ndl = Vector3::dot(normal, w_i);
				if (ndl > 0.0f)
				{
					Color color;
					sample(w_i, color);
					prefilter_color += color * ndl;
					total_weight += ndl;
				}
			}

			prefilter_color /= total_weight;
			prefilter_map->write(texture->height - 1 - row, col, prefilter_color);
		});

		prefiltered_maps.push_back(prefilter_map);
		Texture::export_image(*prefilter_map, prefilter_map_path);
	}

	constexpr int brdf_size = 512;

	void CubeMap::precompute_brdf_lut()
	{
		size_t begin = texture_path.find(".hdr");
		std::string brdf_lut_path = texture_path;
		brdf_lut_path = brdf_lut_path.replace(begin, begin + 4, "_brdf_lut.hdr");

		if (std::filesystem::exists(RES_PATH + brdf_lut_path))
		{
			brdf_lut = Texture::load_raw(brdf_lut_path);
			return;
		}

		brdf_lut = std::make_shared<Texture>(brdf_size, brdf_size, TextureFormat::rgb16f);
		brdf_lut->filtering = Filtering::POINT;

		std::vector<std::pair<uint32_t, uint32_t>> indexers;
		indexers.reserve((size_t)(brdf_size) * (size_t)(brdf_size));
		for (uint32_t row = 0; row < brdf_size; row++)
		{
			for (uint32_t col = 0; col < brdf_size; col++)
			{
				indexers.push_back({ row, col });
			}
		}

		uint32_t size = brdf_size;
		std::for_each(
			std::execution::par_unseq,
			indexers.begin(),
			indexers.end(),
			[this, size](auto&& coord)
		{
			uint32_t row = coord.first;
			uint32_t col = coord.second;

			float u, v;
			pixel2uv(size, size, row, col, u, v);

			float ndv = std::max(u, 1.0f / (float)brdf_size);
			float roughness = std::max(v, 1.0f / (float)brdf_size);

			Vector3 view_dir;
			view_dir.x = sqrt(1.0f - ndv * ndv);
			view_dir.y = ndv;
			view_dir.z = 0.0f;

			Vector3 normal = Vector3::UP;

			Color brdf = Color(0.0f);

			for (uint32_t sample_index = 0u; sample_index < kSampleCount; sample_index++)
			{
				Vector2 random_dir = hammersley(sample_index, kSampleCount);
				Vector3 half_way = importance_sampling(random_dir, normal, roughness);
				Vector3 w_i = Vector3::normalize(2.0f * Vector3::dot(view_dir, half_way) * half_way - view_dir);

				float ndl = std::max(w_i.y, 0.0f);
				float ndh = std::max(half_way.y, 0.0f);
				float vdh = std::max(Vector3::dot(view_dir, half_way), 0.0f);

				if (ndl > 0.0f)
				{
					float geo = geometry_smith(normal, view_dir, w_i, roughness);
					float geo_vis = (geo * vdh) / (ndh * ndv);
					float fc = std::pow(1.0f - vdh, 5.0f);

					brdf.r += (1.0f - fc) * geo_vis;
					brdf.g += fc * geo_vis;
				}
			}

			brdf /= (float)kSampleCount;
			brdf_lut->write(size - row - 1, col, brdf);
		});

		Texture::export_image(*brdf_lut, brdf_lut_path);
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
			LOG("save cubemap: {}", path.c_str());
		}
		else
		{
			ERROR("path does not exist: {}", (ASSETS_PATH + path).c_str());
		}
	}

	void CubeMap::spawn(std::string path, std::shared_ptr<CubeMap>& cubemap)
	{
		cubemap = std::shared_ptr<CubeMap>(new CubeMap());
		CubeMap::deserialize(path, *cubemap);
	}


	void CubeMap::deserialize(std::string path, CubeMap& cubemap)
	{
		std::FILE* fd = fopen((ASSETS_PATH + path).c_str(), "r");
		if (fd != nullptr)
		{
			LOG("deserialize: {}", (ASSETS_PATH + path).c_str());
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