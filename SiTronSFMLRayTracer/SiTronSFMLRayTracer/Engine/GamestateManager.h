#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Gamestate.h"

class GamestateManager
{
private:
	Gamestate* currentState;

public:
	GamestateManager(Gamestate* _initialState);
	~GamestateManager();

	void setState(Gamestate* _newState);

	void initState();
	void handleInputState(float dt);
	void updateState(float dt);
	void drawState();
};