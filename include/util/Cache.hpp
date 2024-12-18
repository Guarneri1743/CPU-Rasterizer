#pragma once
#include <string>
#include <memory>
#include <unordered_map>

// simple cache based on weak_ptr
template<typename T>
class Cache
{
private:
	std::unordered_map<std::string, std::weak_ptr<T>> path2res;
	std::unordered_map<uint32_t, std::weak_ptr<T>> id2res;

public:
	Cache();
	~Cache<T>();
	void put(const std::string& path, const std::shared_ptr<T>& res);
	void put(uint32_t id, const std::shared_ptr<T>& res);
	void free(const std::string& path);
	void free(uint32_t id);
	bool get(const std::string& path, std::shared_ptr<T>& res);
	bool get(uint32_t id, std::shared_ptr<T>& res);

private:
	bool destructed;
};

#include "detail/Cache.inl"