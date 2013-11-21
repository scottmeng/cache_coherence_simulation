#ifndef MESICACHE_H
#define MESICACHE_H

#include "cache.h"
#include "transaction.h"
using namespace std;

class mesiCache : public cache{
public:
	bool blocked;

	mesiCache(){}
	mesiCache(int cacheSize, int blockSize, int associativity);
	bool isReadHit(unsigned addr, int cycle);
	bool isWriteHit(unsigned addr, int cycle);
	void readCache(unsigned addr, int cycle);
	void writeCache(unsigned addr, int cycle);

	void selfChangeState(unsigned addr, int instrType, bool isHit,int cycle);
	void otherChangeState(unsigned addr, int instrType, bool isHit, int cycle);

	bool isCacheModified(unsigned addr);
	transaction generateTransaction(unsigned addr, int instrType);
	
};

#endif