//
//  scrambler.c
//  receiverModule
//
//  Created by DEV on 11/6/16.
//  Copyright © 2016 DEV. All rights reserved.
//

#include "stdlib.h"
#include "string.h"
#include "queue.h"
#include "scrambler.h"
#include "LISAreceiver.h"


void scrambleShiftRegisterReset(){
	memset(scramblerRegister,0,scramblerRegisterSize);

}
void descrambleShiftRegisterReset(){
	memset(descramblerRegister,0,descramblerRegisterSize);
}


void scrambleStep(uint8_t *dest, uint8_t *source,int32_t size, uint8_t reset){

    static int32_t bitSize;
    bitSize = size<<3;
    
    static int32_t scrambleIndex;
//    if(reset) {
//        scrambleIndex = bitSize - 1;
//    } else if (scrambleIndex!=0){
//        scrambleIndex--;
//    }
    if(reset) {
        scrambleIndex = 0;
    } else if (scrambleIndex!=bitSize - 1){
        scrambleIndex++;
    }

    // perform scramble on this bit based on register size
    static int32_t endIndex;
    static int32_t midIndex;
    
    static uint8_t endBit;
    static uint8_t midBit;
    
    static uint8_t sourceEndBit;
    static uint8_t scrambledBit;
    
    
    endIndex = scramblerRegisterBitSize - 1;
    midIndex = scramblerRegisterBitSize/2;

    // get values for from mid and end locations in bit register
    endBit = getBit(scramblerRegister[endIndex>>3],endIndex%8);
    midBit = getBit(scramblerRegister[midIndex>>3], midIndex%8);
    
    // get value for source bit about to go into shift register
    sourceEndBit = getBit(source[scrambleIndex>>3],(7-(scrambleIndex%8)));
    
    // perform XOR on bit thats about to enter the register
    scrambledBit = sourceEndBit ^ (endBit ^ midBit);
    
    dest[scrambleIndex>>3] |= scrambledBit<<(7-(scrambleIndex%8));
    
    // get scrambled bit from and shift in
    scramblerShiftIn(scrambledBit);

}

void scramble(uint8_t *dest, uint8_t *source,int32_t sourceLength){
    static int32_t sourceBitLength;
    sourceBitLength = sourceLength << 3;
//    printf("\n[INIT] SOURC STRING: ");
//    printArrayBin(source, sourceLength);
//    printf("\nSHIFT REGIST: ");
//    printArrayBin(scramblerRegister, scramblerRegisterSize);
//    printf("\n[INIT] SCRBL STRING: ");
//    printArrayBin(dest, sourceLength);
//    printf("\n");
    for (int32_t i = sourceBitLength-1; i >= 0; i--) {

        if (i == sourceBitLength - 1) {
            scrambleStep(dest, source, sourceLength, 1);
        } else {
            scrambleStep(dest, source, sourceLength, 0);
        }
//        printf("\n[%d] SOURC STRING: ",i);
//        printArrayBin(source, sourceLength);
//        printf("\nSHIFT REGIST: ");
//        printArrayBin(scramblerRegister, scramblerRegisterSize);
//        printf("\n[%d] SCRBL STRING: ",i);
//        printArrayBin(dest, sourceLength);
//        printf("\n");
    }
}

void descramble(uint8_t *dest, uint8_t *source,int32_t sourceLength){
    static int32_t sourceBitLength;
    sourceBitLength = sourceLength << 3;
    
    for (int32_t i = 0; i < sourceBitLength; i++) {
//        printf("\nSCRBL STRING:");
//        printArrayBin(source, sourceLength);
//        printf("\nSHIFT REGIST:");
//        printArrayBin(descramblerRegister, descramblerRegisterSize);
//        printf("\nDCRBL STRING:");
//        printArrayBin(dest, sourceLength);
//        printf("\n");
        if (i == 0) {
            descrambleStep(dest, source, sourceLength, 1);
        } else {
            descrambleStep(dest, source, sourceLength, 0);
        }
    }
//    printf("\nSCRBL STRING:");
//    printArrayBin(source, sourceLength);
//    printf("\nSHIFT REGIST:");
//    printArrayBin(descramblerRegister, descramblerRegisterSize);
//    printf("\nDCRBL STRING:");
//    printArrayBin(dest, sourceLength);
//    printf("\n");

}

void descrambleStep(uint8_t *dest, uint8_t *source,int32_t size, uint8_t reset){
    static int32_t bitSize;
    bitSize = size<<3;
    
    static int32_t descrambleIndex;
    
//    if(reset) {
//        descrambleIndex = bitSize;
//    } else if (descrambleIndex!=0){
//        descrambleIndex--;
//    }

    if(reset) {
        descrambleIndex = 0;
    } else if (descrambleIndex!=bitSize - 1){
        descrambleIndex++;
    }
    
    // perform descramble on this bit based on register size
    static int32_t endIndex;
    static int32_t midIndex;
    static uint8_t endBit;
    static uint8_t midBit;
    static uint8_t sourceEndBit;
    static uint8_t descrambledBit;
    
    
    endIndex = descramblerRegisterBitSize - 1;
    midIndex = descramblerRegisterBitSize/2;
    // get values for from mid and end locations in bit register
    endBit = getBit(descramblerRegister[endIndex>>3],endIndex%8);
    midBit = getBit(descramblerRegister[midIndex>>3], midIndex%8);
    // get value for source bit about to go into shift register
    sourceEndBit = getBit(source[descrambleIndex>>3],7-(descrambleIndex%8));
    // perform XOR on bit thats about to enter the register
    descrambledBit = sourceEndBit ^ (endBit ^ midBit);
    
    dest[descrambleIndex>>3] |= descrambledBit<<(7-(descrambleIndex%8));
    // test algorithm without scramble
//    dest[descrambleIndex>>3] |= sourceEndBit<<(descrambleIndex%8);
    
    // get bit from source and shift in
    descramblerShiftIn(sourceEndBit);

}


void scrambleShiftRegisterInit(int32_t order, uint8_t startingBit){
    scramblerRegister = malloc(((order>>3)+1) * sizeof(scramblerRegister[0]));
    memset(scramblerRegister, startingBit, ((order>>3)+1) * sizeof(scramblerRegister[0]));
    scramblerRegisterSize = (order>>3)+1;
    scramblerEndBitIndex = order%8;
    scramblerRegisterBitSize = order;
}


void descrambleShiftRegisterInit(int32_t order, uint8_t startingBit){
    descramblerRegister = malloc(((order>>3)+1) * sizeof(descramblerRegister[0]));
    memset(descramblerRegister, startingBit, ((order>>3)+1) * sizeof(descramblerRegister[0]));
    descramblerRegisterSize = (order>>3)+1;
    descramblerEndBitIndex = order%8;
    descramblerRegisterBitSize = order;
}

void scramblerShiftIn(uint8_t sourceBit){

    shiftInArray(scramblerRegister, scramblerRegisterSize, sourceBit);
}

void descramblerShiftIn(uint8_t sourceBit){
    shiftInArray(descramblerRegister,descramblerRegisterSize, sourceBit);
}

void shiftInArray(uint8_t *array, int32_t size, uint8_t shiftInBit) {
    
    shiftInBit &= 0x1; // mask the first bit
    for (int32_t i = size-1; i >= 0; i--) {
        array[i] <<= 1;
        if (i != 0) {
            array[i] |= getBit(array[i-1], 7);
        } else {
            array[i] |= shiftInBit;
        }
    }
    
}

uint8_t * getScramblerRegister(){
    return scramblerRegister;
}

int32_t getScramblerRegisterSize(){
    return scramblerRegisterSize;
}

uint8_t * getDescramblerRegister(){
    return descramblerRegister;
}

int32_t getDescramblerRegisterSize(){
    return descramblerRegisterSize;
}


