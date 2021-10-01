#ifndef _PBR_
#define _PBR_

#include "Define.hpp"
#include "tinymath.h"

namespace Guarneri
{
    inline float distribution_ggx(const tinymath::vec3f& normal, const tinymath::vec3f& half_way, const float& roughness)
    {
        float a = roughness * roughness;
        float a2 = a * a;
        float ndh = tinymath::max(tinymath::dot(normal, half_way), 0.0f);
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

    inline float geometry_smith(const tinymath::vec3f& normal, const tinymath::vec3f& view_dir, const tinymath::vec3f& light_dir, const float& roughness)
    {
        float ndv = tinymath::max(tinymath::dot(normal, view_dir), 0.0f);
        float ndl = tinymath::max(tinymath::dot(normal, light_dir), 0.0f);

        float ggx2 = geometry_cchlick_ggx(ndv, roughness);
        float ggx1 = geometry_cchlick_ggx(ndl, roughness);

        return ggx1 * ggx2;
    }

    inline tinymath::vec3f fresnel_schlick(const float& cos_theta, const tinymath::vec3f& f0)
    {
        float omc = 1.0f - cos_theta;

        return f0 + (1.0f - f0) * omc * omc * omc * omc * omc;
    }

    inline tinymath::vec3f fresnel_schlick_roughness(float cos_theta, tinymath::vec3f f0, float roughness)
    {
        return f0 + (tinymath::max(tinymath::vec3f(1.0f - roughness), f0) - f0) * tinymath::pow(tinymath::clamp(1.0f - cos_theta, 0.0f, 1.0f) , 5.0f);
    }
}

#endif