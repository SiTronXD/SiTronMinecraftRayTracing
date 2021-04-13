#pragma once

#include <iostream>

class Log
{
public:
	static void print(const std::string& text);
	static void pauseConsole();
};