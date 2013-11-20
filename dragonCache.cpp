#include "dragonCache.h"



bool dragonCache::isWriteHit(int addr, int cycle) {
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

void dragonCache::writeCache(int addr, int cycle) {

}