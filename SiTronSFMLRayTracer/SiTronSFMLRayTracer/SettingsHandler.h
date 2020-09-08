#pragma once
class SettingsHandler
{
private:
	int windowWidth;
	int windowHeight;

public:
	SettingsHandler();
	~SettingsHandler();

	const int GetWindowWidth() const;
	const int GetWindowHeight() const;
};

