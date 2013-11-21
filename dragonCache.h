#ifndef DRAGONCACHE_H
#define DRAGONCACHE_H

#include "cache.h"

class dragonCache : public cache {

public:
    void readCache(int addr, int cycle, int hitSet, bool shared);
    void writeCache(int addr, int cycle, int hitSet, bool shared);

};

#endif