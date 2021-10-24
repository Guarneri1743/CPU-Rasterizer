#include "CubeMap.hpp"
#include <assert.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <execution>
#include "Define.hpp"
#include "Utility.hpp"
#include "Logger.hpp"
#include "Cache.hpp"
#include "stb_image.h"
#include "Sampling.hpp"
#include "Serialization.hpp"
#include "Texture.hpp"

namespace CpuRasterizer
{
	CubeMap::CubeMap()
	{
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

	bool CubeMap::sample(const tinymath::vec3f& dir, tinymath::Color& ret)
	{
		auto uv = spherical_coord_to_uv(dir);
		return texture->sample(uv.x, uv.y, ret);
	}

	bool CubeMap::sample_irradiance_map(const tinymath::vec3f& dir, tinymath::Color& ret)
	{
		auto uv = spherical_coord_to_uv(dir);
		return irradiance_map->sample(uv.x, uv.y, ret);
	}

	bool CubeMap::sample_prefilter_map(const tinymath::vec3f& dir, tinymath::Color& ret)
	{
		auto uv = spherical_coord_to_uv(dir);
		return prefiltered_maps[0]->sample(uv.x, uv.y, ret);
	}

	bool CubeMap::sample_prefilter_map_lod(const tinymath::vec3f& dir, float lod, tinymath::Color& ret)
	{
		auto uv = spherical_coord_to_uv(dir);
		float mip = tinymath::clamp((lod * (float)kMaxMip), 0.f, (float)(kMaxMip - 1));
		int mip_int = (int)tinymath::floor(mip);
		float frac = mip - (float)mip_int;

		if (mip_int < kMaxMip - 1)
		{
			int next_mip = mip_int + 1;
			tinymath::Color c1, c2;
			prefiltered_maps[mip_int]->read(uv.x, uv.y, c1);
			prefiltered_maps[next_mip]->read(uv.x, uv.y, c2);
			ret = c1 + (c2 - c1) * frac;
			return true;
		}
		else
		{
			return prefiltered_maps[mip_int]->read(uv.x, uv.y, ret);
		}
	}

	bool CubeMap::sample_brdf(const tinymath::vec2f& uv, tinymath::Color& ret)
	{
		return brdf_lut->sample(uv.x, uv.y, ret);
	}

	void CubeMap::precompute_ibl_textures()
	{
		if (texture != nullptr)
		{
			precompute_irradiance_map();

			prefiltered_maps.clear();
			for (size_t mip = 0; mip < kMaxMip; mip++)
			{
				precompute_prefilter_map_fast(mip);
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

		irradiance_map = std::make_shared<Texture>(texture->width, texture->height, TextureFormat::kRGB16);

		std::vector<std::pair<size_t, size_t>> indexers;
		indexers.reserve((size_t)texture->width * (size_t)texture->height);
		for (size_t row = 0; row < texture->height; row++)
		{
			for (size_t col = 0; col < texture->width; col++)
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
			size_t row = coord.first;
			size_t col = coord.second;
			float u, v;
			pixel2uv(texture->width, texture->height, row, col, u, v);
			tinymath::vec2f uv = { u, v };
			tinymath::vec3f normal = uv_to_spherical_coord(uv);

			tinymath::Color irradiance = tinymath::Color(0.0f);
			float step = kSampleStep;
			float samples = 0.0f;

			// brute force sampling (whole sphere) 
			for (float phi = 0.0f; phi < TWO_PI; phi += step)
			{
				for (float theta = -HALF_PI; theta < HALF_PI; theta += step)
				{
					tinymath::vec3f w_i = radian_to_spherical_coord(theta, phi);
					float ndl = tinymath::dot(w_i, normal);
					if (ndl > 0.0f)
					{
						tinymath::Color color;
						sample(w_i, color);
						irradiance += color * ndl;
					}

					samples += 1.0f;
				}
			}

			irradiance *= PI;
			irradiance /= samples;
			irradiance_map->write(row, col, irradiance);
		});

		Texture::export_image(*irradiance_map, irradiance_path);
	}

	constexpr size_t kSampleCount = 2048u;

	void CubeMap::precompute_prefilter_map(size_t mip)
	{
		size_t begin = texture_path.find(".hdr");
		std::string prefilter_map_path = texture_path;
		if (mip > 0)
		{
			prefilter_map_path = prefilter_map_path.replace(begin, begin + 4, "_prefilter_" + std::to_string(mip) + ".hdr");
		}

		if (std::filesystem::exists(RES_PATH + prefilter_map_path))
		{
			auto tex = Texture::load_raw(prefilter_map_path);
			prefiltered_maps.push_back(tex);
			return;
		}

		auto prefilter_map = std::make_shared<Texture>(texture->width, texture->height, TextureFormat::kRGB16);

		prefilter_map->copy(*texture);
		prefilter_map->resize(prefilter_map->width >> mip, prefilter_map->height >> mip);

		if (mip > 0)
		{
			std::vector<std::pair<size_t, size_t>> indexers;
			indexers.reserve((size_t)prefilter_map->width * (size_t)prefilter_map->height);
			for (size_t row = 0; row < prefilter_map->height; row++)
			{
				for (size_t col = 0; col < prefilter_map->width; col++)
				{
					indexers.push_back({ row, col });
				}
			}

			float roughness = mip / (float)(kMaxMip - 1);
			float r = roughness;
			std::for_each(
				std::execution::par_unseq,
				indexers.begin(),
				indexers.end(),
				[this, r, prefilter_map](auto&& coord)
			{
				size_t row = coord.first;
				size_t col = coord.second;
				float u, v;
				pixel2uv(texture->width, texture->height, row, col, u, v);
				tinymath::vec2f uv = { u, v };
				tinymath::vec3f normal = uv_to_spherical_coord(uv);

				// Epic approximation
				tinymath::vec3f reflect_dir = normal;
				tinymath::vec3f view_dir = reflect_dir;

				float total_weight = 0.0f;
				tinymath::Color prefilter_color = tinymath::Color(0.0f);

				for (size_t sample_index = 0u; sample_index < kSampleCount; sample_index++)
				{
					tinymath::vec2f random_dir = hammersley((uint32_t)sample_index, (uint32_t)kSampleCount);
					//tinymath::vec2f random_dir = random_vec2_01(kSampleCount);
					tinymath::vec3f half_way = importance_sampling(random_dir, normal, r);
					tinymath::vec3f w_i = tinymath::normalize(2.0f * tinymath::max(tinymath::dot(view_dir, half_way), 0.0f) * half_way - view_dir);
					float ndl = tinymath::dot(normal, w_i);
					if (ndl > 0.0f)
					{
						tinymath::Color color;
						sample(w_i, color);
						prefilter_color += color * ndl;
						total_weight += ndl;
					}
				}

				prefilter_color /= total_weight;
				prefilter_map->write(row, col, prefilter_color);
			});
		}

		prefiltered_maps.push_back(prefilter_map);
		Texture::export_image(*prefilter_map, prefilter_map_path);
	}

	void CubeMap::precompute_prefilter_map_fast(size_t mip)
	{
		size_t begin = texture_path.find(".hdr");
		std::string prefilter_map_path = texture_path;

		if (mip > 0)
		{
			prefilter_map_path = prefilter_map_path.replace(begin, begin + 4, "_prefilter_" + std::to_string(mip) + ".hdr");
		}

		if (std::filesystem::exists(RES_PATH + prefilter_map_path))
		{
			auto tex = Texture::load_raw(prefilter_map_path);
			prefiltered_maps.push_back(tex);
			return;
		}

		auto prefilter_map = std::make_shared<Texture>(texture->width, texture->height, TextureFormat::kRGB16);

		prefilter_map->copy(*texture);

		bool horizontal = false;
		float weights[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };


		if (mip > 0)
		{
			int iterations = 2 * (int)mip;

			assert(iterations % 2 == 0);

			prefilter_map->resize(prefilter_map->width >> mip, prefilter_map->height >> mip);

			for (int scale = 0; scale < iterations; ++scale)
			{
				std::vector<std::pair<size_t, size_t>> indexers;
				indexers.reserve((size_t)prefilter_map->width * (size_t)prefilter_map->height);
				for (size_t row = 0; row < prefilter_map->height; row++)
				{
					for (size_t col = 0; col < prefilter_map->width; col++)
					{
						indexers.push_back({ row, col });
					}
				}

				for (int i = 0; i < iterations; i++)
				{
					std::for_each(
						std::execution::par_unseq,
						indexers.begin(),
						indexers.end(),
						[this, &horizontal, &weights, &prefilter_map](auto&& coord)
					{
						int row = (int)coord.first;
						int col = (int)coord.second;

						tinymath::Color prefilter_color;
						prefilter_map->read((size_t)row, (size_t)col, prefilter_color);
						prefilter_color *= weights[0];

						if (horizontal)
						{
							for (int idx = 1; idx < 5; idx++)
							{
								float weight = weights[idx];
								tinymath::Color c1 = tinymath::kColorBlack;
								tinymath::Color c2 = tinymath::kColorBlack;
								prefilter_map->read((size_t)row, (size_t)(tinymath::min(col + idx, (int)prefilter_map->width)), c1);
								prefilter_map->read((size_t)row, (size_t)(tinymath::max(col - idx, 0)), c2);
								prefilter_color += c1 * weight;
								prefilter_color += c2 * weight;
							}
						}
						else
						{
							for (int idx = 1; idx < 5; idx++)
							{
								float weight = weights[idx];
								tinymath::Color c1 = tinymath::kColorBlack;
								tinymath::Color c2 = tinymath::kColorBlack;
								prefilter_map->read((size_t)(tinymath::min(row + idx, (int)prefilter_map->height)), (size_t)col, c1);
								prefilter_map->read((size_t)(tinymath::max(row - idx, 0)), (size_t)col, c2);
								prefilter_color += c1 * weight;
								prefilter_color += c2 * weight;
							}
						}

						prefilter_color.a = 1.0f;
						prefilter_map->write((size_t)row, (size_t)col, prefilter_color);

						horizontal = !horizontal;
					});
				}
			}
		}

		prefiltered_maps.push_back(prefilter_map);
		Texture::export_image(*prefilter_map, prefilter_map_path);
	}

	constexpr int brdf_size = 1024;

	void CubeMap::precompute_brdf_lut()
	{
		std::string brdf_lut_path = "/hdri/brdf_lut.hdr";

		if (std::filesystem::exists(RES_PATH + brdf_lut_path))
		{
			brdf_lut = Texture::load_raw(brdf_lut_path);		
			return;
		}

		brdf_lut = std::make_shared<Texture>(brdf_size, brdf_size, TextureFormat::kRGB16);

		std::vector<std::pair<size_t, size_t>> indexers;
		indexers.reserve((size_t)(brdf_size) * (size_t)(brdf_size));
		for (size_t row = 0; row < brdf_size; row++)
		{
			for (size_t col = 0; col < brdf_size; col++)
			{
				indexers.push_back({ row, col });
			}
		}

		size_t size = brdf_size;
		std::for_each(
			std::execution::par_unseq,
			indexers.begin(),
			indexers.end(),
			[this, size](auto&& coord)
		{
			size_t row = coord.first;
			size_t col = coord.second;

			float u, v;
			pixel2uv(size, size, row, col, u, v);

			float ndv = tinymath::max(u, 1.0f / (float)brdf_size);
			float roughness = tinymath::max(v, 1.0f / (float)brdf_size);

			tinymath::vec3f view_dir;
			view_dir.x = sqrt(1.0f - ndv * ndv);
			view_dir.y = ndv;
			view_dir.z = 0.0f;

			tinymath::vec3f normal = tinymath::kVec3fUp;

			tinymath::Color brdf = tinymath::Color(0.0f);

			for (size_t sample_index = 0u; sample_index < kSampleCount; sample_index++)
			{
				tinymath::vec2f random_dir = hammersley((uint32_t)sample_index, (uint32_t)kSampleCount);
				tinymath::vec3f half_way = importance_sampling(random_dir, normal, roughness);
				tinymath::vec3f w_i = tinymath::normalize(2.0f * tinymath::dot(view_dir, half_way) * half_way - view_dir);

				float ndl = tinymath::max(w_i.y, 0.0f);
				float ndh = tinymath::max(half_way.y, 0.0f);
				float vdh = tinymath::max(tinymath::dot(view_dir, half_way), 0.0f);

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
			brdf_lut->write(row, col, brdf);
		});

		Texture::export_image(*brdf_lut, brdf_lut_path);
	}

	void CubeMap::spawn(std::string path, std::shared_ptr<CubeMap>& cubemap)
	{
		cubemap = std::shared_ptr<CubeMap>(new CubeMap());
		Serializer::deserialize(path, *cubemap);
	}
}