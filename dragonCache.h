#ifndef DRAGONCACHE_H
#define DRAGONCACHE_H

#include "cache.h"

class dragonCache : public cache {

public:
    bool isReadHit(unsigned addr, int cycle);
    bool isWriteHit(unsigned addr, int cycle);
    void readCache(unsigned addr, int cycle);
    void writeCache(unsigned addr, int cycle);
    
};

#endif