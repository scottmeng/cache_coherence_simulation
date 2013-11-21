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

bool cache::uniIsReadHit(unsigned addr, int cycle){
	int index;
	int tag;
	index = (addr / (_blockSize / 2)) % _height;
	tag = (addr / (_blockSize / 2)) / _height;

	for(int i = 0; i < _width; i++) {
		if(tag == _cacheBlocks[index][i].tag && _cacheBlocks[index][i].blockStatus != cacheBlock::INVALID) {
			_cacheBlocks[index][i].lru = cycle;
			return true;
		}
	}
	return false;
}

bool cache::uniIsWriteHit(unsigned addr, int cycle){
	int index;
    int tag;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;

    for(int i = 0; i < _width; i++) {
        if(tag == _cacheBlocks[index][i].tag && _cacheBlocks[index][i].blockStatus != cacheBlock::INVALID) {
            _cacheBlocks[index][i].lru = cycle;
			_cacheBlocks[index][i].blockStatus = cacheBlock::MODIFIED;
            return true;
        }
    }
    return false;
}

void cache::uniReadCache(unsigned addr, int cycle){
	int index;
    int tag;
    int minLRU = cycle + 1;
    int changeBlock = -1;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;

    for(int i = 0; i < _width; i++) {
        if(_cacheBlocks[index][i].blockStatus == cacheBlock::INVALID) {
            changeBlock = i;
			break;
        } else {
			if(minLRU > _cacheBlocks[index][i].lru) {
				minLRU = _cacheBlocks[index][i].lru;
				changeBlock = i;
			} 
		}
    }
    if(changeBlock > -1){
        _cacheBlocks[index][changeBlock].tag = tag;
        _cacheBlocks[index][changeBlock].lru = cycle;
        _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::EXCLUSIVE;
    }
}

void cache::uniWriteCache(unsigned addr, int cycle){
	int index;
    int tag;
    int minLRU = cycle + 1;
    int changeBlock = -1;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;

    for(int i = 0; i < _width; i++) {
        if(_cacheBlocks[index][i].blockStatus == cacheBlock::INVALID) {
            changeBlock = i;
			break;
        } else {
			if(minLRU > _cacheBlocks[index][i].lru) {
				minLRU = _cacheBlocks[index][i].lru;
				changeBlock = i;
			}   
        }
    }
    if(changeBlock > -1){
        _cacheBlocks[index][changeBlock].tag = tag;
        _cacheBlocks[index][changeBlock].lru = cycle;
        _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::EXCLUSIVE;
    }
	return;
}

bool cache::isReadHit(unsigned addr, int cycle) {
    return false;
}
bool cache::isWriteHit(unsigned addr, int cycle) {
    return false;
}
// Assume read miss, put the addr into the cache
void cache::readCache(unsigned addr, int cycle) {
    return ;
}
void cache::writeCache(unsigned addr, int cycle) {
    return;
}

int cache::isCacheHit(unsigned addr) {
    int index;
    int tag;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;

    for(int i = 0; i < (int)_cacheBlocks[index].size(); i++) {
        if(tag == _cacheBlocks[index][i].tag && _cacheBlocks[index][i].blockStatus != cacheBlock::INVALID) {
            return i;
        }
    }
    return -1;
}