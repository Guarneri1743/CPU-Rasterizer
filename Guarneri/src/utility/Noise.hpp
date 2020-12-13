#ifndef _NOISE_
#define _NOISE_
#include <Guarneri.hpp>

namespace Guarneri {
	struct Noise {
		static void generate_fractal_image(std::shared_ptr<Texture> image, int w, int h) {
            double    cr, ci;
            double nextr, nexti;
            double prevr, previ;

            const int max_iterations = 1000;

            for (int y = 0; y < h; ++y)
            {
                for (int x = 0; x < w; ++x)
                {
                    cr = 1.5 * (2.0f* x / w - 1.0) - 0.5;
                    ci = (2.0f* y / h - 1.0);

                    nextr = nexti = 0;
                    prevr = previ = 0;

                    Vector4 c = Vector4(0.4f, 0.6f, 0.8f, 1.0f);
                    image->write(x, y, c);

                    for (int i = 0; i < max_iterations; ++i)
                    { 
                        prevr = nextr;
                        previ = nexti;

                        nextr = prevr * prevr - previ * previ + cr;
                        nexti = 2 * prevr * previ + ci;

                        if (((nextr * nextr) + (nexti * nexti)) > 4)
                        {
                            const double z = sqrt(nextr * nextr + nexti * nexti);

                            /*const uint32_t index = static_cast<uint32_t>
                                (1000.0f* log2(1.75 + i - log2(log2(z))) / log2(max_iterations));*/
                            //float n = (float)(10 * log2(1.75 + i - log2(log2(z))) / log2(max_iterations));
                            c = Vector4(0.2f, 0.4f, 0.6f, 1.0f);
                            image->write(x, y, c);

                            break;
                        }
                    }
                }
            }
		}
	};
}
#endif