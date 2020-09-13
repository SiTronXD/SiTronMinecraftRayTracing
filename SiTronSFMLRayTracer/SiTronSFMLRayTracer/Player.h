#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "WorldHandler.h"
#include "Ray.h"
#include "SMath.h"

class Player
{
private:
	WorldHandler& worldHandler;

	sf::Vector3f position;
	sf::Vector2f direction; // (horizontal angle in radians, vertical angle in radians)

	sf::Vector3f worldUpVector;
	sf::Vector3f cameraForwardVector;
	sf::Vector3f cameraUpVector;
	sf::Vector3f cameraRightVector;

	const float movementSpeed = 5.0f;

	BlockType currentPlaceBlockType;

public:
	Player(WorldHandler& _worldHandler);
	~Player();

	void movePosition(const float _forwardDir, const float _upDir, const float _rightDir, const float _dt);
	void rotateDirection(const float _horizontalAngleDir, const float _verticalAngleDir);

	void setCurrentPlaceBlockType(const BlockType _newBlockType);

	void placeBlock();
	void removeBlock();

	const sf::Vector3f& getPosition() const;
	const sf::Vector2f& getDirection() const;
	const sf::Vector3f& getForwardVector() const;
	const sf::Vector3f& getUpVector() const;
	const sf::Vector3f& getRightVector() const;
};