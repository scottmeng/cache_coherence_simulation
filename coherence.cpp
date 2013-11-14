// coherence.cpp
// Created by: Long Jinghan, Meng Kaizhi, Zhou Ruofan (National University of Singapore)
// Created on: 7 Nov 2013
// Last edited on: 7 Nov 2013
// All rights reserved

#include <stdio.h>
#include <vector>

using namespace std;

// to be added
class cacheBlock {
	public:
		// define enum for cache_unit status
		enum status {
			INVALID, MODIFIED, SHARED, EXCLUSIVE
		};

		vector<int> addresses;
		int numOfUnits;
		status blockStatus;

		cacheBlock(int blockSize) {
			numOfUnits = blockSize / 2;
			for(int i = 0; i < numOfUnits; i++) {
				addresses.push_back(0);
			}
			blockStatus = INVALID;
		}
};

/*
 * 	word size : 16 bits = 2 bytes	
 *	# of lines : cache_size / (block_size * associativity)		- height of 2D array
 *	# of bytes in one line : block_size * associativity 		
 * 	# of cache units in one line : block_size * associativity / 2 - width of 2D array
 */

class cache {
	private:
		int _cacheSize;
		int _blockSize;
		int _associativity;

		int _height;
		int _numOfBlocks;		

		vector<cacheBlock> _cacheBlocks;

	public:
		cache(int cacheSize, int blockSize, int associativity) {
			_cacheSize = cacheSize;
			_blockSize = blockSize;
			_associativity = associativity;

			// compute width and height
			_numOfBlocks = cacheSize / blockSize;
			_height = cacheSize / (blockSize * associativity);

			// initialize all cache blocks using dirty data
			for(int i = 0; i < _numOfBlocks; i++) {
				cacheBlock dirtyBlock(_blockSize);
				_cacheBlocks.push_back(dirtyBlock);
			}

		}

		// check if the requested memory block is present in cache
		bool isCacheHit(int address) {


			return false;
		}

		// calculate the row index of the corresponding 
		int calRowNum(int address) {
			return 0;
		}
};

int main() {

	// read user inputs
	// cache-size, isAssociative, #processors, block-size, input-file

	// initialize cache block for all processors (#processors)
	// using parameters: cache-size, isAssociative, block-size

	// fopen, read file * #processors

	// read single instruction from each processor
	// serialize memory access (if any) 


	return 1;
}