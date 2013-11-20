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
#include "mesiCache.h"

using namespace std;

#define DRAGON_PROTOCOL "DRAGON"
#define MESI_PROTOCOL "MESI"

#define FTT_FILE "FTT"
#define WEATHER_FILE "WEATHER"


struct instruction {
	int instrType;
	unsigned addr;

	instruction() {
		instrType = 0;
		addr = 0;
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

bool readInstr(FILE * file, instruction &newInstr) {
	char buf[20];

	if(fgets(buf, sizeof(buf), file)) {
		int size = sscanf(buf, "%d %x", &newInstr.instrType, &newInstr.addr);

		if(size == 2) {
			return true;
		}
	}

	return false;
}

int main(int argc, char * argv[]) {	

	// user inputs
	int noProcessors, blockSize, cacheSize, associativity;
	char * protocol, * inputFile;
	bool isDragon = false, isWeather = false;

	// performance statistics
	int cycle = 0;
	int numOfInstr = 0;
	int numOfDataAccess = 0;
	int numOfDataMiss = 0;

	// count of CPU cycles
	int wait = 0;

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

	mesiCache simpleCache(cacheSize, blockSize, associativity);
	instruction curInstr;

	while(1) {
		
		// new CPU cycle
		cycle += 1;

		// empty cycles
		if(wait > 0 ) {
			wait -= 1;
			continue;
		}

		// read single instruction from each processor
		//instrType = readInstrType(files[0]);
		//addr = readAddr(files[0]);
		if(!readInstr(files[0], curInstr)) {
			break;
		}
		numOfInstr += 1;
		
		if((numOfInstr % 100000) == 0) {
			printf("-");
		}

		if(numOfInstr == 7451715) {
			int test = 0;
		}
		
		// if it is instruction reference 
		// simply increment cycle counter
		if(curInstr.instrType == 0) {
			continue;
		}
		numOfDataAccess += 1;

		// if it is memory read
		// check isCacheHit
		// add time penalty
		// swap in cache block
		if(curInstr.instrType == 2) {
			if(!simpleCache.isReadHit(curInstr.addr, cycle)) {
				simpleCache.readCache(curInstr.addr, cycle);
				wait = 10;
				numOfDataMiss += 1;
				//printf("Data read miss \n");
			}
			continue;
		}

		// if it is memory write
		// check isCacheHit
		// add time penalty
		// swap in cache block
		// modify block status
		if(curInstr.instrType == 3) {
			if(!simpleCache.isWriteHit(curInstr.addr,cycle)) {
				simpleCache.writeCache(curInstr.addr,cycle);
				wait = 10;
				numOfDataMiss += 1;
				//printf("Data write miss \n");
			}
			continue;
		}
	}

	// output statistics
	printf("Total cycle is: %d\n", cycle);
	printf("Total number of instructions is: %d\n", numOfInstr);
	printf("Total number of data access is: %d\n", numOfDataAccess);
	printf("Total number of data miss is: %d\n", numOfDataMiss);

	// close files
	for(int i = 0; i < noProcessors; i++) {
		fclose(files[i]);
	}

	// hold screen
	while(1) {}

	return 1;
}
