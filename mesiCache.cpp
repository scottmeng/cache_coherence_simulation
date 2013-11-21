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

void mesiCache::selfChangeState(unsigned addr, int instrType, bool isShared, int cycle){
	int row = getRowNum(addr);
    int col = getColNum(addr);
    int tag = (addr / (_blockSize / 2)) / _height;
    int minLRU = cycle + 1;
    int changeBlock = -1;

	// If the copy is not in the cache
    if(col < 0) {
        for(int i = 0; i < _width; i++) {
            if(minLRU > _cacheBlocks[row][i].lru) {
                minLRU = _cacheBlocks[row][i].lru;
                changeBlock = i;
            }
        }
        if(changeBlock > 0) {
            // Update the LRU
            _cacheBlocks[row][changeBlock].lru = cycle;
            //If it a read type
            if(instrType == READ) {
                if(isShared)
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::SHARED;
                else
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::EXCLUSIVE;
            } else {
                _cacheBlocks[row][changeBlock].tag = tag;
                if(isShared)
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::MODIFIED;
                else
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::MODIFIED;
            }
        }  
    // Else the copy is in the cache
    } else {
        _cacheBlocks[row][col].lru = cycle;
        // Only consider the write instruction, as read won't change the state in its own blocks
        if(instrType == WRITE) {
            _cacheBlocks[row][col].tag = tag;
            
            if(_cacheBlocks[row][changeBlock].blockStatus == cacheBlock::EXCLUSIVE)
                _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::MODIFIED;
            else if(_cacheBlocks[row][changeBlock].blockStatus == cacheBlock::SHARED) 
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::MODIFIED;
            else if(_cacheBlocks[row][changeBlock].blockStatus == cacheBlock::MODIFIED)
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::MODIFIED;
            }
    }
    
}

void mesiCache::otherChangeState(unsigned addr, int transType, int cycle){
	int row = getRowNum(addr);
    int col = getColNum(addr);
    int tag = (addr / (_blockSize / 2)) / _height;
    int minLRU = cycle + 1;
    int changeBlock = -1;

    // Only change state when the cache block is inside the cache
    if(col >= 0) {
        switch(transType){
        case BUS_RD:
            _cacheBlocks[row][col].blockStatus = cacheBlock::SHARED;
            break;
		case BUS_RD_X:
			_cacheBlocks[row][col].blockStatus = cacheBlock::INVALID;
			break;
        }
    }
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

transaction mesiCache::generateTransaction(unsigned addr, int instrType, int prIndex){
	int row = getRowNum(addr);
    int col = getColNum(addr);
    transaction curXact;
    curXact.addr = addr;
    curXact.prIndex = prIndex;
    curXact.transType = -1;
    
    if(col < 0) {
        curXact.transType = BUS_RD;
    }
    else {
        switch(_cacheBlocks[row][col].blockStatus) {
		case cacheBlock::INVALID:
			
			break;
        case cacheBlock::SHARED:
			break;
        case cacheBlock::MODIFIED:
            if(instrType == WRITE)
                curXact.transType = BUS_UPDATE;
            break;
		case cacheBlock::EXCLUSIVE:
			break;
        }
    }
    
    return curXact;
}
