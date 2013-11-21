#ifndef DRAGONCACHE_H
#define DRAGONCACHE_H

#define READ 2
#define WRITE 3

#include "cache.h"
#include "transaction.h"

class dragonCache : public cache {

public:
    void selfChangeState(unsigned addr, int instrType, bool isShared, int cycle);
    void otherChangeState(unsigned addr, int transType, int cycle);
    //bool isCacheModified(unsigned addr);
    transaction generateTransaction(unsigned addr, int instrType);


};

#endif