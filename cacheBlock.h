// cacheBlock.h
// Created by: Long Jinghan, Meng Kaizhi, Zhou Ruofan (National University of Singapore)
// Created on: 18 Nov 2013
// Last edited on: 18 Nov 2013
// All rights reserved

#ifndef CACHEBLOCK_H
#define CACHEBLOCK_H

/*
 *	blockStatus : indicates the status of the cache block
 *	numOfUnits : number of words
 *	tag : identifier number
 */
class cacheBlock {
	public:
		// define enum for cache_unit status
		enum status {
			INVALID, MODIFIED, SHARED, EXCLUSIVE, SHAREDCLEAN, SHAREDMODIFIED
		};

		int tag;
		int numOfWords;
		status blockStatus;
		int lru;

		cacheBlock(int);
};

#endif