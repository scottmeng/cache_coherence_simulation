#ifndef MESICACHE_H
#define MESICACHE_H

#include "cache.h"
using namespace std;

class mesiCache : public cache{
public:
	mesiCache(){}
	mesiCache(int cacheSize, int blockSize, int associativity);
	bool isReadHit(int addr, int cycle);
	bool isWriteHit(int addr, int cycle);
	void readCache(int addr, int cycle);
	void writeCache(int addr, int cycle);
};

#endif