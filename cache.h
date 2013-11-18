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
	private:
		int _cacheSize;
		int _blockSize;
		int _associativity;

		int _height;
		int _width;
		int _numOfBlocks;		

		vector<vector <cacheBlock> > _cacheBlocks;

	public:
		cache(int cacheSize, int blockSize, int associativity);
};


#endif