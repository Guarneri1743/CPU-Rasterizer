#pragma once
#include <stdint.h>

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
		uint8_t msaa_subsample_count;

	public:
		void execute(){}
	};
}