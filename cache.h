// cache.h
// Created by: Long Jinghan, Meng Kaizhi, Zhou Ruofan (National University of Singapore)
// Created on: 18 Nov 2013
// Last edited on: 18 Nov 2013
// All rights reserved

#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include "cacheBlock.h"

using namespace std;

class cache{
    protected:
        int _cacheSize;
        int _blockSize;
        int _associativity;

        int _height;
        int _width;
        int _numOfBlocks;		
		bool blocked;
        vector<vector <cacheBlock> > _cacheBlocks;

        int getRowIndex(unsigned addr);
        int getColNum(unsigned addr);
        int getRowNum(unsigned addr);

    public:
        cache() {}
        cache(int cacheSize, int blockSize, int associativity);

        bool isCacheHit(unsigned addr);
		
    protected:
        int getColNum(unsigned addr);
        int getRowNum(unsigned addr);


};

#endif
