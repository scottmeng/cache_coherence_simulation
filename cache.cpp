// cache.cpp
// Created by: Long Jinghan, Meng Kaizhi, Zhou Ruofan (National University of Singapore)
// Created on: 18 Nov 2013
// Last edited on: 18 Nov 2013
// All rights reserved

#include <vector>
#include "cache.h"
#include "cacheBlock.h"

cache :: cache(int cacheSize, int blockSize, int associativity) {
	_cacheSize = cacheSize;
	_blockSize = blockSize;
	_associativity = associativity;

	// compute width and height
	_numOfBlocks = _cacheSize / _blockSize;
	_width = _associativity;
	_height = _numOfBlocks / _associativity;

	// initialize all cache blocks using dirty data
	for(int i = 0; i < _width; i++) {
		vector<cacheBlock> column;
		for(int j = 0; j < _height; j++) {
			cacheBlock dirtyBlock = cacheBlock(_blockSize);
			column.push_back(dirtyBlock);
		}
		_cacheBlocks.push_back(column);
	}
}