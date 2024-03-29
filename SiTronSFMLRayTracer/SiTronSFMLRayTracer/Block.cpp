#include "Block.h"

const sf::Glsl::Vec4 Block::TEXTURE_RECTS[] =
{
    sf::Glsl::Vec4(0, 0, 16, 16),    // Dirt block - up
    sf::Glsl::Vec4(16, 0, 16, 16),   // Dirt block - side
    sf::Glsl::Vec4(32, 0, 16, 16),   // Dirt block - down

    sf::Glsl::Vec4(0, 16, 16, 16),   // Stone block - up
    sf::Glsl::Vec4(0, 16, 16, 16),   // Stone block - side
    sf::Glsl::Vec4(0, 16, 16, 16),   // Stone block - down

    sf::Glsl::Vec4(16, 16, 16, 16),   // Redstone block - up
    sf::Glsl::Vec4(16, 16, 16, 16),   // Redstone block - side
    sf::Glsl::Vec4(16, 16, 16, 16),   // Redstone block - down

    sf::Glsl::Vec4(32, 16, 16, 16),  // Mirror block - up
    sf::Glsl::Vec4(32, 16, 16, 16),  // Mirror block - side
    sf::Glsl::Vec4(32, 16, 16, 16)   // Mirror block - down
};

const float Block::SPECULAR[] =
{
    0.0f,
    0.0f,
    0.0f,
    0.9f
};

const float Block::TRANSPARENCY[] =
{
    1.0f,
    1.0f,
    1.0f,
    1.0f
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
	return this->position;
}

int Block::getBlockTypeIndex() const
{
	return (int) this->blockType;
}

float Block::getBlockSpecular() const
{
    return SPECULAR[(int) this->blockType];
}

float Block::getBlockTransparency() const
{
    return TRANSPARENCY[(int) this->blockType];
}
