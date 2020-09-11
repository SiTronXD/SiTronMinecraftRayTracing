#pragma once

#include <iostream>
#include <map>
#include <string>

#include "Block.h"

class WorldHandler
{
private:
	std::map<std::string, Block*> blocks;
	std::map<int, std::string> indexToBlockPos;

	int currentBlockIndex;

	std::string FromPosToKey(sf::Vector3i position);

public:
	WorldHandler();
	~WorldHandler();

	void AddBlock(sf::Vector3i blockPos, BlockType blockType);

	std::vector<sf::Vector3i> GetBlocksToRender();
};