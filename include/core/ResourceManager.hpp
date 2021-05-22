#ifndef _RES_MGR_
#define _RES_MGR_
#include <string>
#include <memory>
#include <unordered_map>

// simple cache based on weak_ptr
namespace Guarneri
{
	template<typename T>
	class ResourceManager
	{
	private:
		std::unordered_map<std::string, std::weak_ptr<T>> path2res;
		std::unordered_map<uint32_t, std::weak_ptr<T>> id2res;

	public:
		void cache(const std::string& path, const std::shared_ptr<T>& res);
		void cache(const uint32_t& id, const std::shared_ptr<T>& res);
		void free(const std::string& path);
		void free(const uint32_t& id);
		bool get(const std::string& path, std::shared_ptr<T>& res);
		bool get(const uint32_t& id, std::shared_ptr<T>& res);
	};
}

#include "detail/ResourceManager.inl"
#endif