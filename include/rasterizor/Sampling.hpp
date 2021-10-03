#pragma once
#include "Define.hpp"
#include "tinymath.h"
#include "PBR.hpp"

#define USE_LOW_DISCREPANCY_SEQUENCE

namespace Guarneri
{
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
        float x = cos(theta) * cos(phi);
        float y = sin(theta);
        float z = cos(theta) * sin(phi);
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

    inline tinymath::vec2f hammersley(const uint32_t& i, const uint32_t& N)
    {
        return tinymath::vec2f(float(i) / float(N), radical_inverse_vdc(i));
    }

    inline tinymath::vec2f random_vec2_01(const uint32_t& N)
    {
        return tinymath::vec2f((float)(rand() % N) / (float)N, (float)(rand() % N) / (float)N);
    }

    inline tinymath::vec3f radian_to_spherical_coord(const float& theta, const float& phi)
    {
        float x = cos(theta) * cos(phi);
        float y = sin(theta);
        float z = cos(theta) * sin(phi);
        return tinymath::normalize(tinymath::vec3f(x, y, z));
    }

    inline tinymath::vec3f importance_sampling(const tinymath::vec2f& random_01, const tinymath::vec3f& normal, const float& roughness)
    {
        float a = roughness * roughness;
        float phi = TWO_PI * random_01.x;
        float cos_theta = sqrt((1.0f - random_01.y) / (1.0f + (a * a - 1.0f) * random_01.y));
        float sin_theta = sqrt(1 - cos_theta * cos_theta);
        
        float x = sin_theta * cos(phi);
        float y = cos_theta; 
        float z = sin_theta * sin(phi);

        tinymath::vec3f w_i = tinymath::normalize(tinymath::vec3f(x, y, z));

        tinymath::vec3f bitangent = normal.y < 0.999f ? tinymath::kVec3fLeft : tinymath::kVec3fRight;
        tinymath::vec3f tangent = tinymath::normalize(tinymath::cross(bitangent, normal));
        bitangent = tinymath::cross(normal, tangent);

        return tinymath::normalize(w_i.x * tangent + w_i.y * normal + w_i.z * bitangent);
    } 
}