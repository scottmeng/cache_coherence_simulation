#include "cacheBlock.h"


cacheBlock :: cacheBlock(int blockSize) {
	numOfWords = blockSize / 2;
	tag = 0;
	blockStatus = INVALID;
}