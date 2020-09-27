#pragma once

#include <SFML/Graphics.hpp>
#include "Log.h"

enum class BlockType
{
	Grass = 0,
	Stone = 1,
	RedstoneBlock = 2,
	Mirror = 3
};

class Block
{
private:
	sf::Vector3i position;

	BlockType blockType;

public:
	Block(sf::Vector3i _position, BlockType _blockType);
	~Block();

	const sf::Vector3i& getPosition() const;

	const int getBlockTypeIndex() const;
	float getBlockSpecular();

	static const int NUM_BLOCK_TYPES = 4;
	static const int MAX_NUM_TEXTURE_RECTS = 12;

	static const sf::Glsl::Vec4 TEXTURE_RECTS[MAX_NUM_TEXTURE_RECTS];
	static const float SPECULAR[NUM_BLOCK_TYPES];
};