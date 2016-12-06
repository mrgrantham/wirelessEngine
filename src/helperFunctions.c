//
//  helperFunctions.c
//  receiverModule
//
//  Created by DEV on 11/7/16.
//  Copyright © 2016 DEV. All rights reserved.
//

#include "helperFunctions.h"

int8_t getBit(uint8_t source, int8_t index){
    return (source >> index) & 0x1;
}

void setBit(uint8_t *source, int8_t index, int8_t bit){
    source[0] = (source[0] & ~(1<<index)) | (bit<<index);
}


void printBinary(uint8_t num) {
    //printf("0b");
    uint8_t temp = 0;
    for (int8_t i=7;i>=0;i--) {
        temp=num;
        temp>>=i;
        temp &= 0x01;
        printf("%d", temp);
    }
}

void printBinaryWPointer(uint8_t num,int32_t index) {
    printBinary(num);
    printf("\n");
    
    if(index < 8) {
        for (int i = 0 ; i < 7-index; i++) {
            printf(" ");
        }
        printf("^\n");
    } else {
        printf("index: %d exceeds limit\n",index);
    }
}
