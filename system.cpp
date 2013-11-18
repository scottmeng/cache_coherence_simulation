// coherence.cpp
// Created by: Long Jinghan, Meng Kaizhi, Zhou Ruofan (National University of Singapore)
// Created on: 7 Nov 2013
// Last edited on: 7 Nov 2013
// All rights reserved

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

#define DRAGON_PROTOCOL "DRAGON"
#define MESI_PROTOCOL "MESI"

#define FTT_FILE "FTT"
#define WEATHER_FILE "WEATHER"

// to be added
class cacheBlock {
	public:
		// define enum for cache_unit status
		enum status {
			INVALID, MODIFIED, SHARED, EXCLUSIVE
		};

		int numOfUnits;
		int tag;
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

/*
 * check if user inputs are still valid
 * prompt users of the correct input format
 */
bool areInputsValid(char * usrProtocol, char * usrInputFile, int usrNoProcessors,
					int usrBlockSize, int usrCacheSize, int usrAssociativity) {
	if(!strncmp(usrInputFile, FTT_FILE, sizeof(FTT_FILE)) && 
	   !strncmp(usrInputFile, WEATHER_FILE, sizeof(WEATHER_FILE))) {
		printf("Input file must be either'WEATHER' or 'FTT'. Please try again.\n");
		return false;
	}

	if(!strncmp(usrProtocol, DRAGON_PROTOCOL, sizeof(DRAGON_PROTOCOL)) && 
	   !strncmp(usrProtocol, MESI_PROTOCOL, sizeof(MESI_PROTOCOL))) {
		printf("Protocol must be either 'DRAGON' or 'MESI'. Please try again.\n");
		return false;
	}

	if((usrNoProcessors != 1) && 
	   (usrNoProcessors != 2) &&
	   (usrNoProcessors != 3) &&
	   (usrNoProcessors != 4)) {
		printf("Number of processors must be 1, 2, 4 or 8. Please try again.\n");
		return false;
	}

	if((usrBlockSize % 16) != 0) {
		printf("Block size must be a multiple of 16(default word size). Please try again.\n");
		return false;
	}

	if((usrCacheSize % (usrBlockSize * usrAssociativity)) != 0) {
		printf("Cache size must be a multiple of the production of block size and associativity. Please try again.\n");
		return false;
	}

	return true;
} 

int main(int argc, char * argv[]) {	

	// user inputs
	int noProcessors, blockSize, cacheSize, associativity;
	char * protocol, * inputFile;
	bool isDragon = false, isWeather = false;

	// performance statistics

	// read user inputs
	// cache-size, isAssociative, #processors, block-size, input-file
	if(argc != 7) {
		printf("Please follow the format specified: 'coherence protocol input_file no_processors cache_size associativity block_size'.\n");
		exit(EXIT_FAILURE);
	}

	protocol = argv[1];
	inputFile = argv[2];
	noProcessors = atoi(argv[3]);
	cacheSize = atoi(argv[4]);
	associativity = atoi(argv[5]);
	blockSize = atoi(argv[6]);

	// check if inputs are valid
	if(!areInputsValid(protocol, inputFile, noProcessors, 
					   cacheSize, associativity, blockSize)) {
		exit(EXIT_FAILURE);
	}

	if(strncmp(protocol, DRAGON_PROTOCOL, sizeof(DRAGON_PROTOCOL))) {
		isDragon = true;
	}

	// initialize cache block for all processors (#processors)
	// using parameters: cache-size, isAssociative, block-size

	// fopen, read file * #processors
	FILE * files[noProcessors];
	char indiFileName[20], * fileIndex;

	for(int i=1; i<=noProcessors; i++) {
		strcpy(indiFileName, inputFile);
		sprintf(fileIndex, "%d", i);

		strcat(indiFileName, fileIndex);
		strcat(indiFileName, ".prg");

		printf(indiFileName);
		files[i] = fopen(indiFileName, "r");
	}

	exit(0);		// for testing purpose

	while(1) {
		
		// read single instruction from each processor
		// serialize memory access (if any) 

		// if it is instruction reference 
		// simply increment cycle counter

		// if it is memory reference
		// check if it is memory read

			// if it is memory read
			// check isCacheHit
			// add time penalty
			// swap in cache block

			// if it is memory write
			// check isCacheHit
			// add time penalty
			// swap in cache block
			// modify block status
	}

	// output statistics


	// close files
	for(int i=1; i<=noProcessors; i++) {
		fclose(files[i]);
	}

	return 1;
}