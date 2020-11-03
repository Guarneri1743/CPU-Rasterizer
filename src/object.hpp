#pragma once
#include <guarneri.hpp>

namespace guarneri {
	class object {
	public:
		object() {}
		virtual ~object() {}

	private:
		uint32_t id;

	public:
		uint32_t get_id() {
			return id;
		}
	};
}
