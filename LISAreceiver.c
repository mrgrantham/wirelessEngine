#include "LISAreceiver.h"


void readBit() {
  // capture bit and send into buffer

}

void sendToBuffer(uint8_t aByte,uint8_t *receiverBuffer) {
  static uint32_t bufferIndex = 0;
  receiverBuffer[bufferIndex] = aByte;
  bufferIndex = (bufferIndex+1) % 64;
}

void findPacket() {

}
