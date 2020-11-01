#pragma once

namespace guarneri {
	template<typename T>
	class singleton {
	public:
		singleton() {}
		~singleton() {}
		
		static T& get() {
			static T _inst;
			return _inst;
		}

	private:
		singleton(const singleton&) = delete;
		void operator=(const singleton&) = delete;
	};
}