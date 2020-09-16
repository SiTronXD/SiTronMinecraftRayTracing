#include "Block.h"

const sf::Glsl::Vec4 Block::textureRects[] = 
{
    sf::Glsl::Vec4(0, 0, 16, 16),    // Dirt block - up
    sf::Glsl::Vec4(48, 0, 16, 16),   // Dirt block - side
    sf::Glsl::Vec4(32, 0, 16, 16),   // Dirt block - down

    sf::Glsl::Vec4(16, 0, 16, 16),   // Stone block - up
    sf::Glsl::Vec4(16, 0, 16, 16),   // Stone block - side
    sf::Glsl::Vec4(16, 0, 16, 16),   // Stone block - down

    sf::Glsl::Vec4(16 * 10, 16, 16, 16),   // Redstone block - up
    sf::Glsl::Vec4(16 * 10, 16, 16, 16),   // Redstone block - side
    sf::Glsl::Vec4(16 * 10, 16, 16, 16),   // Redstone block - down

    sf::Glsl::Vec4(16 * 8, 16 * 7, 16, 16),  // Mirror block - up
    sf::Glsl::Vec4(16 * 8, 16 * 7, 16, 16),  // Mirror block - side
    sf::Glsl::Vec4(16 * 8, 16 * 7, 16, 16)   // Mirror block - down
};

Block::Block(sf::Vector3i _position, BlockType _blockType)
{
	this->position = _position;
	this->blockType = _blockType;
}

Block::~Block()
{

}

const sf::Vector3i& Block::getPosition() const
{
	return position;
}

const int Block::getBlockTypeIndex() const
{
	return (int) blockType;
}