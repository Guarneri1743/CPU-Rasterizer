#ifndef _GRAPHICS_CMD_
#define _GRAPHICS_CMD_
#include <CPURasterizer.hpp>

namespace Guarneri
{
	class GraphicsCommand
	{
		virtual void execute() = 0;
	};

	class MsaaCommand : public GraphicsCommand
	{
	public:
		bool enable;
		uint32_t msaa_subsample_count;

	public:
		void execute(){}
	};
}

#endif