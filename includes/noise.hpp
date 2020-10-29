#pragma once 
#include <common.hpp>
#include <float3.hpp>
#include <float4.hpp>
#include <algorithm>

namespace guarneri {
	const float F3 = 0.3333333;
	const float G3 = 0.1666667;

	struct noise {
		static void generate_fractal_image(texture<float>& image, int w, int h) {
            double    cr, ci;
            double nextr, nexti;
            double prevr, previ;

            const unsigned int max_iterations = 1000;

            for (unsigned int y = 0; y < h; ++y)
            {
                for (unsigned int x = 0; x < w; ++x)
                {
                    cr = 1.5 * (2.0 * x / w - 1.0) - 0.5;
                    ci = (2.0 * y / h - 1.0);

                    nextr = nexti = 0;
                    prevr = previ = 0;

                    for (unsigned int i = 0; i < max_iterations; ++i)
                    {
                        prevr = nextr;
                        previ = nexti;

                        nextr = prevr * prevr - previ * previ + cr;
                        nexti = 2 * prevr * previ + ci;

                        if (((nextr * nextr) + (nexti * nexti)) > 4)
                        {
                            using namespace std;

                            const double z = sqrt(nextr * nextr + nexti * nexti);

                            /*const unsigned int index = static_cast<unsigned int>
                                (1000.0 * log2(1.75 + i - log2(log2(z))) / log2(max_iterations));*/
                            float c = (float)(1000.0 * log2(1.75 + i - log2(log2(z))) / log2(max_iterations));

                            image.write(x, y, c);

                            break;
                        }
                    }
                }
            }
		}
	};
}