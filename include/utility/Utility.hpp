#ifndef _COMMON_UTILITY_
#define _COMMON_UTILITY_
#include <filesystem>
#include <string>
#include "Marcos.h"

#pragma warning( disable : 4505 )

namespace Guarneri {
	static std::string replace(std::string str, std::string pattern, std::string content) {
		while (str.find(pattern) != std::string::npos) {
			str.replace(str.find(pattern), 1, content);
		}
		return str;
	}

	static std::string cur_path(){
		std::string ret = std::filesystem::current_path().string();
#if _DEBUG
		ret += "/bin/Debug";
#else
		ret += "/bin/Release";
#endif
		return replace(ret, "\\", "/");
	}

#define APP_PATH cur_path()
#define RES_PATH (APP_PATH + "/res/raw")
#define ASSETS_PATH (APP_PATH + "/res/assets")
}

#endif