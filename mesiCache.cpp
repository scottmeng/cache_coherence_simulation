#include <vector>
#include "mesiCache.h"

using namespace std;

mesiCache::mesiCache(int cacheSize, int blockSize, int associativity) {
    _cacheSize = cacheSize;
    _blockSize = blockSize;
    _associativity = associativity;

    // compute width and height
    _numOfBlocks = _cacheSize / _blockSize;
    _width = _associativity;
    _height = _numOfBlocks / _associativity;

    // initialize all cache blocks using dirty data
    for(int i = 0; i < _height ; i++) {
        vector<cacheBlock> row;
        for(int j = 0; j <_width; j++) {
            cacheBlock dirtyBlock = cacheBlock(_blockSize);
            row.push_back(dirtyBlock);
        }
        _cacheBlocks.push_back(row);
    }

}

bool mesiCache::isReadHit(unsigned addr, int cycle){
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

bool mesiCache::isWriteHit(unsigned addr, int cycle){
	int index;
    int tag;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;

    for(int i = 0; i < (int)_cacheBlocks[index].size(); i++) {
        if(tag == _cacheBlocks[index][i].tag && _cacheBlocks[index][i].blockStatus != cacheBlock::INVALID) {
            _cacheBlocks[index][i].lru = cycle;
			_cacheBlocks[index][i].blockStatus = cacheBlock::MODIFIED;
            return true;
        }
    }
    return false;
}

void mesiCache::readCache(unsigned addr, int cycle){
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

void mesiCache::writeCache(unsigned addr, int cycle){
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
	return;
}