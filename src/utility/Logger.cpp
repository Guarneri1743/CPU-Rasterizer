#include "Logger.hpp"

namespace Guarneri
{
	std::vector<std::string> Logger::console_log_buffer;
	std::stringstream Logger::console_buffer;
	std::stringstream Logger::terminal_buffer;
}