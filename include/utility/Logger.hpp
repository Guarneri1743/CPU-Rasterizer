#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <queue>

#undef LOG
#undef WARN
#undef ERROR
#undef FATAL

constexpr bool kLogToCommandLine = true;
constexpr char* kSpecifier = "{}";
constexpr int kSpecifierLength = 2;
constexpr int kConsoleLogMaxNum = 100;

class Logger
{
public:
	enum class Severity
	{
		kLog = 0,
		kWarning = 1,
		kError = 2,
		kFatalError = 3
	};

	static void log(const Severity& severity, std::string str)
	{
		write(terminal_buffer, str);
		print_and_flush(terminal_buffer, severity);
	}

	template<typename Arg, typename... Args>
	static void log(const Severity& severity, std::string fmt, Arg arg, Args... args)
	{
		write(terminal_buffer, fmt, arg, args...);
		print_and_flush(terminal_buffer, severity);
	}

	static void log_console(const Severity& severity, std::string str)
	{
		write(console_buffer, str);
		std::string line = read_and_flush(console_buffer, severity);
		console_log_buffer.emplace_back(line);
		try_resize_console_logs();

		if (kLogToCommandLine)
		{
			write(terminal_buffer, str);
			print_and_flush(terminal_buffer, severity);
		}
	}

	template<typename Arg, typename... Args>
	static void log_console(const Severity& severity, std::string fmt, Arg arg, Args... args)
	{
		write(console_buffer, fmt, arg, args...);
		std::string line = read_and_flush(console_buffer, severity);
		console_log_buffer.emplace_back(line);
		try_resize_console_logs();

		if (kLogToCommandLine)
		{
			write(terminal_buffer, fmt, arg, args...);
			print_and_flush(terminal_buffer, severity);
		}
	}

	static std::vector<std::string> get_console_logs()
	{
		return console_log_buffer;
	}

	static void print_console_logs()
	{
		for (auto& log : console_log_buffer)
		{
			std::cout << log << std::endl;
		}
	}

private:
	template<typename Arg>
	static void write(std::stringstream& buffer, std::string fmt, Arg arg)
	{
		size_t pos = 0;
		std::string token;
		if ((pos = fmt.find(kSpecifier)) != std::string::npos)
		{
			token = fmt.substr(0, pos);
			buffer << token;
			buffer << arg;
		}
	}

	template<typename Arg, typename... Args>
	static void write(std::stringstream& buffer, std::string fmt, Arg arg, Args... args)
	{
		size_t pos = 0;
		std::string token;
		if ((pos = fmt.find(kSpecifier)) != std::string::npos)
		{
			token = fmt.substr(0, pos);
			buffer << token;
			buffer << arg;
			fmt.erase(0, pos + kSpecifierLength);
			write(buffer, fmt, args...);
		}
	}

	static void write(std::stringstream& buffer, const std::string& fmt)
	{
		buffer << fmt;
	}

	static void try_resize_console_logs()
	{
		if (console_log_buffer.size() > kConsoleLogMaxNum)
		{
			std::swap(console_log_buffer[0], console_log_buffer[console_log_buffer.size() - 1]);
			console_log_buffer.pop_back();
		}
	}

	static std::string  get_prefix(const Severity& severity)
	{
		std::string prefix;
		switch (severity)
		{
		case Severity::kLog:
			prefix = "[Log]";
			break;
		case Severity::kWarning:
			prefix = "[Warning]";
			break;
		case Severity::kError:
			prefix = "[Error]";
			break;
		case Severity::kFatalError:
			prefix = "[FatalError]";
			break;
		}
		return prefix;
	}

	static std::string read_and_flush(std::stringstream& buffer, const Severity& severity)
	{
		std::string ret = get_prefix(severity) + buffer.str();
		buffer.str(std::string());
		return ret;
	}

	static void print_and_flush(std::stringstream& buffer, const Severity& severity)
	{
		std::cout << get_prefix(severity) << buffer.str() << std::endl;
		buffer.str(std::string());
	}

	static std::vector<std::string> console_log_buffer;
	static std::stringstream console_buffer;
	static std::stringstream terminal_buffer;
};

//=========================== Log Marcos ======================================
#define LOG(...) Logger::log_console(Logger::Severity::kLog, __VA_ARGS__)
#define WARN(...) Logger::log_console(Logger::Severity::kWarning, __VA_ARGS__)
#define ERROR(...) Logger::log_console(Logger::Severity::kError, __VA_ARGS__)
#define FATAL(...) Logger::log_console(Logger::Severity::kFatalError, __VA_ARGS__)
//=============================================================================

