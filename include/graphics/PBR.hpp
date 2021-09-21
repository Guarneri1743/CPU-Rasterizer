#ifndef _PBR_
#define _PBR_

#include "Define.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"

namespace Guarneri
{
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

    inline float geometry_cchlick_ggx(const float& ndv, const float& roughness)
    {
        float a = roughness;
        float k = (a * a) / 2.0f;

        float nom = ndv;
        float denom = ndv * (1.0f - k) + k;

        return nom / denom;
    }

    inline float geometry_smith(const Vector3& normal, const Vector3& view_dir, const Vector3& light_dir, const float& roughness)
    {
        float ndv = std::max(Vector3::dot(normal, view_dir), 0.0f);
        float ndl = std::max(Vector3::dot(normal, light_dir), 0.0f);

        float ggx2 = geometry_cchlick_ggx(ndv, roughness);
        float ggx1 = geometry_cchlick_ggx(ndl, roughness);

        return ggx1 * ggx2;
    }

    inline Vector3 fresnel_schlick(const float& cos_theta, const Vector3& F0)
    {
        float omc = 1.0f - cos_theta;

        return F0 + (1.0f - F0) * omc * omc * omc * omc * omc;
    }

    inline Vector3 fresnel_schlick_roughness(float cosTheta, Vector3 F0, float roughness)
    {
        return F0 + (std::max(Vector3(1.0f - roughness), F0) - F0) * std::pow(1.0f - cosTheta, 5.0f);
    }
}

#endif