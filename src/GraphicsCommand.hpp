#ifndef _GRAPHICS_CMD_
#define _GRAPHICS_CMD_
#include <Guarneri.hpp>

namespace Guarneri {
	class GraphicsCommand {
	public:
		virtual void execute() = 0;
	};
}

#endif