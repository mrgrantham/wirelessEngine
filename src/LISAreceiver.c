#include "board.h"

#include "LISAreceiver.h"
#include "queue.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#define MAX_PACKET_SIZE 64 // in bytes
#define RECEIVER_BUFFER_SIZE 128 // in bytes
#define SEGMENT_SIZE 4 // in bytes
#define PREFIX_SIZE 32 // in bytes

#define RX_PORT 0
#define RX_PIN 8


#define EXTERNAL_GREEN_LED_PORT 2
#define EXTERNAL_GREEN_LED_PIN 13

uint8_t readBit() {
  // capture bit
  static uint8_t temp;
  temp =  Chip_GPIO_GetPinState(LPC_GPIO, RX_PORT, RX_PIN);
  if(temp) {
	  Chip_GPIO_SetPinOutHigh(LPC_GPIO, EXTERNAL_GREEN_LED_PORT , EXTERNAL_GREEN_LED_PIN );
  } else {
	  Chip_GPIO_SetPinOutLow(LPC_GPIO, EXTERNAL_GREEN_LED_PORT , EXTERNAL_GREEN_LED_PIN );
  }
  //printf("%d",temp);
  return temp;
}

void printTransmissionLine() {
	uint8_t temp=0;
	uint32_t index=0;
	int32_t count = 1;
	while(temp != 0xff) {
		temp = getNext(0);
		if (temp!= 0xff) {
			static uint8_t bitLoader = 0;
			bitLoader |= (temp<<(7-(index%8)));
			//printf("[%2d]  %2x %c ",index,temp,temp);
			if(index%8==7) {
				printBinary(bitLoader);
				printf(" ");
				bitLoader=0;
				if(count%4==0) {
					printf("\n");
				}
				count++;
			}
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
    bufferIndex = (bufferIndex+1) % RECEIVER_BUFFER_SIZE;
  }
}

void printBuffer(uint8_t *receiverBuffer) {
  for (int i=0; i < RECEIVER_BUFFER_SIZE; i++) {
    printf("[%0.2x] ", receiverBuffer[i]);
  }
}

void printBufferChar(uint8_t *receiverBuffer) {
  for (int i=0; i < RECEIVER_BUFFER_SIZE; i++) {
    printf("[%c ] ", receiverBuffer[i]);
    if (i%8==7) {
    	printf("\n");
    }
  }
}

void printBufferBinary(uint8_t *receiverBuffer) {
  for (int i=0; i < RECEIVER_BUFFER_SIZE; i++) {
    printBinary(receiverBuffer[i]);
    printf(" ");
    if (i%8==7) {
    	printf("\n");
    }
  }
}

void findPacket() {

}

int32_t findPrefix(uint8_t *receiverBuffer) {
  static uint8_t matchString[] = {0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
                                  0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
                                  0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,
                                  0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf};

  // check every 32 bit sequence in the buffer
  static int bufferBitSize = (RECEIVER_BUFFER_SIZE<<3);
  static int buffIndex;
  for(buffIndex = 0; buffIndex < bufferBitSize; buffIndex++) {


      // compare segment to each 32 bit section prefixString
      static int matchStringBitSize = (PREFIX_SIZE<<3);
      static int compareIndex;
      for (compareIndex = 0; compareIndex < matchStringBitSize; compareIndex++) {

          uint8_t match = 1;

          // now that bytes are aligned they can be compared in 4 iterations
          static int checkBit;
          for(checkBit = 0; checkBit < (SEGMENT_SIZE<<3); checkBit++) {
        	  static int32_t currBuffByteIndex;
        	  static int32_t currBuffBitIndex;
        	  static int32_t currPrefByteIndex;
        	  static int32_t currPrefBitIndex;

        	  currBuffByteIndex = ((buffIndex + checkBit)%bufferBitSize)>>3;
        	  currBuffBitIndex = (buffIndex + checkBit) % 8;
        	  currPrefByteIndex = ((compareIndex + checkBit)%matchStringBitSize)>>3;
        	  currPrefBitIndex = (compareIndex + checkBit) % 8;

        	  static uint8_t buffBit;
        	  static uint8_t prefBit;

        	  buffBit = receiverBuffer[currBuffByteIndex]&(0x1<<currBuffBitIndex);
        	  buffBit = buffBit >> currBuffBitIndex;

        	  prefBit = matchString[currPrefByteIndex]&(0x1<<currBuffBitIndex);
        	  prefBit = prefBit >> currBuffBitIndex;



        	  if(buffBit != prefBit) {
        		  //printf("NOT A MATCH\n\n" );
        		  match = 0;
        		  break;
        	  }
          }
          if (match) {
        	  // return the index for the start of the payload
        	  return (buffIndex + matchStringBitSize - compareIndex) % bufferBitSize; // return exact index in buffer where payload begins
          }

      }
  }

  return -1;
}

void makeSubString(uint8_t **subString, uint32_t startingDatBitIndex, uint8_t *dataArray, uint32_t dataArraySize, uint32_t subStringSize) {
	//printf("\nstartingDatBitIndex: %d\n dataArraySize: %d\n subStringSize: %d\n",startingDatBitIndex,dataArraySize,subStringSize );
	//printf("----subString source array---\n");
	//printArray(dataArray, dataArraySize);
	//printf("realloc size: %d",subStringSize * sizeof(dataArray[0]));
	*subString = realloc((*subString), subStringSize * sizeof(dataArray[0]));
	memset(*subString, 0, subStringSize * sizeof(dataArray[0]));

	static int32_t dataArrayBitSize;
	dataArrayBitSize = dataArraySize << 3;
	static int32_t subStringBitSize;
	subStringBitSize = subStringSize << 3;

	static int bitIndex;
	for (bitIndex=0; bitIndex < subStringBitSize; bitIndex++) {
		static int32_t currDatByteIndex;
		static int32_t currDatBitIndex;
		static int32_t currStrByteIndex;
		static int32_t currStrBitIndex;

		currDatByteIndex = ((startingDatBitIndex + bitIndex)%dataArrayBitSize)>>3;
		currDatBitIndex = (startingDatBitIndex + bitIndex) % 8;
		currStrByteIndex = bitIndex>>3;
		currStrBitIndex = bitIndex % 8;
		//printf("\ncurrDatByteIndex: %d\ncurrDatBitIndex: %d\ncurrStrByteIndex: %d\ncurrStrBitIndex: %d\n",currDatByteIndex,currDatBitIndex,currStrByteIndex,currStrBitIndex );

		static uint8_t tempStrBit;
		tempStrBit = dataArray[currDatByteIndex] &
				(0x01<<currDatBitIndex);
		(*subString)[currStrByteIndex] |= (currDatBitIndex > currStrBitIndex) ?
						(tempStrBit >> (currDatBitIndex-currStrBitIndex)):
						(tempStrBit << (currStrBitIndex-currDatBitIndex));
		//printf("bitIndex %2d subString[%3d]: %2x ",bitIndex, currStrByteIndex,subString[currStrByteIndex]);
		//printBinary(subString[currStrByteIndex]);
		//printf("\n");

	}
	//printf("----subString array---\n");
	//printArray(subString, subStringSize);

}

char* makeSubStringChar(uint8_t **subString, uint32_t startingDatBitIndex, uint8_t *dataArray, uint32_t dataArraySize, uint32_t subStringSize) {
	makeSubString(subString, startingDatBitIndex,dataArray, dataArraySize, subStringSize);
	char * temp;
	if(subString[subStringSize-1] != '\0') {
		temp = malloc(subStringSize + 1);
		//free(*subString);
		temp[subStringSize] = '\0';
		//*subString = temp;
	} else {
		temp = malloc(subStringSize + 1);
	}
	memcpy(temp,*subString,subStringSize);

	return (char*)temp;

}

void printArray(uint8_t *data, uint32_t dataSize) {
   for (int i = 0; i < dataSize; i++) {
     printf("%0.2x ", data[i]);
   }
}

void printArrayChar(uint8_t *data, uint32_t dataSize) {
  printf("ARRAY: ");
   for (int i = 0; i < dataSize; i++) {
     printf("%2c ", data[i],data[i]);
   }
}

void printArrayBin(uint8_t *data, uint32_t dataSize) {
  printf("ARRAY: ");
   for (int i = 0; i < dataSize; i++) {
	   printBinary(data[i]);
     printf(" ");
   }
}
