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

public:
	Player(WorldHandler& _worldHandler);
	~Player();

	void Update(const float _dt);

	void MovePosition(const float _forwardDir, const float _upDir, const float _rightDir, const float _dt);
	void RotateDirection(const float _horizontalAngleDir, const float _verticalAngleDir);

	void PlaceBlock();
	void RemoveBlock();

	const sf::Vector3f& GetPosition() const;
	const sf::Vector2f& GetDirection() const;
	const sf::Vector3f& GetForwardVector() const;
	const sf::Vector3f& GetUpVector() const;
	const sf::Vector3f& GetRightVector() const;
};