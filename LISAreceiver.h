#include "stdint.h"

uint8_t readBit();
void printTransmissionLine();
void sendToBuffer(uint8_t aByte,uint8_t *receiverBuffer);
void findPacket();
void printBuffer(uint8_t *receiverBuffer);
uint32_t findPrefix(uint8_t *receiverBuffer);
uint8_t *makeSubString(uint32_t startingSubStringBitIndex,
                        uint8_t *dataArray,
                        uint32_t dataArraySize,
                        uint32_t subStringSize);
void printArray(uint8_t *data, uint32_t dataSize);
