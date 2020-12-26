#ifndef _COMMON_UTILITY_
#define _COMMON_UTILITY_
#include <CPURasterizer.hpp>

namespace Guarneri {
	unsigned char CLAMP(unsigned char x, unsigned char min, unsigned char max) { return (x < min) ? min : ((x > max) ? max : x); }
	uint32_t CLAMP_UINT(uint32_t x, uint32_t min, uint32_t max) { return (x < min) ? min : ((x > max) ? max : x); }
	int CLAMP_INT(int x, int min, int max) { return (x < min) ? min : ((x > max) ? max : x); }
	float CLAMP_FLT(float x, float min, float max) { return (x < min) ? min : ((x > max) ? max : x); }

	static std::string replace(std::string str, std::string pattern, std::string content) {
		while (str.find(pattern) != std::string::npos) {
			str.replace(str.find(pattern), 1, content);
		}
		return str;
	}

	static std::string cur_path() {
		std::string ret = FS::current_path().string();
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

	IdAllocator idalloc(INVALID_ID + 1, MAX_ID);
	#define ALLOC_ID() idalloc.alloc();
	#define FREE_ID(id) idalloc.free(id);
}

#endif