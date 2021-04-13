#include "Log.h"

void Log::print(const std::string& text)
{
	std::cout << text << std::endl;
}

void Log::pauseConsole()
{
	(void)getchar();
}