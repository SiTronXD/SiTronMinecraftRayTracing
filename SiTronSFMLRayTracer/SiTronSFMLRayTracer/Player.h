#pragma once

#include <SFML/Graphics.hpp>

class Player
{
private:
	sf::Glsl::Vec3 position;
	sf::Glsl::Vec2 direction; // (vorizontal angle in radians, vertical angle in radians)

	sf::Glsl::Vec3 cameraForwardVector;
	sf::Glsl::Vec3 cameraUpVector;
	sf::Glsl::Vec3 cameraRightVector;

	const float movementSpeed = 5.0f;

public:
	Player();
	~Player();

	void Update(const float _dt);

	void MovePosition(const float _forwardDir, const float _upDir, const float _rightDir, const float _dt);
	void RotateDirection(const float _horizontalAngleDir, const float _verticalAngleDir);

	const sf::Glsl::Vec3& GetPosition() const;
	const sf::Glsl::Vec2& GetDirection() const;
	const sf::Glsl::Vec3& GetForwardVector() const;
};