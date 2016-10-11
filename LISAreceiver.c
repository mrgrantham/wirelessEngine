#include "LISAreceiver.h"
#include "queue.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#define MAX_PACKET_SIZE 64 // in bytes
#define RECEIVER_BUFFER_SIZE 1024 // in bytes
#define SEGMENT_SIZE 4 // in bytes
#define PREFIX_SIZE 32 // in bytes

uint8_t readBit() {
  // capture bit
  uint8_t temp = getFront();
  pop();
  return temp;
}

void printTransmissionLine() {
  uint8_t temp=0;
  uint32_t index=0;
  while(temp != 0xff) {
    temp = getNext(0);
    if (temp!= 0xff) {
      printf("[%2d]  %2x %c\n",index,temp,temp);
    }
    index++;
  }
  getNext(1);
}

void sendToBuffer(uint8_t aByte,uint8_t *receiverBuffer) {
  static uint8_t byteIndex = 0;
  static uint32_t bufferIndex = 0;
  receiverBuffer[bufferIndex] &= ~(0x01 << (7-byteIndex));
  receiverBuffer[bufferIndex] |= ((aByte & 0x01) << (7-byteIndex));
  byteIndex = (byteIndex + 1) % 8;

  if (byteIndex == 0) {
    bufferIndex = (bufferIndex+1) % 64;
  }
}

void printBuffer(uint8_t *receiverBuffer) {
  for (int i; i < 1024; i++) {
    printf("%2c %2x ", receiverBuffer[i],receiverBuffer[i]);
  }
}

void findPacket() {

}

uint32_t findPrefix(uint8_t *receiverBuffer) {
  static uint8_t matchString[] = {0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
                                  0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
                                  0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,
                                  0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf};
  static uint8_t segment[4];

  // check every 32 bit sequence in the buffer
  int bufferBitSize = RECEIVER_BUFFER_SIZE;
  bufferBitSize <<= 3;
  for(int buffIndex = 0; buffIndex < bufferBitSize; buffIndex++) {
      // make segment to test against matchString
      printf("buffer subString\n");
      uint8_t *buffSegment = makeSubString(buffIndex,
                                              receiverBuffer,
                                              RECEIVER_BUFFER_SIZE,
                                              SEGMENT_SIZE);


      // compare segment to each 32 bit section prefixString
      int matchStringBitSize = PREFIX_SIZE;
      matchStringBitSize<<=3;
      for (int compareIndex = 0; compareIndex < matchStringBitSize; compareIndex++) {
          // make substring from matchString
          printf("prefix subString\n");
          uint8_t *matchStrSegment = makeSubString(compareIndex,
                                                  matchString,
                                                  PREFIX_SIZE,
                                                  SEGMENT_SIZE);

          uint8_t match = 1;

          for(int checkByte = 0; checkByte < SEGMENT_SIZE; checkByte++) {
              if(buffSegment[checkByte] != matchStrSegment[checkByte]) {
                printf("NOT A MATCH\n\n" );
                match = 0;
                break;
              }
          }
          if (match) {
            // return the index for the start of the payload
            printf("-----MATCH-----\n\n" );
            free(buffSegment);
            printf("buffIndex: %2d compareIndex: %2d\n",buffIndex,compareIndex );
            return buffIndex + 32 - compareIndex; // return exact index in buffer where payload begins
          }

      }
  }

  int mainPayloadIndex;
  return mainPayloadIndex;
}

uint8_t *makeSubString(uint32_t startingDatBitIndex, uint8_t *dataArray, uint32_t dataArraySize, uint32_t subStringSize) {
  //printf("\nstartingDatBitIndex: %d\n dataArraySize: %d\n subStringSize: %d\n",startingDatBitIndex,dataArraySize,subStringSize );
  printf("----subString source array---\n");
  printArray(dataArray, dataArraySize);

  uint8_t *subString;
  subString = malloc(subStringSize * sizeof(dataArray[0]));
  memset(subString, 0, subStringSize * sizeof(dataArray[0]));

  int32_t dataArrayBitSize = dataArraySize << 3;
  int32_t subStringBitSize = subStringSize << 3;

  for (int bitIndex=0; bitIndex < subStringBitSize; bitIndex++) {
    int32_t currDatByteIndex = ((startingDatBitIndex + bitIndex)>>3) % dataArraySize;
    int32_t currDatBitIndex = (startingDatBitIndex + bitIndex) % 8;
    int32_t currStrByteIndex = bitIndex>>3;
    int32_t currStrBitIndex = bitIndex % 8;
    //printf("\ncurrDatByteIndex: %d\ncurrDatBitIndex: %d\ncurrStrByteIndex: %d\ncurrStrBitIndex: %d\n",currDatByteIndex,currDatBitIndex,currStrByteIndex,currStrBitIndex );

    uint8_t tempStrBit = dataArray[currDatByteIndex] &
                                  (0x01<<currDatBitIndex);
    subString[currStrByteIndex] |= (currDatBitIndex-currStrBitIndex > 0) ?
                                    tempStrBit >> (currDatBitIndex-currStrBitIndex):
                                    tempStrBit << (currStrBitIndex-currDatBitIndex);
    printf("bitIndex %2d subString[%3d]: %2x ",bitIndex, currStrByteIndex,subString[currStrByteIndex]);
    printBinary(subString[currStrByteIndex]);
    printf("\n");

  }
  printf("----subString array---\n");
  printArray(subString, subStringSize);

  return subString;
}

void printArray(uint8_t *data, uint32_t dataSize) {
  printf("ARRAY: ");
   for (int i = 0; i < dataSize; i++) {
     printf("%x ", data[i]);
   }
   printf("\n");
}
