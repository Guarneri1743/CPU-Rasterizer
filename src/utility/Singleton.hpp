#ifndef _SINGLETON_
#define _SINGLETON_

namespace Guarneri {
	template<typename T>
	class Singleton {
	public:
		Singleton() {}
		~Singleton() {}
		
		static T& get() {
			static T _inst;
			return _inst;
		}

	private:
		Singleton(const Singleton&) = delete;
		void operator=(const Singleton&) = delete;
	};
}
#endif