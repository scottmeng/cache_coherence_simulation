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

        vector<vector <cacheBlock> > _cacheBlocks;

        int getRowIndex(unsigned addr);

    public:
        cache() {}
        cache(int cacheSize, int blockSize, int associativity);

        bool virtual isReadHit(unsigned addr, int cycle);
        bool virtual isWriteHit(unsigned addr, int cycle);
        void virtual readCache(unsigned addr, int cycle);
        void virtual writeCache(unsigned addr, int cycle);

        int isCacheHit(unsigned addr);
};

#endif