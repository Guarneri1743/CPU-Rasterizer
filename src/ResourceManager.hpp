#ifndef _RES_MGR_
#define _RES_MGR_
#include <string>
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


	template<typename T>
	void ResourceManager<T>::cache(const std::string& path, const std::shared_ptr<T>& res)
	{
		if (path2res.count(path) > 0)
		{
			std::cerr << "can not load a resource twice: " << path << std::endl;
			return;
		}
		path2res[path] = res;
	}

	template<typename T>
	bool ResourceManager<T>::get(const std::string& path, std::shared_ptr<T>& res)
	{
		if (path2res.count(path) > 0)
		{
			res = path2res[path].lock();
			return true;
		}
		return false;
	}

	template<typename T>
	void ResourceManager<T>::free(const std::string& path)
	{
		if (path2res.count(path) > 0)
		{
			path2res[path].reset();
			path2res.erase(path);
		}
	}

	template<typename T>
	void ResourceManager<T>::cache(const uint32_t& id, const std::shared_ptr<T>& res)
	{
		if (id2res.count(id) > 0)
		{
			std::cerr << "id duplicated: " << id << std::endl;
			return;
		}
		id2res[path] = res;
	}

	template<typename T>
	bool ResourceManager<T>::get(const uint32_t& id, std::shared_ptr<T>& res)
	{
		if (id2res.count(id) > 0)
		{
			res = id2res[id].lock();
			return true;
		}
		return false;
	}

	template<typename T>
	void ResourceManager<T>::free(const uint32_t& id)
	{
		if (id2res.count(id) > 0)
		{
			id2res[id].reset();
			id2res.erase(id);
		}
	}

}
#endif