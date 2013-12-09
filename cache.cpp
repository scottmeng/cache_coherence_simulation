// cache.cpp
// Created by: Long Jinghan, Meng Kaizhi, Zhou Ruofan (National University of Singapore)
// Created on: 18 Nov 2013
// Last edited on: 18 Nov 2013
// All rights reserved

#include <vector>
#include "cache.h"

using namespace std;

cache::cache(int cacheSize, int blockSize, int associativity) {
    _cacheSize = cacheSize;
    _blockSize = blockSize;
    _associativity = associativity;

    // compute width and height
    _numOfBlocks = _cacheSize / _blockSize;
    _width = _associativity;
    _height = _numOfBlocks / _associativity;

    // initialize all cache blocks using dirty data
    for(int i = 0; i < _height; i++) {
        vector<cacheBlock> row;
        for(int j = 0; j < _width; j++) {
            cacheBlock dirtyBlock = cacheBlock(_blockSize);
            row.push_back(dirtyBlock);
        }
        _cacheBlocks.push_back(row);
    }
}

int cache::getColNum(unsigned addr){
	int tag = (addr / (_blockSize / 2)) / _height;
	int index = (addr / (_blockSize / 2)) % _height; 
	for (int i = 0; i< _width; i++){
		if (tag == _cacheBlocks[index][i].tag )
			return i;
	}
	return -1;
}

int cache::getRowNum(unsigned addr){
	return (addr / (_blockSize / 2)) % _height;
}


bool cache::isCacheHit(unsigned addr) {
    int index;
    int tag;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;

    for(int i = 0; i < (int)_cacheBlocks[index].size(); i++) {
        if(tag == _cacheBlocks[index][i].tag && _cacheBlocks[index][i].blockStatus != cacheBlock::INVALID) {
            return true;
        }
    }
    return false;
}
