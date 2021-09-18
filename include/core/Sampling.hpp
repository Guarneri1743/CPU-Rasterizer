#ifndef _SAMPLING_
#define _SAMPLING_

#include "Define.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"

namespace Guarneri
{
    inline Vector3 cos_radian_to_spherical_coord(const float& cos_theta, const float& cos_phi, const float& sin_theta, const float& sin_phi)
    {
        float x = cos_theta * cos_phi;
        float y = sin_theta;
        float z = cos_theta * sin_phi;
        return Vector3(x, y, z).normalized();
    }

    inline Vector3 radian_to_spherical_coord(const float& theta, const float& phi)
    {
        return cos_radian_to_spherical_coord(cos(theta), cos(phi), sin(theta), sin(phi));
    }

    inline Vector2 spherical_coord_to_radian(const Vector3& coord)
    {
        auto direction = Vector3::normalize(coord);
        return Vector2(atan2(direction.z, direction.x), asin(direction.y));
    }

    inline Vector2 spherical_coord_to_uv(const Vector3& coord)
    {
        Vector2 radian = spherical_coord_to_radian(coord);
        Vector2 uv = Vector2(radian.x / TWO_PI, radian.y / PI) + 0.5f;
        return uv;
    }

    inline Vector3 uv_to_spherical_coord(const Vector2& uv)
    {
        float phi = (uv.x - 0.5f) * TWO_PI;
        float theta = (uv.y - 0.5f) * PI;
        return radian_to_spherical_coord(theta, phi);
    }

    inline float radical_inverse_vdc(uint32_t bits)
    {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        return (float)(bits) * 2.3283064365386963e-10f;
    }

    inline Vector2 hammersley(const uint32_t& i, const uint32_t& normal)
    {
        return Vector2((float)i / (float)normal, radical_inverse_vdc(i));
    }

    inline float distribution_ggx(const Vector3& normal, const Vector3& half_way, const float& roughness)
    {
        float a = roughness * roughness;
        float a2 = a * a;
        float ndh = std::max(Vector3::dot(normal, half_way), 0.0f);
        float ndh_2 = ndh * ndh;

        float nom = a2;
        float denom = (ndh_2 * (a2 - 1.0f) + 1.0f);
        denom = PI * denom * denom;

        return nom / denom;
    }

    inline Vector3 importance_sample_ggx(const Vector2& random_dir, const Vector3& normal, const float& roughness)
    {
        float a = roughness * roughness;

        float phi = TWO_PI * random_dir.x;
        float cos_theta = sqrt((1.0f - random_dir.y) / (1.0f + (a * a - 1.0f) * random_dir.y));
        float sin_theta = sqrt(1 - cos_theta * cos_theta);
        
        // from spherical coordinates to cartesian coordinates
        Vector3 H;
        H.x = cos(phi) * sin_theta;
        H.y = sin(phi) * sin_theta;
        H.z = cos_theta;

        auto N = normal;
        // from tangent-space vector to world-space sample vector
        Vector3 up = std::abs(N.z) < 0.999 ? Vector3(0.0, 0.0, 1.0) : Vector3(1.0, 0.0, 0.0);
        Vector3 tangent = Vector3::normalize(Vector3::cross(up, N));
        Vector3 bitangent = Vector3::cross(N, tangent);

        Vector3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
        return  Vector3::normalize(sampleVec);


        //return uv_to_spherical_coord(random_dir);
    } 
}

#endif