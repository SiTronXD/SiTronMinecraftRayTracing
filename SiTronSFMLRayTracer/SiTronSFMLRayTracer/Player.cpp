#include "Player.h"

Player::Player(WorldHandler& _worldHandler)
	: worldHandler(_worldHandler)
{
	position = sf::Vector3f(0.0f, 0.0f, -3.0f);

	direction = sf::Vector2f(0.0f, 0.0f);

	cameraForwardVector = sf::Vector3f(0.0f, 0.0f, 1.0f);
	worldUpVector = sf::Vector3f(0.0f, 1.0f, 0.0f);
	cameraRightVector = sf::Vector3f(-1.0f, 0.0f, 0.0f);
}

Player::~Player()
{
}

void Player::Update(const float _dt)
{
	
}

void Player::MovePosition(const float _forwardDir, const float _upDir, const float _rightDir, const float _dt)
{
	// Ignore the y-component and normalize the flat vector
	//sf::Vector3f tempForwardVector = cameraForwardVector;
	//tempForwardVector.y = 0.0f;
	//tempForwardVector /= sqrt(tempForwardVector.x * tempForwardVector.x + tempForwardVector.z * tempForwardVector.z);

	position += cameraRightVector * _rightDir * movementSpeed * _dt;
	position += worldUpVector * _upDir * movementSpeed * _dt;
	position += cameraForwardVector * _forwardDir * movementSpeed * _dt;
}

void Player::RotateDirection(const float _horizontalAngleDir, const float _verticalAngleDir)
{
	direction.x += _horizontalAngleDir;
	direction.y += _verticalAngleDir;

	// Clamp vertical angle
	direction.y = SMath::clamp(direction.y, -3.1415 * 0.45f, 3.1415 * 0.45f);

	// Calculate forward vector
	cameraForwardVector.x = sin(direction.x) * cos(direction.y);
	cameraForwardVector.y = sin(direction.y);
	cameraForwardVector.z = cos(direction.x) * cos(direction.y);

	// Calculate right vector
	SMath::vectorCross(cameraForwardVector, worldUpVector, cameraRightVector);
	SMath::vectorNormalize(cameraRightVector); // Forward and world up are probably not orthogonal

	// Calculate up vector
	SMath::vectorCross(cameraRightVector, cameraForwardVector, cameraUpVector);
}

void Player::PlaceBlock()
{
	// Create ray
	Ray ray(position, cameraForwardVector);
	Hit bestHit;

	// Let ray intersect with the blocks
	std::vector<sf::Vector3i> blocksToCheck = worldHandler.GetBlocksToRender();
	for (int i = 0; i < blocksToCheck.size(); i++)
	{
		Hit tempHit = ray.GetClosestBoxHit(blocksToCheck[i]);

		// Find the closest hit
		bestHit = Hit::getClosestHit(bestHit, tempHit);
	}

	// Check if the ray even hit something
	if (bestHit.hitSomething())
	{
		sf::Vector3f newBlockPos = bestHit.point + bestHit.normal * 0.5f;
		newBlockPos.x = round(newBlockPos.x);
		newBlockPos.y = round(newBlockPos.y);
		newBlockPos.z = round(newBlockPos.z);

		worldHandler.AddBlock((sf::Vector3i) newBlockPos, BlockType::Grass);
	}
}

void Player::RemoveBlock()
{
	// Create ray
	Ray ray(position, cameraForwardVector);
	Hit bestHit;

	// Let ray intersect with the blocks
	std::vector<sf::Vector3i> blocksToCheck = worldHandler.GetBlocksToRender();
	for (int i = 0; i < blocksToCheck.size(); i++)
	{
		Hit tempHit = ray.GetClosestBoxHit(blocksToCheck[i]);

		// Find the closest hit
		bestHit = Hit::getClosestHit(bestHit, tempHit);
	}

	// Check if the ray even hit something
	if (bestHit.hitSomething())
	{
		sf::Vector3f newBlockPos = bestHit.point - bestHit.normal * 0.5f;
		newBlockPos.x = round(newBlockPos.x);
		newBlockPos.y = round(newBlockPos.y);
		newBlockPos.z = round(newBlockPos.z);

		worldHandler.RemoveBlock((sf::Vector3i) newBlockPos);
	}
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

const sf::Vector3f& Player::GetUpVector() const
{
	return cameraUpVector;
}

const sf::Vector3f& Player::GetRightVector() const
{
	return cameraRightVector;
}
