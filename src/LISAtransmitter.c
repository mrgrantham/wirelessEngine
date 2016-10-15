#include "LISAtransmitter.h"
#include "queue.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "board.h"

#define TX_PORT 0
#define TX_PIN 9

//#include "string.h"

#define MAX_PACKET_SIZE 64

uint8_t * createPrefix(uint8_t A5) {
  uint8_t * prefix = (uint8_t*)malloc(16*sizeof(uint8_t));

  for (uint8_t i = 0 ; i < 16 ; i++) {
    uint8_t temp = A5;
    temp <<= 4;
    temp |= i;
    prefix[i]   = temp;

  }
  return prefix;
}

void addToPacket(uint8_t * data, uint32_t dataSize, uint8_t ** packetf, uint32_t * packetSizef) {

  uint8_t * temp = (uint8_t*)malloc((dataSize+packetSizef[0]) * sizeof(uint8_t));
  memset(temp, 0, (dataSize+packetSizef[0]) * sizeof(uint8_t));
  if (packetSizef[0] > 0) {
    memcpy(temp, packetf[0], packetSizef[0] * sizeof(uint8_t));
    free(*packetf);
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



void sendPacket(uint8_t *packet, uint32_t packetSize) {
	if (packet != NULL) {
		for (uint32_t i = 0; i < packetSize; i++) {
			//printf("sendPacket i: %d packet[i] = %c\n",i,packet[i]);
			for (int32_t b = 7; b >= 0; b--) {
				pushBit((packet[i]>>b) & 0x01);
			}

		}
	}
}

void deletePacket(uint8_t **packet, uint32_t *packetSize) {
	free(*packet);
	packet[0] = NULL;
	packetSize[0] = 0;
}



void composePacket(char * payloadString, uint8_t ** packet, uint32_t * packetSize) {

  printf("--%s\n",payloadString);

  uint8_t *prefix5 = createPrefix(0x5);
  uint8_t *prefixA = createPrefix(0xA);
  addToPacket(prefix5,16,packet,packetSize);
  addToPacket(prefixA,16,packet,packetSize);
  addToPacket((uint8_t*)payloadString, strlen(payloadString)+1, packet , packetSize);
  addToPacket((uint8_t*)"\n",1, packet , packetSize);
  free(prefixA);
  free(prefix5);
}

void pushBit(uint8_t bit) {
  // set pin to high or low
  // until that code is ready just do printf
  static uint32_t count = 0;
  static uint32_t byteCount = 0;

  if (count == 0) {
    //printf("\n[%d] SENT BYTE 0b", byteCount);
    byteCount++;
  }

  //printf("%d", bit);
  push(bit);
  count = (count + 1) % 8;
}

uint8_t sendBit() {
  // capture bit
  static uint8_t val;
  val = getFront();
  static uint8_t temp;
  temp = (val == 0xff)? 0 : val;
  if(temp) {
	  Chip_GPIO_SetPinOutHigh(LPC_GPIO, TX_PORT, TX_PIN);
  } else {
	  Chip_GPIO_SetPinOutLow(LPC_GPIO, TX_PORT, TX_PIN);
  }
  pop();
  return temp;
}


