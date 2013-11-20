#ifndef DRAGONCACHE_H
#define DRAGONCACHE_H

#include "cache.h"

class dragonCache : public cache {

public:
    bool isWriteHit(int addr, int cycle);
    void writeCache(int addr, int cycle);
    
};

#endif