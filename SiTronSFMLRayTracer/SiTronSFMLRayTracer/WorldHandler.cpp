#include "WorldHandler.h"

std::string WorldHandler::FromPosToKey(sf::Vector3i position)
{
	std::string key = 
		std::to_string(position.x) + "x" +
		std::to_string(position.y) + "x" +
		std::to_string(position.z);

	return key;
}

WorldHandler::WorldHandler()
{

}

WorldHandler::~WorldHandler()
{
	// Delete all blocks
	for (auto it = blocks.begin(); it != blocks.end(); it++)
	{
		delete it->second;
	}
}

void WorldHandler::AddBlock(sf::Vector3i blockPos, BlockType blockType)
{
	// Remove current block
	RemoveBlock(blockPos);


	std::string blocksKey = FromPosToKey(blockPos);
	
	blocks[blocksKey] = new Block(blockPos, blockType);
}

void WorldHandler::RemoveBlock(sf::Vector3i blockPos)
{
	std::string blocksKey = FromPosToKey(blockPos);

	delete blocks[blocksKey];
	blocks.erase(blocksKey);
}

std::vector<sf::Vector3i> WorldHandler::GetBlocksToRender()
{
	std::vector<sf::Vector3i> tempBlocksToRender;
	tempBlocksToRender.reserve(blocks.size());

	std::for_each(blocks.begin(), blocks.end(), [&](std::pair<const std::string, Block*> & element)
	{
		tempBlocksToRender.push_back(element.second->GetPosition());
	});

	return tempBlocksToRender;
}