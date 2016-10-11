#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "queue.h"
#include "LISAtransmitter.h"
#include "LISAreceiver.h"

uint8_t receiverBuffer[1024];

char *payloadStringReceived;
uint8_t *packet;
uint8_t packetSize=0;

int main() {

  payloadStringReceived = "JAMES 6912";
  char * payloadString = malloc(32);
  memcpy(payloadString,payloadStringReceived,strlen(payloadStringReceived));


  composePacket(payloadString, &packet, &packetSize);
  printReadyPacket(packet, 64);


  sendPacket(packet);

  // random offset to test prefix finding algorithm
  push(0x1);
  push(0x1);
  push(0x1);
  push(0x1);
  push(0x1);
  push(0x0);
  push(0x1);
  push(0x1);
  push(0x1);
  push(0x0);
  push(0x0);

  // recieving side
  printTransmissionLine();

  uint8_t aBit = 0;
  aBit = readBit();
  while (aBit != 0xff) {
    sendToBuffer(aBit,receiverBuffer);
    aBit = readBit();
  }




  printf("stuff\n");
  printBuffer(receiverBuffer);

  uint32_t index;
  index = findPrefix(receiverBuffer);
  printf("\nbitIndex of match %d\n",index);
  findPacket();

  printf("%s\n",payloadStringReceived);

  return 0;
}
