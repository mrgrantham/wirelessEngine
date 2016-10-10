#include "LISAreceiver.h"
#include "queue.h"
#include "stdio.h"

void readBit() {
  // capture bit and send into buffer

}

void printTransmissionLine() {
  uint8_t temp=0;
  uint8_t index=0;
  while(temp!= 0xff) {
    temp = getNext(0);
    printf("[%2d]  %02x %c\n",index,temp,temp);
    index++;
  }
  getNext(1);
}

void sendToBuffer(uint8_t aByte,uint8_t *receiverBuffer) {
  static uint32_t bufferIndex = 0;
  receiverBuffer[bufferIndex] = aByte;
  bufferIndex = (bufferIndex+1) % 64;
}

void findPacket() {

}
