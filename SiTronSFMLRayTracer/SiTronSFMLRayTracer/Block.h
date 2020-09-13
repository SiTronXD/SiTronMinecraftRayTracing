#pragma once

#include <SFML/Graphics.hpp>

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

	static const int MAX_NUM_TEXTURE_RECTS = 12;
	static const sf::Glsl::Vec4 textureRects[MAX_NUM_TEXTURE_RECTS];
};