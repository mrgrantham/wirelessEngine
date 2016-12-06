//
//  helperFunctions.h
//  receiverModule
//
//  Created by DEV on 11/7/16.
//  Copyright © 2016 DEV. All rights reserved.
//

#ifndef helperFunctions_h
#define helperFunctions_h

#include <stdio.h>

int8_t getBit(uint8_t source, int8_t index);
void setBit(uint8_t *source, int8_t index, int8_t bit);
void printBinary(uint8_t num);
void printBinaryWPointer(uint8_t num,int32_t index);

#endif /* helperFunctions_h */
