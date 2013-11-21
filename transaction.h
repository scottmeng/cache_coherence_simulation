#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "constants.h"

class transaction {
public:
    unsigned addr;
    int transType;
    int prIndex;
};
#endif