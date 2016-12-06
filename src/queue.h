
#include "stdint.h"
#include "stddef.h"

typedef struct  TransmissionItem TransmissionItem;

static int32_t queueSize = 0;

struct  TransmissionItem{

    uint8_t val; //
    TransmissionItem *ahead;
    TransmissionItem *behind;

};

static TransmissionItem *front = NULL;
static TransmissionItem *back = NULL;

void push(uint8_t val);
void pop();
uint8_t getNext(uint8_t reset);
uint8_t getFront();
uint8_t getBack();
int32_t getQueueSize();
