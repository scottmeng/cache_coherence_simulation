#include "dragonCache.h"

void dragonCache::readCache(int addr, int cycle, int hitSet, bool shared) {
    int index;
    int tag;
    int minLRU = cycle + 1;
    int changeBlock = -1;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;

    if(hitSet >= 0)
    {
        _cacheBlocks[index][hitSet].lru = cycle;
        // Processor read hits don't affect state.

    // If hit misses
    } else
    {
        // Find least recently used block
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
        if(changeBlock > -1)
        {
            // Update the block
            _cacheBlocks[index][changeBlock].tag = tag;
            _cacheBlocks[index][changeBlock].lru = cycle;
            // Change states
            if(shared) 
                _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::SHAREDCLEAN;
            else
                _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::EXCLUSIVE;
        }   

    }
}

void dragonCache::writeCache(int addr, int cycle, int hitSet, bool shared) {
    int index;
    int tag;
    int minLRU = cycle + 1;
    int changeBlock = -1;
    index = (addr / (_blockSize / 2)) % _height;
    tag = (addr / (_blockSize / 2)) / _height;
    
    // If write hits, do transition directly
    if(hitSet >= 0) 
    {
        //Update the block
        _cacheBlocks[index][hitSet].tag = tag;
        _cacheBlocks[index][hitSet].lru = cycle;
        if(_cacheBlocks[index][hitSet].blockStatus == cacheBlock::SHAREDCLEAN)
        {
            if(shared) 
                _cacheBlocks[index][hitSet].blockStatus = cacheBlock::SHAREDMODIFIED;
            else
                _cacheBlocks[index][hitSet].blockStatus = cacheBlock::MODIFIED;
        } else if(_cacheBlocks[index][hitSet].blockStatus == cacheBlock::SHAREDMODIFIED)
        {
            if(!shared)
                _cacheBlocks[index][hitSet].blockStatus = cacheBlock::MODIFIED;
        } else if(_cacheBlocks[index][hitSet].blockStatus == cacheBlock::EXCLUSIVE)
        {
            _cacheBlocks[index][hitSet].blockStatus = cacheBlock::MODIFIED;
        }

    } else 
    {
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
        if(changeBlock > -1)
        {
            _cacheBlocks[index][changeBlock].tag = tag;
            _cacheBlocks[index][changeBlock].lru = cycle;
            if(shared) 
                _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::SHAREDMODIFIED;
            else
                _cacheBlocks[index][changeBlock].blockStatus = cacheBlock::MODIFIED;
        }   
    return;
    }

}