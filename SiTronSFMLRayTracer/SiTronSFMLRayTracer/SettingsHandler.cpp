#include "SettingsHandler.h"

SettingsHandler::SettingsHandler()
	: windowWidth(1280), windowHeight(720)
{

}

SettingsHandler::~SettingsHandler()
{
}

const int SettingsHandler::GetWindowWidth() const
{
	return windowWidth;
}

const int SettingsHandler::GetWindowHeight() const
{
	return windowHeight;
}
