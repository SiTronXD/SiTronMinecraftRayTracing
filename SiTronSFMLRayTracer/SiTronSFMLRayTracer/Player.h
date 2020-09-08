#pragma once

#include <SFML/Graphics.hpp>

#include "SMath.h"

class Player
{
private:
	sf::Vector3f position;
	sf::Vector2f direction; // (horizontal angle in radians, vertical angle in radians)

	sf::Vector3f cameraForwardVector;
	sf::Vector3f cameraUpVector;
	sf::Vector3f cameraRightVector;

	const float movementSpeed = 5.0f;

public:
	Player();
	~Player();

	void Update(const float _dt);

	void MovePosition(const float _forwardDir, const float _upDir, const float _rightDir, const float _dt);
	void RotateDirection(const float _horizontalAngleDir, const float _verticalAngleDir);

	const sf::Vector3f& GetPosition() const;
	const sf::Vector2f& GetDirection() const;
	const sf::Vector3f& GetForwardVector() const;
};