#include "stdlib.h"
#include "stdio.h"
#include "string.h"

uint8_t receiverBuffer[1024];

char *payloadStringReceived;
uint8_t * payload;
uint8_t   payloadSize=0;

uint8_t *packet;
uint8_t packetSize=0;

void addToPacket(uint8_t * data, uint8_t dataSize, uint8_t * packetf, uint8_t * packetSizef);
void composePayload(char * payloadString);
uint8_t * createPrefix(uint8_t A5);
void printReadyPayload();
void sendPayload();
void findPayload();

int main() {
  payloadStringReceived = "fuck this";
  char * payloadString = malloc(30);

  composePayload(payloadString);
  printReadyPayload();
  sendPayload();

  findPayload();

  printf("%s",payloadStringReceived);

  return 0;
}

void composePayload(char * payloadString) {

  printf("%s\n",payloadString);

  uint8_t *prefixA = createPrefix(0xA);
  uint8_t *prefix5 = createPrefix(0x5);
  addToPacket(prefixA,16,packet,&packetSize);
  addToPacket(prefix5,16,packet,&packetSize);

}

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

void addToPacket(uint8_t * data, uint8_t dataSize, uint8_t * packetf, uint8_t * packetSizef) {
  if (packetf == NULL) {
    packetf = malloc(dataSize * sizeof(data[0]));
  } else {
    uint8_t * temp = malloc((dataSize+(*packetSizef))* sizeof(uint8_t));
    memcpy(temp, packetf, (*packetSizef) * sizeof(uint8_t));
    memcpy(temp+(*packetSizef), data, dataSize * sizeof(uint8_t));
    free(packetf);
    packetf = temp;
  }
  *packetSizef += dataSize;
}

void printReadyPayload() {
  for (int i = 0; i < packetSize; i++) {
    printf("%02x\n",payload[i]);
  }
  printf("Packet Size: %u\n", packetSize);
}

void sendPayload() {

}

void findPayload() {

}
