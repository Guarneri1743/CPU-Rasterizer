#pragma once
#include <ostream>
#include <sstream>

struct vec2f;
struct vec3f;
struct vec4f;
struct mat3x3;
struct mat4x4;
class Camera;
class Vertex;

static inline std::ostream& operator <<(std::ostream& stream, const vec2f& v);
static inline std::stringstream& operator <<(std::stringstream& stream, const vec2f& v);
static inline std::ostream& operator <<(std::ostream& stream, const vec3f& v);
static inline std::stringstream& operator <<(std::stringstream& stream, const vec3f& v);
static inline std::ostream& operator <<(std::ostream& stream, const vec4f& v);
static inline std::stringstream& operator <<(std::stringstream& stream, const vec4f& v);
static inline std::ostream& operator <<(std::ostream& stream, const mat3x3& v);
static inline std::stringstream& operator <<(std::stringstream& stream, const mat3x3& v);
static inline std::ostream& operator <<(std::ostream& stream, const mat4x4& v);
static inline std::stringstream& operator <<(std::stringstream& stream, const mat4x4& v);
static inline std::ostream& operator << (std::ostream& stream, const Camera& cam);
static inline std::stringstream& operator <<(std::stringstream& stream, const Camera& cam);
static inline std::ostream& operator << (std::ostream& stream, const Vertex& cam);
static inline std::stringstream& operator <<(std::stringstream& stream, const Vertex& cam);


