#ifndef _SAMPLING_
#define _SAMPLING_

#include "Define.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "PBR.hpp"

#define USE_LOW_DISCREPANCY_SEQUENCE

namespace Guarneri
{
    inline Vector2 spherical_coord_to_uv(const Vector3& coord)
    {
        auto direction = Vector3::normalize(coord);
        Vector2 radian = Vector2(atan2(direction.z, direction.x), asin(direction.y));
        Vector2 uv = Vector2(radian.x / TWO_PI, radian.y / PI) + 0.5f;
        return uv;
    }

    inline Vector3 uv_to_spherical_coord(const Vector2& uv)
    {
        float phi = (uv.x - 0.5f) * TWO_PI;
        float theta = (uv.y - 0.5f) * PI;
        float x = cos(theta) * cos(phi);
        float y = sin(theta);
        float z = cos(theta) * sin(phi);
        return Vector3(x, y, z).normalized();
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

    inline Vector2 hammersley(const uint32_t& i, const uint32_t& N)
    {
        return Vector2(float(i) / float(N), radical_inverse_vdc(i));
    }

    inline Vector2 random_vec2_01(const uint32_t& i, const uint32_t& N)
    {
    #ifdef USE_LOW_DISCREPANCY_SEQUENCE
        return hammersley(i, N);
    #else
        return Vector2((float)(rand() % N) / (float)N, (float)(rand() % N) / (float)N);
    #endif
    }

    inline Vector3 radian_to_spherical_coord(const float& theta, const float& phi)
    {
        float x = cos(theta) * cos(phi);
        float y = sin(theta);
        float z = cos(theta) * sin(phi);
        return Vector3(x, y, z).normalized();
    }

    inline Vector3 importance_sampling(const Vector2& random_01, const Vector3& normal, const float& roughness)
    {
        float a = roughness * roughness;
        float phi = TWO_PI * random_01.x;
        float cos_theta = sqrt((1.0f - random_01.y) / (1.0f + (a * a - 1.0f) * random_01.y));
        float sin_theta = sqrt(1 - cos_theta * cos_theta);
        
        float x = sin_theta * cos(phi);
        float y = cos_theta; 
        float z = sin_theta * sin(phi);

        Vector3 w_i = Vector3(x, y, z).normalized();

        Vector3 bitangent = normal.y < 0.999f ? Vector3::LEFT : Vector3::RIGHT;
        Vector3 tangent = Vector3::normalize(Vector3::cross(bitangent, normal));
        bitangent = Vector3::cross(normal, tangent);

        return (w_i.x * tangent + w_i.y * normal + w_i.z * bitangent).normalized();
    } 
}

#endif