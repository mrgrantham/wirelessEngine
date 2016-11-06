#include "stdint.h"

uint8_t readBit();
void printTransmissionLine();
void sendToBuffer(uint8_t aByte,uint8_t *receiverBuffer);
void findPacket();
uint32_t bufferIndexChanged();
void printBuffer(uint8_t *receiverBuffer);
void printBufferChar(uint8_t *receiverBuffer);
int32_t findPrefix(uint8_t *receiverBuffer);
void makeSubString(uint8_t **subString,
						uint32_t startingSubStringBitIndex,
                        uint8_t *dataArray,
                        uint32_t dataArraySize,
                        uint32_t subStringSize);
char* makeSubStringChar(uint8_t **subString, uint32_t startingDatBitIndex, uint8_t *dataArray, uint32_t dataArraySize, uint32_t subStringSize);
void printArray(uint8_t *data, uint32_t dataSize);
void printArrayChar(uint8_t *data, uint32_t dataSize);
void printArrayBin(uint8_t *data, uint32_t dataSize);
