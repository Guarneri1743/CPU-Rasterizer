#ifndef _COMMON_UTILITY_
#define _COMMON_UTILITY_
#include <filesystem>
#include "Marcos.h"
#include "Matrix3x3.hpp"
#include "matrix4x4.hpp"

namespace Guarneri {
	static std::string replace(std::string str, std::string pattern, std::string content) {
		while (str.find(pattern) != std::string::npos) {
			str.replace(str.find(pattern), 1, content);
		}
		return str;
	}

	static std::string cur_path() {
		std::string ret = std::filesystem::current_path().string();
#if _DEBUG
		ret += "/bin/Debug";
#else
		ret += "/bin/Release";
#endif
		return replace(ret, "\\", "/");
	}

	static std::string res_path() {
		return cur_path() + "/res";
	}

	static Matrix3x3 mat4tomat3(const Matrix4x4& mat)
	{
		Matrix3x3 mat3;
		mat3.m00 = mat.at(0, 0);
		mat3.m01 = mat.at(0, 1);
		mat3.m02 = mat.at(0, 2);
		mat3.m10 = mat.at(1, 0);
		mat3.m11 = mat.at(1, 1);
		mat3.m12 = mat.at(1, 2);
		mat3.m20 = mat.at(2, 0);
		mat3.m21 = mat.at(2, 1);
		mat3.m22 = mat.at(2, 2);
		return mat3;
	}
}

#endif