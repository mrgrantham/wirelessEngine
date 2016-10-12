#include "LISAtransmitter.h"
#include "queue.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define MAX_PACKET_SIZE 64

uint8_t * createPrefix(uint8_t A5) {
  uint8_t * prefix = malloc(16*sizeof(uint8_t));

  for (uint8_t i = 0 ; i < 16 ; i++) {
    uint8_t temp = A5;
    temp <<= 4;
    temp |= i;
    prefix[i]   = temp;

  }
  return prefix;
}

void addToPacket(uint8_t * data, uint8_t dataSize, uint8_t ** packetf, uint8_t * packetSizef) {

  uint8_t * temp = malloc(MAX_PACKET_SIZE * sizeof(uint8_t));
  memset(temp, 0, MAX_PACKET_SIZE);
  if (packetf[0] != NULL) {
    memcpy(temp, packetf[0], packetSizef[0] * sizeof(uint8_t));
    free(packetf[0]);
  }
  memcpy(&temp[packetSizef[0]], data, dataSize * sizeof(uint8_t));
  packetf[0] = temp;
  *packetSizef += dataSize;
}

void printReadyPacket(uint8_t *packet, int16_t packetSize) {
  printf("----printReadyPacket----\n");
  for (uint8_t i = 0; i < packetSize; i++) {
    printf("[%2d]  %02x %c\n",i,packet[i],packet[i]);
  }
  printf("Packet Size: %u\n", packetSize);
}



void sendPacket(uint8_t *packet) {
  for (uint8_t i = 0; i < MAX_PACKET_SIZE; i++) {
    static uint8_t temp;
    temp = packet[i];
    printf("\n");
    for (int8_t b = 7; b >= 0; b--) {
        sendBit((packet[i] >> b) & 0x01);
    }

  }
}


void composePacket(char * payloadString, uint8_t ** packet, uint8_t * packetSize) {

  printf("--%s\n",payloadString);

  uint8_t *prefixA = createPrefix(0x5);
  uint8_t *prefix5 = createPrefix(0xA);
  addToPacket(prefixA,16,packet,packetSize);
  addToPacket(prefix5,16,packet,packetSize);
  addToPacket((uint8_t*)payloadString, strlen(payloadString), packet , packetSize);
  (*packet)[63] = '\n';
}

void sendBit(uint8_t bit) {
  // set pin to high or low
  // until that code is ready just do printf
  static uint32_t count = 0;
  static uint32_t byteCount = 0;

  if (count == 0) {
    //printf("\n[%d] SENT BYTE 0b", byteCount);
    byteCount++;
  }

  printf("%d", bit);
  push(bit);
  count = (count + 1) % 8;
}