#pragma once
#include <guarneri.hpp>

namespace guarneri {
	class object {
	public:
		object() { id = ALLOC_ID(); }
		virtual ~object() { FREE_ID(id); }

	private:
		uint32_t id;

	public:
		uint32_t get_id() {
			return id;
		}
	};
}
