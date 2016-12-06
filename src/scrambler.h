//
//  scrambler.h
//  receiverModule
//
//  Created by DEV on 11/6/16.
//  Copyright © 2016 DEV. All rights reserved.
//

#ifndef scrambler_h
#define scrambler_h

#include <stdio.h>

uint8_t * scramblerRegister;
int32_t scramblerRegisterSize;
int32_t scramblerEndBitIndex;
int32_t scramblerRegisterBitSize;
uint8_t * descramblerRegister;
int32_t descramblerRegisterSize;
int32_t descramblerRegisterBitSize;
int32_t descramblerEndBitIndex;
int32_t descramblerBitSize;



void scramble(uint8_t *dest, uint8_t *source,int32_t sourceLength);
void scrambleStep(uint8_t *dest, uint8_t *source,int32_t size, uint8_t reset);

void descramble(uint8_t *dest, uint8_t *source,int32_t sourceLength);
void descrambleStep(uint8_t *dest, uint8_t *source,int32_t size, uint8_t reset);

void scrambleShiftRegisterInit(int32_t order, uint8_t startingBit);
void descrambleShiftRegisterInit(int32_t order, uint8_t startingBit);

void scrambleShiftRegisterReset();
void descrambleShiftRegisterReset();


void scramblerShiftIn(uint8_t sourceBit);
void descramblerShiftIn(uint8_t sourceBit);
void shiftInArray(uint8_t *array, int32_t size, uint8_t shiftInBit);

uint8_t * getScramblerRegister();
int32_t getScramblerRegisterSize();
uint8_t * getDescramblerRegister();
int32_t getDescramblerRegisterSize();


#endif /* scrambler_h */
