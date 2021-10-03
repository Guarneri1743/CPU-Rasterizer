#pragma once

template<typename T>
class Singleton
{
public:
	~Singleton();
	static T& get();

private:
	Singleton();
	Singleton(const Singleton&) = delete;
	void operator=(const Singleton&) = delete;
};

template<typename T>
Singleton<T>::Singleton()
{}

template<typename T>
Singleton<T>::~Singleton()
{}

template<typename T>
T& Singleton<T>::get()
{
	static T _inst;
	return _inst;
}

#define INST(type) Singleton<type>::get()
