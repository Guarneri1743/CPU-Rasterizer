#pragma once

#define UNUSED(...) ((void)##__VA_ARGS__)

#ifdef WIN32
#undef min
#undef max
#undef GetObject
#endif

#ifndef FORCE_INLINE
#define RASTER_INLINE inline
#else
#ifdef _MSC_VER
#define RASTER_INLINE __forceinline
#else
#define RASTER_INLINE inline __attribute__((always_inline))
#endif
#endif