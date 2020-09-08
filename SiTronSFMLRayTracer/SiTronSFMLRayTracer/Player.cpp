#include "Player.h"

Player::Player()
{
	position = sf::Glsl::Vec3(0.0f, 0.0f, -3.0f);

	direction = sf::Glsl::Vec2(0.0f, 0.0f);

	cameraForwardVector = sf::Glsl::Vec3(0.0f, 0.0f, 1.0f);
	cameraUpVector = sf::Glsl::Vec3(0.0f, 1.0f, 0.0f);
	cameraRightVector = sf::Glsl::Vec3(-1.0f, 0.0f, 0.0f);
}

Player::~Player()
{
}

void Player::Update(const float _dt)
{
	//this->SetDirection(direction.x + _dt, direction.y);
}

void Player::MovePosition(const float _forwardDir, const float _upDir, const float _rightDir, const float _dt)
{
	position += cameraRightVector * _rightDir * movementSpeed * _dt;
	position += cameraUpVector * _upDir * movementSpeed * _dt;
	position += cameraForwardVector * _forwardDir * movementSpeed * _dt;
}

void Player::RotateDirection(const float _horizontalAngleDir, const float _verticalAngleDir)
{
	direction.x += _horizontalAngleDir;
	direction.y += _verticalAngleDir;

	cameraForwardVector.x = sin(direction.x) * cos(direction.y);
	cameraForwardVector.y = sin(direction.y);
	cameraForwardVector.z = cos(direction.x) * cos(direction.y);


	cameraRightVector.x = cameraForwardVector.y * cameraUpVector.z - cameraForwardVector.z * cameraUpVector.y;
	cameraRightVector.y = cameraForwardVector.x * cameraUpVector.z - cameraForwardVector.z * cameraUpVector.x;
	cameraRightVector.z = cameraForwardVector.x * cameraUpVector.y - cameraForwardVector.y * cameraUpVector.x;
}


const sf::Glsl::Vec3& Player::GetPosition() const
{
	return position;
}

const sf::Glsl::Vec2& Player::GetDirection() const
{
	return direction;
}

const sf::Glsl::Vec3& Player::GetForwardVector() const
{
	return cameraForwardVector;
}
