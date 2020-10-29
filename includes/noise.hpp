#pragma once 
#include <common.hpp>
#include <float3.hpp>
#include <float4.hpp>
#include <algorithm>

namespace guarneri {
	const float F3 = 0.3333333;
	const float G3 = 0.1666667;

	struct noise {
		static float3 random3(float3 c) {
			float j = 4096.0 * sin(float3::dot(c, float3(17.0, 59.4, 15.0)));
			float3 r;
			r.z = FRAC(512.0f * j);
			j *= .125;
			r.x = FRAC(512.0 * j);
			j *= .125;
			r.y = FRAC(512.0 * j);
			return r - 0.5;
		}

		static float snoise(float3 p) {

			float3 s = float3::floor(p + float3::dot(p, float3(F3)));
			float3 x = p - s + float3::dot(s, float3(G3));

			float3 e = STEP(float3(0.0f), x - x.yzx());
			float3 i1 = e * (1.0 - e.zxy());
			float3 i2 = 1.0 - e.zxy() * (1.0 - e);

			float3 x1 = x - i1 + G3;
			float3 x2 = x - i2 + 2.0 * G3;
			float3 x3 = x - 1.0 + 3.0 * G3;

			float4 w, d;

			w.x = float3::dot(x, x);
			w.y = float3::dot(x1, x1);
			w.z = float3::dot(x2, x2);
			w.w = float3::dot(x3, x3);

			w = float3::maxf((0.6f - w).xyz(), 0.0f);

			d.x = float3::dot(random3(s), x);
			d.y = float3::dot(random3(s + i1), x1);
			d.z = float3::dot(random3(s + i2), x2);
			d.w = float3::dot(random3(s + 1.0), x3);

			w *= w;
			w *= w;
			d *= w;

			return float4::dot(d, float4(52.0));
		}

		static float fractal_noise(float3 m) {
			return   0.5333333 * snoise(m)
				+ 0.2666667 * snoise(2.0 * m)
				+ 0.1333333 * snoise(4.0 * m)
				+ 0.0666667 * snoise(8.0 * m);
		}
	};
}