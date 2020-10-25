#pragma once
#include <common.hpp>
#include <vertex.hpp>

namespace guarneri {
	struct mesh {
	public:
		mesh() {}

	public:
		std::vector<vertex> vertices;
	};
}