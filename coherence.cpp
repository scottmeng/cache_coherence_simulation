// coherence.cpp
// Created by: Long Jinghan, Meng Kaizhi, Zhou Ruofan (National University of Singapore)
// Created on: 7 Nov 2013
// Last edited on: 21 Nov 2013
// All rights reserved

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>
#include <queue>

#include "cache.h"
#include "mesiCache.h"
#include "transaction.h"
#include "dragonCache.h"
#include "constants.h"

using namespace std;

struct instruction {
	int instrType;
	unsigned addr;

	instruction() {
		instrType = 0;
		addr = 0;
	}
};

struct busRequest{
	int prIndex;
	unsigned addr;
	int countDown;
	bool fromCache;

	busRequest(int inPrIndex, unsigned inAddr, bool inFromCache) {
		prIndex = inPrIndex;
		addr = inAddr;
		fromCache = inFromCache;

		if(fromCache) {
			countDown = 1;
		} else {
			countDown = 10;
		}
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
	vector<int> numOfCycles, numOfDataAccesses, numOfDataMisses;

	// count of CPU cycles
	int wait = 0;

	// data and address buses shared between memory and cache
	queue<busRequest> inBuffer, outBuffer;
	vector<busRequest> processingRequests;
	
	// bus transaction queue
	queue<transaction> transactions;

	// bus transaction request

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

	vector<dragonCache> caches;
	vector<instruction> curInstrs;
	vector<bool> finished;

	// initialize caches, instructions and performance statistics
	for(int i = 0; i < noProcessors; i++) {
		dragonCache simpleCache(cacheSize, blockSize, associativity);
		caches.push_back(simpleCache);

		instruction curInstr;
		curInstrs.push_back(curInstr);

		finished.push_back(false);
		numOfCycles.push_back(0);
		numOfDataAccesses.push_back(0);
		numOfDataMisses.push_back(0);
	}

	while(1) {
		
		// new CPU cycle
		cycle += 1;

		// temperary storage for bus requests (for randomnization)
		vector<busRequest> requests;

		// dequeue bus transaction
		transaction curTrans = transactions.front();
		transactions.pop();

		bool isShared = false;
		bool isModified = false;

		// for each other processor
		for(int i = 0; i < noProcessors; i++) {
			if(i != curTrans.prIndex) {
				
				// check if any of the caches contain the same cache block
				if(caches[i].isCacheHit(curTrans.addr)) {
					isShared = true;
				}

				// make necessary state transition
				// and generate data bus request type
				int requestType = caches[i].otherChangeState(curTrans.addr, curTrans.transType, cycle);

				if(requestType == FLUSH) {
					busRequest flushRequest(curTrans.prIndex, curTrans.addr, true);
					requests.push_back(flushRequest);
				}
			}
		}

		// for the origin processor
		// if cache block exists, make state transition
		if(caches[curTrans.prIndex].isCacheHit(curTrans.addr)) {
			caches[curTrans.prIndex].selfChangeState(curTrans.addr, curInstrs[curTrans.prIndex].instrType, isShared, cycle);
		} else {
			// if no caches contain the copy
			// fetch from memory
			if(!isShared) {
				busRequest fetchRequest(curTrans.prIndex, curTrans.addr, false);
				requests.push_back(fetchRequest);
			}
		}
		
		// randomize the sequence of bus requests
		// and push them into the queue
		while(requests.size() > 0){
			int index = rand()%(requests.size());
			inBuffer.push(requests[index]);
			requests.erase(requests.begin()+index);
		}

		// if any of the requests processing right now is ready
		// push them into the out buffer
		// and remove from processing request list
		for(int i = 0; i < processingRequests.size(); i++) {
			processingRequests[i].countDown -= 1;
			if(processingRequests[i].countDown == 0) {
				outBuffer.push(processingRequests[i]);
				processingRequests.erase(processingRequests.begin() + i);
			}
		}

		// put the request onto shared data bus
		busRequest curRequest = outBuffer.front();
		outBuffer.pop();

		// load cache block in the corresponding cache
		// and unlock the cache
		caches[curRequest.prIndex].selfChangeState(curRequest.addr, curInstrs[curRequest.prIndex].instrType, false, cycle);
		caches[curRequest.prIndex].blocked = false;

		// put one bus request onto the address bus
		busRequest newRequest = inBuffer.front();
		inBuffer.pop();

		// start processing a new request
		processingRequests.push_back(newRequest);

		// for every processor
		for(int prIndex = 0; prIndex < noProcessors; prIndex ++) {
			
			// if the processor is still waiting for data
			// skip this cycle
			if(caches[prIndex].blocked) {
				continue;
			}

			// read single instruction from each processor
			// if there is no more instruction
			// assign
			if(!readInstr(files[prIndex], curInstrs[prIndex])) {
				if(!finished[prIndex]) {
					numOfCycles[prIndex] = cycle;
				}
				continue;
			}

			// if it is instruction reference 
			// simply increment cycle counter
			if(curInstrs[prIndex].instrType == 0) {
				continue;
			}


			transaction trans = caches[prIndex].generateTransaction(curInstrs[prIndex].addr, curInstrs[prIndex].instrType, prIndex);

			// if no transaction is needed
			// change state immediately
			if(trans.transType == -1) {
				caches[prIndex].selfChangeState(curInstrs[prIndex].addr, curInstrs[prIndex].instrType, false, cycle);
				continue;
			}

			// otherwise push transaction onto the bus and notify other caches in order
			// block the corresponding cache
			transactions.push(trans);
			caches[prIndex].blocked = true;

			/*
			// if it is a memory hit
			// perform state transition
			// notify other processors if needed
			// let all other processors respond
			if(caches[prIndex].isCacheHit(curInstrs[prIndex].addr)){
				
				

				bool isShared = false;

				// check if the same cache block exists in other caches
				for(int newPrIndex = 0; newPrIndex < noProcessors; newPrIndex++) {
					if(newPrIndex != prIndex && caches[newPrIndex].isCacheHit(curInstrs[prIndex].addr)) {
						isShared = true;
					}
				}

				// perform state transition
				int transType = caches[prIndex].stateTransit(curInstrs[prIndex], isShared);

				// other processors respond to bus transaction
				for(int newPrIndex = 0; newPrIndex < noProcessors; newPrIndex++) {
					if(newPrIndex != prIndex) {
						caches[newPrIndex].respondToTransaction(transType, curInstrs[prIndex]);
					}
				}
			} else {

				// generate bus request and block current processor
				busRequest request(prIndex, curInstrs[prIndex].addr, 0);
				requests.push_back(request);
				caches[prIndex].blocked = true;
			}*/
		}
	}

	// output statistics

	for(int i = 0; i < noProcessors; i++) {
		printf("Statistics for processor #%d:\n", i);
		printf("Total cycle is: %d\n", numOfCycles[i]);
		printf("Total number of data access is: %d\n", numOfDataAccesses[i]);
		printf("Total number of data miss is: %d\n", numOfDataMisses[i]);
	}

	// close files
	for(int i = 0; i < noProcessors; i++) {
		fclose(files[i]);
	}

	// hold screen
	while(1) {}

	return 1;
}
