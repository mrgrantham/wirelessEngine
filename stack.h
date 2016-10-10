

#include "stdint.h"
#include "stddef.h"

typedef struct  TransmissionItem TransmissionItem;

struct  TransmissionItem{

    uint8_t val; //
    TransmissionItem *next;

};

static TransmissionItem *top = NULL;

TransmissionItem *getTop();
void push(TransmissionItem *newItem);
void pop();
