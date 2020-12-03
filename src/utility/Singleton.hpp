#ifndef _SINGLETON_
#define _SINGLETON_

namespace Guarneri
{
	template<typename T>
	class Singleton
	{
	public:
		Singleton();
		~Singleton();
		static T& get();

	private:
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
}
#endif