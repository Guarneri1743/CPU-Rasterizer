#ifndef _MARCOS_
#define _MARCOS_

#define INVALID_ID 0
#define MAX_ID UINT_MAX
#define EPSILON 1e-04f
#define ONE_MINUS_EPSILON 1 - EPSILON
#define EQUALS(a, b) std::abs(a-b) <= EPSILON
#define LESS_THAN(a, b) (a - b < -EPSILON)
#define GREATER_THAN(a, b) (a - b > EPSILON)
#define GREATAER_EQUAL(a, b) (a - b > EPSILON || EQUALS(a, b))
#define LESS_EQUAL(a, b) (a - b < -EPSILON || EQUALS(a, b))
#define FLOAT_LOG_PRECISION 6
#define DEGREE2RAD(degree) degree * 0.0174532925f
#define PI 3.1415926f
#define INVALID_TEXTURE_ID 0
#define FRAC(val) val - (long)val
#define STEP(y, x) x >= y ? 1 : 0 
#define CAMERA_ROTATE_SPEED 0.25f
#define CAMERA_MOVE_SPEED 0.1f
#define CAMERA_ZOOM_SPEED 1.0f
#define R_CHANNEL 1
#define RG_CHANNEL 2
#define RGB_CHANNEL 3
#define RGBA_CHANNEL 4
#define TEXTURE_MAX_SIZE 4096
#define UNUSED(...) ((void)##__VA_ARGS__)
#define INST(type) Singleton<type>::get()

#define LEFT_HANDED
#define FAR_Z 1.0f
#define DEFAULT_STENCIL 0x00
#define DEFAULT_COLOR Color::encode_bgra(Color::BLACK)
#define DEFAULT_DEPTH_COLOR Color::encode_bgra(Color::WHITE)
#endif
