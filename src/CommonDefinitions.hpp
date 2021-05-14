#ifndef _COMMON_DEF_
#define _COMMON_DEF_
#include <stdint.h>

namespace Guarneri
{
	// optimize unordered_map performance
	// typedef std::string property_name;
	typedef uint32_t property_name;
	const property_name albedo_prop = 0; //"texture_diffuse/albedo";
	const property_name normal_prop = 1; // "texture_normal";
	const property_name ao_prop = 2; // "texture_ao";
	const property_name emission_prop = 3; // "texture_emission"
	const property_name height_prop = 4; // "texture_emission"
	const property_name f0_prop = 5; // "texture_f0"

	// specular/glossiness workflow
	const property_name specular_prop = 11; // "texture_specular";
	const property_name glossiness_prop = 12; // "texture_glossiness";

	// metallic/roughness workflow
	const property_name metallic_prop = 13; // "texture_metallic"
	const property_name roughness_prop = 14; // "texture_roughness"
	const property_name base_color_prop = 15; //"texture_base_color";

	// other
	const property_name cubemap_prop = 20; // "skybox_cubemap";
	const property_name shadowmap_prop = 21; // "shadowmap";

	// statistics
	struct GraphicsStatistic
	{
		uint32_t triangle_count;
		uint32_t culled_triangle_count;
		uint32_t culled_backface_triangle_count;
		uint32_t earlyz_optimized;
	};

	// pixel block
	typedef struct
	{
		uint32_t start_x; uint32_t start_y; uint32_t size;
	} PixelBlock;

	// framebuffer color 
	typedef struct
	{
		uint8_t r; uint8_t g; uint8_t b;
	} color_rgb;

	typedef struct
	{
		uint8_t r; uint8_t g; uint8_t b; uint8_t a;
	} color_rgba;

	typedef struct
	{
		uint8_t b; uint8_t g; uint8_t r; uint8_t a;
	} color_bgra;

	typedef struct
	{
		uint8_t r; uint8_t g;
	}color_rg;

	typedef struct
	{
		uint8_t gray;
	}color_gray;

	typedef uint8_t image_ubyte;
}

#endif