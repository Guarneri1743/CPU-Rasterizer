#pragma once
#include "Define.hpp"
#include "tinymath.h"

#define USE_LOW_DISCREPANCY_SEQUENCE

inline tinymath::vec2f dir_to_uv(const tinymath::vec3f& dir, int& index)
	{
		tinymath::vec3f abs_dir = tinymath::abs(dir);
		if (abs_dir.x > abs_dir.y && abs_dir.x > abs_dir.z)
		{
			// x is the max axis 
			if (dir.x > 0)
			{
				index = 0;
				return tinymath::vec2f(1.0f - (dir.z + 1.0f) / 2.0f, (dir.y + 1.0f) / 2.0f);
			}
			else 
			{
				index = 1;
				return tinymath::vec2f((dir.z + 1.0f) / 2.0f, (dir.y + 1.0f) / 2.0f);
			}
		}
		else if (abs_dir.y > abs_dir.x && abs_dir.y > abs_dir.z)
		{
			// y is the max axis
			if (dir.y > 0)
			{
				index = 2;
				return tinymath::vec2f((dir.x + 1.0f) / 2.0f, 1.0f - (dir.z + 1.0f) / 2.0f);
			}
			else 
			{
				index = 3;
				return tinymath::vec2f((dir.x + 1.0f) / 2.0f, (dir.z + 1.0f) / 2.0f);
			}
		}
		else 
		{
			// z is the max axis
			if (dir.z > 0)
			{
				index = 4;
				return tinymath::vec2f((dir.x + 1.0f) / 2.0f, (dir.y + 1.0f) / 2.0f);
			}
			else 
			{
				index = 5;
				return tinymath::vec2f(1.0f - (dir.x + 1.0f) / 2.0f, (dir.y + 1.0f) / 2.0f);
			}
		}
	}


inline tinymath::vec2f spherical_coord_to_uv(const tinymath::vec3f& coord)
{
    auto direction = tinymath::normalize(coord);
    tinymath::vec2f radian = tinymath::vec2f(tinymath::atan2(direction.z, direction.x), tinymath::asin(direction.y));
    tinymath::vec2f uv = tinymath::vec2f(radian.x / TWO_PI, radian.y / PI) + 0.5f;
    return uv;
}

inline tinymath::vec3f uv_to_spherical_coord(const tinymath::vec2f& uv)
{
    float phi = (uv.x - 0.5f) * TWO_PI;
    float theta = (uv.y - 0.5f) * PI;
    float x = tinymath::cos(theta) * tinymath::cos(phi);
    float y = tinymath::sin(theta);
    float z = tinymath::cos(theta) * tinymath::sin(phi);
    return tinymath::normalize(tinymath::vec3f(x, y, z));
}

inline float radical_inverse_vdc(uint32_t bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10f;
}

inline tinymath::vec2f hammersley(uint32_t i, uint32_t N)
{
    return tinymath::vec2f(float(i) / float(N), radical_inverse_vdc(i));
}

inline tinymath::vec2f random_vec2_01(uint32_t N)
{
    return tinymath::vec2f((float)(rand() % N) / (float)N, (float)(rand() % N) / (float)N);
}

inline tinymath::vec3f radian_to_spherical_coord(float theta, float phi)
{
    float x = tinymath::cos(theta) * tinymath::cos(phi);
    float y = tinymath::sin(theta);
    float z = tinymath::cos(theta) * tinymath::sin(phi);
    return tinymath::normalize(tinymath::vec3f(x, y, z));
}

inline tinymath::vec3f tangent2world(const tinymath::vec3f& vec, const tinymath::vec3f& normal)
{
    tinymath::vec3f bitangent = normal.y < 0.999f ? tinymath::kVec3fLeft : tinymath::kVec3fRight;
    tinymath::vec3f tangent = tinymath::normalize(tinymath::cross(bitangent, normal));
    bitangent = tinymath::cross(normal, tangent);

    return tinymath::normalize(vec.x * tangent + vec.y * normal + vec.z * bitangent);
}

inline tinymath::vec3f importance_sampling(const tinymath::vec2f& random_01, const tinymath::vec3f& normal, float roughness)
{
    float a = roughness * roughness;
    float phi = TWO_PI * random_01.x;
    float cos_theta = sqrt((1.0f - random_01.y) / (1.0f + (a * a - 1.0f) * random_01.y));
    float sin_theta = sqrt(1 - cos_theta * cos_theta);
        
    float x = sin_theta * tinymath::cos(phi);
    float y = cos_theta; 
    float z = sin_theta * tinymath::sin(phi);

    tinymath::vec3f w_i = tinymath::normalize(tinymath::vec3f(x, y, z));

    return tangent2world(w_i, normal);
} 

inline float get_mip_level(const tinymath::vec2f& ddx_uv, const tinymath::vec2f& ddy_uv, size_t width, size_t height)
{
    tinymath::vec2f ddx = ddx_uv * tinymath::vec2f((float)width, (float)height);
    tinymath::vec2f ddy = ddy_uv * tinymath::vec2f((float)width, (float)height);
    float p = tinymath::max(tinymath::sqrt(tinymath::dot(ddx, ddx)), tinymath::sqrt(tinymath::dot(ddy, ddy)));
    float level = (tinymath::log2(p));
    return tinymath::clamp(level, 0.f, (float)(kMaxMip - 1));
}