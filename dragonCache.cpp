#include "dragonCache.h"

void dragonCache::selfChangeState(unsigned addr, int instrType, bool isShared, int cycle) {
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
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::SHAREDCLEAN;
                else
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::EXCLUSIVE;
            } else {
                _cacheBlocks[row][changeBlock].tag = tag;
                if(isShared)
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::SHAREDMODIFIED;
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
            else if(_cacheBlocks[row][changeBlock].blockStatus == cacheBlock::SHAREDCLEAN) {
                if(isShared)
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::SHAREDMODIFIED;
                else
                    _cacheBlocks[row][changeBlock].blockStatus = cacheBlock::MODIFIED;
            } else if(_cacheBlocks[row][changeBlock].blockStatus == cacheBlock::SHAREDMODIFIED) {
                if(!isShared)
                    _cacheBlocks[row][changeBlock].blockStatus == cacheBlock::MODIFIED;
            }
        }
    }
}

int dragonCache::otherChangeState(unsigned addr, int transType, int cycle) {
    int row = getRowNum(addr);
    int col = getColNum(addr);
    int tag = (addr / (_blockSize / 2)) / _height;
    int minLRU = cycle + 1;
    int changeBlock = -1;
    int transType = -1;

    // Only change state when the cache block is inside the cache
    if(col >= 0) {
        switch(transType){
        case BUS_RD:
            if(_cacheBlocks[row][col].blockStatus == cacheBlock::EXCLUSIVE) {
                _cacheBlocks[row][col].blockStatus = cacheBlock::SHAREDCLEAN;
                transType = UPDATE;
            } else if(_cacheBlocks[row][col].blockStatus == cacheBlock::MODIFIED) {
                _cacheBlocks[row][col].blockStatus = cacheBlock::SHAREDMODIFIED;
                transType = FLUSH;
            } else if(_cacheBlocks[row][col].blockStatus == cacheBlock::SHAREDMODIFIED) {
                transType = FLUSH;
            }
            break;

        case BUS_UPDATE:
            if(_cacheBlocks[row][col].blockStatus == cacheBlock::SHAREDMODIFIED) {
                _cacheBlocks[row][col].blockStatus = cacheBlock::SHAREDCLEAN;
            } else if(_cacheBlocks[row][col].blockStatus == cacheBlock::SHAREDCLEAN) {
                transType = UPDATE;
            }
            break;
        }
    }
    return transType;
}

//bool isCacheModified(unsigned addr);
transaction dragonCache::generateTransaction(unsigned addr, int instrType, int prIndex) {
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
        case cacheBlock::SHAREDCLEAN:
        case cacheBlock::SHAREDMODIFIED:
            if(instrType == WRITE)
                curXact.transType = BUS_UPDATE;
            break;
        }
    }
    
    return curXact;
}