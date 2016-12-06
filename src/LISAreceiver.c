#include "board.h"

#include "LISAreceiver.h"
#include "queue.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#define MAX_PACKET_SIZE 64 // in bytes
#define RECEIVER_BUFFER_SIZE 128 // in bytes
#define SEGMENT_SIZE 10 // in bytes
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

static uint8_t bufferBitIndex = 0;
static uint32_t bufferByteIndex = 0;

void sendToBuffer(uint8_t aByte,uint8_t *receiverBuffer) {

  receiverBuffer[bufferByteIndex] &= ~(0x01 << (7-bufferBitIndex));
  receiverBuffer[bufferByteIndex] |= ((aByte & 0x01) << (7-bufferBitIndex));
  bufferBitIndex = (bufferBitIndex + 1) % 8;

  if (bufferBitIndex == 0) {
    bufferByteIndex = (bufferByteIndex+1) % RECEIVER_BUFFER_SIZE;
  }
}

uint32_t bufferIndexChanged(){
	  // skip function if the index hasn't added additional 64 bytes
	  static uint8_t prevBufferBitIndex = 0;
	  static uint32_t prevBufferByteIndex = 0;
	  static uint32_t changed;

	  changed = 1;
	  if ((prevBufferBitIndex == bufferBitIndex) && (prevBufferByteIndex == bufferByteIndex)) {
		  changed = 0;
	  }
	  prevBufferBitIndex = bufferBitIndex;
	  prevBufferByteIndex = bufferByteIndex;

	  return changed;
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
    static int32_t bufferBitSize = (RECEIVER_BUFFER_SIZE<<3);
    static int32_t buffIndex;


    // cycle through the length of the buffer
    for(buffIndex = 0; buffIndex < bufferBitSize; buffIndex++) {

        static int32_t first_loop_run;

        static int32_t bitDiff;

        // compare segment to each 32 bit section prefixString
        static int32_t matchStringBitSize = (PREFIX_SIZE<<3);
        static int32_t compareIndex;
        for (compareIndex = 0; compareIndex < matchStringBitSize; compareIndex++) {

            first_loop_run = 1;

//            printf("\n\ncompareIndex: %d\n",compareIndex);
            static uint8_t match;
            match = 1;

            static int32_t checkBit;
            // loop for the number of bits in the byte size we have chosen (4 bytes default)
            // check against prefix
            for(checkBit = 0; checkBit < (SEGMENT_SIZE<<3); checkBit++) {
                static int32_t currBuffByteIndex;
                static int32_t currBuffBitIndex;
                static int32_t currPrefByteIndex;
                static int32_t currPrefBitIndex;

                //
                //        	  currBuffByteIndex = ((buffIndex + checkBit)%bufferBitSize)>>3;
                //        	  currBuffBitIndex = (buffIndex + checkBit) % 8;
                //        	  currPrefByteIndex = ((compareIndex + checkBit)%matchStringBitSize)>>3;
                //        	  currPrefBitIndex = (compareIndex + checkBit) % 8;

                // try reading in reverse
                currBuffByteIndex = ((buffIndex + checkBit)%bufferBitSize)>>3;
                currBuffBitIndex = ((buffIndex + checkBit) % 8);
                currPrefByteIndex = ((compareIndex + checkBit)%matchStringBitSize)>>3;
                currPrefBitIndex = ((compareIndex + checkBit) % 8);

                if (first_loop_run) {
                    first_loop_run = 0;
                    bitDiff = 8 - currBuffBitIndex;
                }

                // adjust for misaligned bits

                if ((currBuffBitIndex > 7 - bitDiff)) {

                    currBuffByteIndex += 2;

                }


                static uint8_t buffBit;
                static uint8_t prefBit;

                // isolate the bit in the string
                buffBit = receiverBuffer[currBuffByteIndex] & (0x1<<currBuffBitIndex);
                buffBit = buffBit >> currBuffBitIndex;
                // isolate the corresponding bit in the prefix
                prefBit = matchString[currPrefByteIndex] & (0x1<<currPrefBitIndex);
                prefBit = prefBit >> currPrefBitIndex;

//                printf("\n");
//                printf("----BUFFER BYTE----\n");
//                printBinaryWPointer(receiverBuffer[currBuffByteIndex], currBuffBitIndex);
//                printf("\n----PREFIX STRING BYTE----\n");
//                printBinaryWPointer(matchString[currPrefByteIndex], currPrefBitIndex);
//                printf("\n");

//                static int newline = 1;
//
//                printf("B%dP%d ",buffBit,prefBit);
//
//                if(newline == 0) {
//                    printf("\n");
//                }
//                newline = (newline + 1) % 8;

                // comparte buffer bit with corresponding prefix bit
                // if match stayed = 1 for the entire length of the segemnt
                // being compared (usually 32bits) then the prefix is said to be found
                //
                if(buffBit != prefBit) {
                    //printf("NOT A MATCH\n\n" );
                    match = 0;
                    break;
                }
            }
            if (match) {
                // return the index for the start of the payload
                static int32_t bufferMasterBitIndex;
                static int32_t payloadStartIndex;
                static int32_t payloadEndIndex;

                bufferMasterBitIndex 	= (bufferBitIndex + (bufferByteIndex * 8)) % bufferBitSize;
                payloadStartIndex 	= (buffIndex + matchStringBitSize - compareIndex) % bufferBitSize;
                payloadEndIndex 		=  (payloadStartIndex + 256) % bufferBitSize; // payload is 32 bytes long


                static int32_t continuousPayload;
                continuousPayload = 1;
                if (payloadStartIndex > payloadEndIndex) {
                    continuousPayload = 0;
                }

                //printf("ContinuousPayload\t%d \nbufferMasterBitIndex\t%d \nPayloadStartIndex\t%d \npayloadEndIndex\t%d\n",continuousPayload,bufferMasterBitIndex,payloadStartIndex,payloadEndIndex);

                // make sure the buffer is not currently writing the payload into the buffer still
                if ((continuousPayload && ((bufferMasterBitIndex < payloadStartIndex) || (bufferMasterBitIndex >= payloadEndIndex))) ||
                    (!continuousPayload && ((bufferMasterBitIndex >= payloadEndIndex) && (bufferMasterBitIndex < payloadStartIndex)))) {
                    return payloadStartIndex; // return exact index in buffer where payload begins
                }
            }

        }
    }

    return -1;
}


void makeSubString(uint8_t *subString, uint32_t startingDatBitIndex, uint8_t *dataArray, uint32_t dataArraySize, uint32_t subStringSize) {
    //printf("\nstartingDatBitIndex: %d\n dataArraySize: %d\n subStringSize: %d\n",startingDatBitIndex,dataArraySize,subStringSize );
    //printf("----subString source array---\n");
    //printArray(dataArray, dataArraySize);
    //printf("realloc size: %d",subStringSize * sizeof(dataArray[0]));
//    *subString = realloc((*subString), subStringSize * sizeof(dataArray[0]));
    //memset(*subString, 0, subStringSize * sizeof(dataArray[0]));

    static int32_t dataArrayBitSize;
    dataArrayBitSize = dataArraySize << 3;
    static int32_t subStringBitSize;
    subStringBitSize = subStringSize << 3;
    static int32_t first_loop_run;
    first_loop_run = 1;

    static int32_t bitDiff;


    static int32_t bitIndex;
    for (bitIndex=0; bitIndex < subStringBitSize; bitIndex++) {
        static int32_t currDatByteIndex;
        static int32_t currDatBitIndex;
        static int32_t currStrByteIndex;
        static int32_t currStrBitIndex;

        currDatByteIndex = ((startingDatBitIndex + bitIndex)%dataArrayBitSize)>>3;
        currDatBitIndex = (startingDatBitIndex + bitIndex) % 8;// % 8;
        currStrByteIndex = bitIndex >> 3;
        currStrBitIndex = bitIndex % 8;

        if (first_loop_run) {
            first_loop_run = 0;
            bitDiff = 8 - currDatBitIndex;
        }

        // adjust for misaligned bits
        if ((currDatBitIndex > 7 - bitDiff)) {
            currDatByteIndex += 2;
        }

        //printf("\ncurrDatByteIndex: %d\ncurrDatBitIndex: %d\ncurrStrByteIndex: %d\ncurrStrBitIndex: %d\n",currDatByteIndex,currDatBitIndex,currStrByteIndex,currStrBitIndex );

        static uint8_t tempStrBit;

        tempStrBit = (dataArray[currDatByteIndex] >> currDatBitIndex) & 0x1;
        subString[currStrByteIndex] =  subString[currStrByteIndex] | (tempStrBit << currStrBitIndex);

        if (currStrBitIndex==6 && currDatByteIndex==0) {
            int32_t i;
            i = 7;
        }
    }
}

char* makeSubStringChar(uint32_t startingDatBitIndex, uint8_t *dataArray, uint32_t dataArraySize, uint32_t subStringSize) {
	uint8_t *subString;
	subString = malloc(subStringSize * sizeof(dataArray[0]));
    memset(subString, 0, subStringSize * sizeof(dataArray[0]));
	makeSubString(subString, startingDatBitIndex,dataArray, dataArraySize, subStringSize);
	char * temp;
	temp = malloc(subStringSize + 1);
    memset(temp, 0, subStringSize + 1);

	if(subString[subStringSize-1] != '\0') {
		//free(*subString);
		temp[subStringSize] = '\0';
		//*subString = temp;
	} else {
		temp = malloc(subStringSize + 1);
	}
	memcpy(temp,subString,subStringSize);
	free(subString);
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
//    printf("ARRAY: ");
    static int32_t row = 1;
    row = 1;
    for (int i = 0; i < dataSize; i++) {
        printBinary(data[i]);
        printf(" ");
        if (row == 0) {
            printf("\n");
        }
        row = (row +1) % 8;
    }
}

void printArrayBinMSB(uint8_t *data, uint32_t dataSize) {
    //    printf("ARRAY: ");
    static int32_t row = 1;
    row = 1;
    for (int i = dataSize-1; i >= 0; i--) {
        printBinary(data[i]);
        printf(" ");
        if (row == 0) {
            printf("\n");
        }
        row = (row +1) % 8;
    }
}
