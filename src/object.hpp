#pragma once
#include <guarneri.hpp>

namespace guarneri {
	class object {
	public:
		object() { id = ALLOC_ID(); }
		virtual ~object() { FREE_ID(id); }

	protected:
		uint32_t id;

	public:
		uint32_t get_id() {
			return id;
		}

		virtual std::string str() const = 0;
	};

	static std::ostream& operator << (std::ostream& stream, const object& obj) {
		stream << obj.str();
		return stream;
	}

	static std::stringstream& operator << (std::stringstream& stream, const object& obj) {
		stream << obj.str();
		return stream;
	}
}
