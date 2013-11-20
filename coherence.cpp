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

#include "cache.h"

using namespace std;

#define DRAGON_PROTOCOL "DRAGON"
#define MESI_PROTOCOL "MESI"

#define FTT_FILE "FTT"
#define WEATHER_FILE "WEATHER"


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

	if((usrBlockSize % 2) != 0) {
		printf("Block size must be a multiple of 16(default word size). Please try again.\n");
		return false;
	}

	if((usrCacheSize % (usrBlockSize * usrAssociativity)) != 0) {
		printf("Cache size must be a multiple of the production of block size and associativity. Please try again.\n");
		return false;
	}

	return true;
} 

int readInstrType(FILE * file) {
	char buf[20];

	if(fgets(buf, sizeof(buf), file)) {
		int instrType;
		int size = sscanf(buf, "%d", &instrType);

		if(size == 1) {
			return instrType;
		}
	}

	return 0;
}

int readAddr(FILE * file) {
	char buf[20];

	if(fgets(buf, sizeof(buf), file)) {
		int addr;
		int size = sscanf(buf, "%d", &addr);

		if(size == 1) {
			return addr;
		}
	}

	return 0;
}

int main(int argc, char * argv[]) {	

	// user inputs
	int noProcessors, blockSize, cacheSize, associativity;
	char * protocol, * inputFile;
	bool isDragon = false, isWeather = false;

	// performance statistics

	// count of CPU cycles
	int wait = 0, cycle = 0;
	int instrType, addr;

	// ================ for debug purpose only ====================

	protocol = "DRAGON";
	inputFile = "FFT";
	noProcessors = 1;
	cacheSize = 1024;
	associativity = 4;
	blockSize = 8;

	// ================ for debug purpose only ====================

	/************************************************
	 ***** uncomment this section when releasing ****
	 ************************************************

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

	************************************************
	***** uncomment this section when releasing ****
	************************************************/

	// check if inputs are valid
	if(!areInputsValid(protocol, inputFile, noProcessors, 
					   blockSize, cacheSize, associativity)) {
		exit(EXIT_FAILURE);
	}

	if(strncmp(protocol, DRAGON_PROTOCOL, sizeof(DRAGON_PROTOCOL))) {
		isDragon = true;
	}

	// initialize cache block for all processors (#processors)
	// using parameters: cache-size, isAssociative, block-size

	// fopen, read file * #processors
	FILE * files[8];
	char indiFileName[20];
	char fileIndex[2] = "";

	for(int i = 0; i < noProcessors; i++) {
		strcpy(indiFileName, inputFile);
		sprintf(fileIndex, "%d", i+1);

		strcat(indiFileName, fileIndex);
		strcat(indiFileName, ".prg");

		files[i] = fopen(indiFileName, "r");

		// if any of the files cannot be opened
		// exit the program
		if(!files[i]) {
			printf("could not open %s \n", indiFileName);
			exit(EXIT_FAILURE);
		}
	}

	//cache simpleCache(cacheSize, blockSize, associativity);
	cache simpleCache;

	while(1) {
		
		// new CPU cycle
		cycle += 1;

		// empty cycles
		if(wait > 0 ) {
			wait -= 1;
			continue;
		}

		// read single instruction from each processor
		instrType = readInstrType(files[0]);
		addr = readAddr(files[0]);

		// if it is instruction reference 
		// simply increment cycle counter
		if(instrType == 0) {
			continue;
		}

		// if it is memory read
		// check isCacheHit
		// add time penalty
		// swap in cache block
		if(instrType == 2) {
			if(!simpleCache.isReadHit(addr, cycle)) {
				simpleCache.readCache(addr, cycle);
				wait = 10;
				continue;
			}
		}

		// if it is memory write
		// check isCacheHit
		// add time penalty
		// swap in cache block
		// modify block status
		if(instrType == 3) {
			if(!simpleCache.isWriteHit(addr)) {
				simpleCache.writeCache(addr);
				wait = 10;
				continue;
			}
		}
	}

	// output statistics


	// close files
	for(int i=1; i<=noProcessors; i++) {
		fclose(files[i]);
	}

	return 1;
}