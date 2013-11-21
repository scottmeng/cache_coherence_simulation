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

	blocked = false;
}

bool mesiCache::isReadHit(unsigned addr, int cycle){
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

bool mesiCache::isWriteHit(unsigned addr, int cycle){
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

void mesiCache::readCache(unsigned addr, int cycle){
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

void mesiCache::writeCache(unsigned addr, int cycle){
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

void mesiCache::selfChangeState(unsigned addr, int instrType, bool isHit,int cycle){
	mesiCache cacheBlks;
	int row = cacheBlks.getRowNum(addr);
	int col = cacheBlks.getColNum(addr);
	cacheBlock cache = _cacheBlocks[row][col];
	switch(cache.blockStatus){
	case cacheBlock::INVALID:
		break;
	case cacheBlock::SHARED:
		break;
	case cacheBlock::MODIFIED:
		break;
	case cacheBlock::EXCLUSIVE:
		break;
	}
	return;
}

void otherChangeStare(unsigned addr, int instrType, bool isHit,int cycle){

	return;
}

bool mesiCache::isCacheModified(unsigned addr){
	int tag = (addr / (_blockSize / 2)) / _height;
	int index = (addr / (_blockSize / 2)) % _height; 
	for (int i = 0; i< _width; i++){
		if(tag == _cacheBlocks[index][i].tag&&_cacheBlocks[index][i].blockStatus == cacheBlock::MODIFIED)
			return true;
	}
	return false;
}

transaction mesiCache::generateTransaction(unsigned addr, int instrType){

}
