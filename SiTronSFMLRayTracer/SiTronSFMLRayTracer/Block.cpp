#include "Block.h"

Block::Block(sf::Vector3i _position, BlockType _blockType)
{
	this->position = _position;
	this->blockType = _blockType;
}

Block::~Block()
{

}

const sf::Vector3i& Block::GetPosition() const
{
	return position;
}

const int Block::GetBlockTypeIndex() const
{
	return (int) blockType;
}
