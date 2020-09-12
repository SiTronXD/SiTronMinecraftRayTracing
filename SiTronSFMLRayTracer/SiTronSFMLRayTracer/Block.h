#pragma once

#include <SFML/Graphics.hpp>

enum class BlockType
{
	Grass = 0,
	Stone = 1
};

class Block
{
private:
	sf::Vector3i position;

	BlockType blockType;

public:
	Block(sf::Vector3i _position, BlockType _blockType);
	~Block();

	const sf::Vector3i& GetPosition() const;

	const int GetBlockTypeIndex() const;

};