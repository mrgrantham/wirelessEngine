#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "queue.h"
#include "LISAtransmitter.h"
#include "LISAreceiver.h"

#define MAX_PAYLOAD_SIZE 64

uint8_t receiverBuffer[1024];

char *payloadStringReceived;
//uint8_t * payload;
//uint8_t   payloadSize=0;

uint8_t *packet;
uint8_t packetSize=0;

int main() {

  payloadStringReceived = "fuck this";
  char * payloadString = malloc(32);
  memcpy(payloadString,payloadStringReceived,strlen(payloadStringReceived));


  composePacket(payloadString, &packet, &packetSize);
  printReadyPacket(packet, 64);


  sendPacket(packet, packetSize);

  printTransmissionLine();

  findPacket();

  printf("%s\n",payloadStringReceived);

  return 0;
}
