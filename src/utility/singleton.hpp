#pragma once

namespace guarneri {
	template<typename T>
	class singleton {
	public:
		static T& instance() {
			static T instance;
			return instance;
		}

	private:
		singleton() {}
		~singleton() {}
		singleton(const singleton&) {}
		void operator=(const singleton&) {}
	};
}