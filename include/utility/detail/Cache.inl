// simple cache based on weak_ptr
template<typename T>
Cache<T>::~Cache()
{
	destructed = true;
}

template<typename T>
inline Cache<T>::Cache()
{}

template<typename T>
void Cache<T>::put(const std::string& path, const std::shared_ptr<T>& res)
{
	if (destructed) return;
	if (path2res.count(path) > 0)
	{
		std::cerr << "can not load a resource twice: " << path << std::endl;
		return;
	}
	path2res[path] = res;
}

template<typename T>
bool Cache<T>::get(const std::string& path, std::shared_ptr<T>& res)
{
	if (destructed) return false;
	if (path2res.count(path) > 0)
	{
		res = path2res[path].lock();
		return true;
	}
	return false;
}

template<typename T>
void Cache<T>::free(const std::string& path)
{
	if (destructed) return;
	if (path2res.count(path) > 0)
	{
		path2res[path].reset();
		path2res.erase(path);
	}
}

template<typename T>
void Cache<T>::put(const uint32_t& id, const std::shared_ptr<T>& res)
{
	if (destructed) return;
	if (id2res.count(id) > 0)
	{
		std::cerr << "id duplicated: " << id << std::endl;
		return;
	}
	id2res[path] = res;
}

template<typename T>
bool Cache<T>::get(const uint32_t& id, std::shared_ptr<T>& res)
{
	if (destructed) return false;
	if (id2res.count(id) > 0)
	{
		res = id2res[id].lock();
		return true;
	}
	return false;
}

template<typename T>
void Cache<T>::free(const uint32_t& id)
{
	if (destructed) return;
	if (id2res.count(id) > 0)
	{
		id2res[id].reset();
		id2res.erase(id);
	}
}