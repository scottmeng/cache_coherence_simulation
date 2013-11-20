// cache.cpp
// Created by: Long Jinghan, Meng Kaizhi, Zhou Ruofan (National University of Singapore)
// Created on: 18 Nov 2013
// Last edited on: 18 Nov 2013
// All rights reserved

#include <vector>
#include "cache.h"

cache::cache(int cacheSize, int blockSize, int associativity) {
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

bool cache::isReadHit(int addr, int cycle) {
    int index;
    int tag;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;

    for(int i = 0; i < (int)_cacheBlocks[index].size(); i++) {
        if(tag == _cacheBlocks[index][i].tag && _cacheBlocks[index][i].blockStatus != cacheBlock::INVALID) {
            _cacheBlocks[index][i].lru = cycle;
            return true;
        }
    }
    return false;
}
bool cache::isWriteHit(int addr, int cycle) {
	int index;
    int tag;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;

    for(int i = 0; i < (int)_cacheBlocks[index].size(); i++) {
        if(tag == _cacheBlocks[index][i].tag && _cacheBlocks[index][i].blockStatus != cacheBlock::INVALID) {
            _cacheBlocks[index][i].lru = cycle;
			_cacheBlocks[index][i].blockStatus = cacheBlock::EXECLUSIVE;
            return true;
        }
    }
    return false;
}
// Assume read miss, put the addr into the cache
void cache::readCache(int addr, int cycle) {
    int index;
    int tag;
    int minLRU = cycle + 1;
    int changeBlock = -1;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;

    for(int i = 0; i < _cacheBlocks[index].size(); i++) {
        if(_cacheBlocks[index][i].blockStatus == cacheBlock::INVALID) {
            changeBlock = i;
        } else {
            minLRU = (minLRU < _cacheBlocks[index][i].lru) ? minLRU : _cacheBlocks[index][i].lru;
            changeBlock = i;
        }
    }
    if(changeBlock > 0){
        _cacheBlocks[index][changeBlock].tag = tag;
        _cacheBlocks[index][changeBlock].lru = cycle;
        _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::EXCLUSIVE;
    }



}
void cache::writeCache(int addr) {

}