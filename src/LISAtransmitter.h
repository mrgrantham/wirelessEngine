// LISA transmitter header file
// Authored by James Grantham
#include "stdint.h"

void addToPacket(uint8_t * data, uint32_t dataSize, uint8_t ** packetf, uint32_t * packetSizef);
void composePacket(char * payloadString, uint8_t ** packet, uint32_t * packetSize);
uint8_t * createPrefix(uint8_t A5);
void printReadyPacket(uint8_t *packet, int16_t payloadSize);
void sendPacket(uint8_t *packet);
void deletePacket(uint8_t **packet, uint32_t *packetSize);
uint8_t sendBit();
void pushBit(uint8_t bit);
