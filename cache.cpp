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
    for(int i = 0; i < _height; i++) {
        vector<cacheBlock> row;
        for(int j = 0; j < _width; j++) {
            cacheBlock dirtyBlock = cacheBlock(_blockSize);
            row.push_back(dirtyBlock);
        }
        _cacheBlocks.push_back(row);
    }
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