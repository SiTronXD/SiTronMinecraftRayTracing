#include "GamestateManager.h"

GamestateManager::GamestateManager(Gamestate* _initialState)
{
	setState(_initialState);
}

GamestateManager::~GamestateManager()
{
	delete currentState;

	std::cout << "Deleted current state" << std::endl;
}

void GamestateManager::setState(Gamestate* _newState)
{
	// Delete current state
	delete currentState;

	std::cout << "---Deleted current state and set new state" << std::endl;

	// Set new state
	currentState = _newState;
	currentState->init();
}

void GamestateManager::initState()
{
	currentState->init();
}

void GamestateManager::handleInputState(float dt)
{
	currentState->handleInput(dt);
}

void GamestateManager::updateState(float dt)
{
	currentState->update(dt);
}

void GamestateManager::drawState()
{
	currentState->draw();
}
