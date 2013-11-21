#include "dragonCache.h"
//
//void dragonCache::readCache(int addr, int cycle, int hitSet, bool shared) {
//    int index;
//    int tag;
//    int minLRU = cycle + 1;
//    int changeBlock = -1;
//    index = (addr / (_blockSize / 2)) % _height;
//    tag = (addr / (_blockSize / 2)) / _height;
//
//    if(hitSet >= 0)
//    {
//        _cacheBlocks[index][hitSet].lru = cycle;
//        // Processor read hits don't affect state.
//
//    // If hit misses
//    } else
//    {
//        // Find least recently used block
//        for(int i = 0; i < _width; i++) {
//            if(_cacheBlocks[index][i].blockStatus == cacheBlock::INVALID) {
//                changeBlock = i;
//                break;
//            } else {
//                if(minLRU > _cacheBlocks[index][i].lru) {
//                    minLRU = _cacheBlocks[index][i].lru;
//                    changeBlock = i;
//                }   
//            }
//        }
//        if(changeBlock > -1)
//        {
//            // Update the block
//            _cacheBlocks[index][changeBlock].tag = tag;
//            _cacheBlocks[index][changeBlock].lru = cycle;
//            // Change states
//            if(shared) 
//                _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::SHAREDCLEAN;
//            else
//                _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::EXCLUSIVE;
//        }   
//
//    }
//}
//
//void dragonCache::writeCache(int addr, int cycle, int hitSet, bool shared) {
//    int index;
//    int tag;
//    int minLRU = cycle + 1;
//    int changeBlock = -1;
//    index = (addr / (_blockSize / 2)) % _height;
//    tag = (addr / (_blockSize / 2)) / _height;
//    
//    // If write hits, do transition directly
//    if(hitSet >= 0) 
//    {
//        //Update the block
//        _cacheBlocks[index][hitSet].tag = tag;
//        _cacheBlocks[index][hitSet].lru = cycle;
//        if(_cacheBlocks[index][hitSet].blockStatus == cacheBlock::SHAREDCLEAN)
//        {
//            if(shared) 
//                _cacheBlocks[index][hitSet].blockStatus = cacheBlock::SHAREDMODIFIED;
//            else
//                _cacheBlocks[index][hitSet].blockStatus = cacheBlock::MODIFIED;
//        } else if(_cacheBlocks[index][hitSet].blockStatus == cacheBlock::SHAREDMODIFIED)
//        {
//            if(!shared)
//                _cacheBlocks[index][hitSet].blockStatus = cacheBlock::MODIFIED;
//        } else if(_cacheBlocks[index][hitSet].blockStatus == cacheBlock::EXCLUSIVE)
//        {
//            _cacheBlocks[index][hitSet].blockStatus = cacheBlock::MODIFIED;
//        }
//
//    } else 
//    {
//        for(int i = 0; i < _width; i++) {
//            if(_cacheBlocks[index][i].blockStatus == cacheBlock::INVALID) {
//                changeBlock = i;
//                break;
//            } else {
//                if(minLRU > _cacheBlocks[index][i].lru) {
//                    minLRU = _cacheBlocks[index][i].lru;
//                    changeBlock = i;
//                }   
//            }
//        }
//        if(changeBlock > -1)
//        {
//            _cacheBlocks[index][changeBlock].tag = tag;
//            _cacheBlocks[index][changeBlock].lru = cycle;
//            if(shared) 
//                _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::SHAREDMODIFIED;
//            else
//                _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::MODIFIED;
//        }   
//    return;
//    }
//
//}

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

void dragonCache::otherChangeState(unsigned addr, int transType, int cycle) {
    int row = getRowNum(addr);
    int col = getColNum(addr);
    int tag = (addr / (_blockSize / 2)) / _height;
    int minLRU = cycle + 1;
    int changeBlock = -1;

    // Only change state when the cache block is inside the cache
    if(col >= 0) {
        switch(transType){
        case BUS_RD:
            if(_cacheBlocks[row][col].blockStatus == cacheBlock::EXCLUSIVE)
                _cacheBlocks[row][col].blockStatus = cacheBlock::SHAREDCLEAN;
            else if(_cacheBlocks[row][col].blockStatus == cacheBlock::MODIFIED)
                _cacheBlocks[row][col].blockStatus = cacheBlock::SHAREDMODIFIED;
            break;

        case UPDATE:
            if(_cacheBlocks[row][col].blockStatus == cacheBlock::SHAREDMODIFIED) {
                _cacheBlocks[row][col].blockStatus == cacheBlock::SHAREDCLEAN;
            }
            break;
        }
    }
}

//bool isCacheModified(unsigned addr);
transaction dragonCache::generateTransaction(unsigned addr, int instrType) {
    transaction curXact;
}