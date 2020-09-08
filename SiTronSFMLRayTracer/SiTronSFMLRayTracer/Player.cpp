#include "Player.h"

Player::Player()
{
	position = sf::Vector3f(0.0f, 0.0f, -3.0f);

	direction = sf::Vector2f(0.0f, 0.0f);

	cameraForwardVector = sf::Vector3f(0.0f, 0.0f, 1.0f);
	cameraUpVector = sf::Vector3f(0.0f, 1.0f, 0.0f);
	cameraRightVector = sf::Vector3f(-1.0f, 0.0f, 0.0f);
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
	// Ignore the y-component and normalize the flat vector
	sf::Vector3f tempForwardVector = cameraForwardVector;
	tempForwardVector.y = 0.0f;
	tempForwardVector /= sqrt(tempForwardVector.x * tempForwardVector.x + tempForwardVector.z * tempForwardVector.z);

	position += cameraRightVector * _rightDir * movementSpeed * _dt;
	position += cameraUpVector * _upDir * movementSpeed * _dt;
	position += tempForwardVector * _forwardDir * movementSpeed * _dt;
}

void Player::RotateDirection(const float _horizontalAngleDir, const float _verticalAngleDir)
{
	direction.x += _horizontalAngleDir;
	direction.y += _verticalAngleDir;

	// Clamp vertical angle
	direction.y = SMath::Clamp(direction.y, -3.1415 * 0.45f, 3.1415 * 0.45f);

	// Calculate forward vector
	cameraForwardVector.x = sin(direction.x) * cos(direction.y);
	cameraForwardVector.y = sin(direction.y);
	cameraForwardVector.z = cos(direction.x) * cos(direction.y);

	// Calculate right vector using the cross product between forward and up.
	cameraRightVector.x = cameraForwardVector.y * cameraUpVector.z - cameraForwardVector.z * cameraUpVector.y;
	cameraRightVector.y = cameraForwardVector.x * cameraUpVector.z - cameraForwardVector.z * cameraUpVector.x;
	cameraRightVector.z = cameraForwardVector.x * cameraUpVector.y - cameraForwardVector.y * cameraUpVector.x;
}


const sf::Vector3f& Player::GetPosition() const
{
	return position;
}

const sf::Vector2f& Player::GetDirection() const
{
	return direction;
}

const sf::Vector3f& Player::GetForwardVector() const
{
	return cameraForwardVector;
}
