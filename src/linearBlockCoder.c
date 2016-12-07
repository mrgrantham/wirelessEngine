//
//  linearBlockCoder.c
//  receiverModule
//
//  Created by DEV on 11/16/16.
//  Copyright © 2016 DEV. All rights reserved.
//

#include "linearBlockCoder.h"
//#include "memory.h"
#include <stdlib.h>
#include "helperFunctions.h"

#define P_MATRIX_WIDTH 4
#define calcIndex(row,col)   (((row) * P_MATRIX_WIDTH) + (col))
#define calcIndexW(row,col,width)   (((row) * (width)) + (col))


//static uint8_t *linearIndMatrix     = NULL;
static uint8_t *parityCheckMatrix   = NULL;
static uint8_t *generatorMatrix     = NULL;
static int32_t aCodeLength;

uint8_t linearIndMatrix[] = {
     1, 0, 0, 0 ,
     0, 1, 0, 0 ,
     0, 0, 1, 0 ,
     0, 0, 0, 1 ,
     1, 0, 1, 0 ,
     0, 1, 1, 0 ,
     1, 1, 1, 1 ,
     0, 0, 1, 1 };

void initLinearBlockCoder(int32_t codeLength){
//    if (linearIndMatrix != NULL) {
//        free(linearIndMatrix);
//    }
//    linearIndMatrix     = malloc(codeLength * P_MATRIX_WIDTH * sizeof(linearIndMatrix[0]));
    generatorMatrix     = malloc(codeLength * (codeLength + P_MATRIX_WIDTH) * sizeof(generatorMatrix[0]));
    parityCheckMatrix   = malloc(P_MATRIX_WIDTH * (codeLength + P_MATRIX_WIDTH) * sizeof(parityCheckMatrix[0]));

//    memset(linearIndMatrix,     0, codeLength * P_MATRIX_WIDTH * sizeof(  linearIndMatrix[0]));
    memset(generatorMatrix,     0, codeLength * (codeLength + P_MATRIX_WIDTH) * sizeof(  generatorMatrix[0]));
    memset(parityCheckMatrix,   0, P_MATRIX_WIDTH * (codeLength + P_MATRIX_WIDTH) * sizeof(parityCheckMatrix[0]));

    aCodeLength = codeLength;
//    generateLinearIndependance(codeLength);
    generateGenerator(codeLength);
    generateParityCheck(codeLength);
}

void chainEncoding(char *sourceString, char *encodedString,int sourceStringLength,int encodeStringLength){
    uint8_t matrix[8];
    uint8_t *encodedMatrix = malloc(encodeStringLength * 12 * sizeof(uint8_t));
    memset(encodedMatrix, 0, encodeStringLength * 12 * sizeof(uint8_t));

    for(int i = 0; i < sourceStringLength; i++) {
        memset(matrix, 0, 8);
        convertTo1DMatrix(&sourceString[i], matrix, 8);
        encode(matrix, &encodedMatrix[i*12], 8, 12);
    }

    convertToString(encodedString, encodedMatrix, encodeStringLength);
    free(encodedMatrix);
}

void chainDecoding(char *sourceString, char *decodedString,int sourceStringLength,int decodeStringLength){
    uint8_t matrix[12];
    uint8_t *decodedMatrix = malloc(decodeStringLength * 12 * sizeof(uint8_t));
    memset(decodedMatrix, 0, decodeStringLength * 12 * sizeof(uint8_t));
    for(int i = 0; i < decodeStringLength; i++) {
        memset(matrix, 0, 12);
        int32_t decodeByteIndex = i*3/2;
        convertTo1DMatrixStartBit(&sourceString[decodeByteIndex], matrix, 12,(i%2)*4);
        decode(matrix, &decodedMatrix[i*8], 12, 8);
    }

    convertToString(decodedString, decodedMatrix, decodeStringLength);
    free(decodedMatrix);
}

void encode(uint8_t * messageSource, uint8_t *messageCoded, int32_t messageBitLength,int32_t codedMessageBitLength){
    matrixMultiply(messageSource, generatorMatrix, messageCoded, 1, messageBitLength, messageBitLength, codedMessageBitLength);
}

void decode(uint8_t * messageSource, uint8_t *messageDecoded, int32_t codedMessageBitLength,int32_t decodedMessageBitLength){
    static uint8_t *syndromeVector;
    syndromeVector = malloc(P_MATRIX_WIDTH * sizeof(uint8_t));
    memset(syndromeVector, 0, P_MATRIX_WIDTH);
    matrixMultiply(messageSource, parityCheckMatrix, syndromeVector, 1, codedMessageBitLength, codedMessageBitLength, P_MATRIX_WIDTH);
    static int32_t sMatch;
    sMatch = syndromeMatch(syndromeVector, decodedMessageBitLength);
    memcpy(messageDecoded, messageSource, decodedMessageBitLength);
    if(sMatch > -1) {
        messageDecoded[sMatch] = (~(messageDecoded[sMatch]))&0x01;
//        printMatrix(messageDecoded, decodedMessageBitLength, 1);
//        printf("\n");

    } else {
//        printf("NO SYNDROME MATCH\n");
    }
//    printf("SYNDROM WORD        ");
//    printMatrix(syndromeVector, P_MATRIX_WIDTH, 1);
    free(syndromeVector);
}

void convertTo1DMatrix(char *aString, uint8_t *matrix, int32_t bitLength){
    
    for (int32_t i = 0; i < bitLength; i++) {
        uint8_t theBit = getBit(aString[i>>3], (7-i)%8);
        matrix[i] = theBit;
    }
}

void convertTo1DMatrixStartBit(char *aString, uint8_t *matrix, int32_t bitLength,int32_t startBitIndex){

    for (int32_t i = 0; i < bitLength; i++) {
        uint8_t theBit = getBit(aString[(startBitIndex+i)>>3], (7-(startBitIndex+i)%8));
        matrix[i] = theBit;
    }
}

void convertToString(char *aString, uint8_t *matrix, int32_t length){
//    printMatrix(matrix, (length<<3), 1);
    for (int32_t i = 0; i < (length<<3); i++) {
        setBit((uint8_t*)&aString[i>>3], 7-(i%8), matrix[i]);
    }
}
void addErrorStrIndex(char *code, int32_t messageLength,int32_t errorIndex){
    uint8_t *codeuint8;
    int32_t codeLength = (messageLength<<3) + P_MATRIX_WIDTH;
    codeuint8 = malloc(codeLength * sizeof(uint8_t));
    convertTo1DMatrix(code, codeuint8, messageLength);
    addErrorIndex(codeuint8, codeLength, errorIndex);
    convertToString(code, codeuint8, messageLength);
    free(codeuint8);

}

void addErrorsStr(char *code, int32_t messageLength, int32_t numErrors){
    uint8_t *codeuint8;
    int32_t codeLength = (messageLength<<3) + P_MATRIX_WIDTH;
    codeuint8 = malloc(codeLength * sizeof(uint8_t));
    convertTo1DMatrix(code, codeuint8, codeLength);
    addErrors(codeuint8, codeLength, numErrors);
    convertToString(code, codeuint8, codeLength);
    free(codeuint8);
}

void checkParityGenerator() {
    uint8_t *productMatrix;
    productMatrix = malloc((aCodeLength + P_MATRIX_WIDTH) * (aCodeLength + P_MATRIX_WIDTH) * sizeof(uint8_t));
    memset(productMatrix, 0, (aCodeLength + P_MATRIX_WIDTH) * (aCodeLength + P_MATRIX_WIDTH) * sizeof(uint8_t));
    matrixMultiply(generatorMatrix, parityCheckMatrix, productMatrix, aCodeLength, aCodeLength + P_MATRIX_WIDTH, aCodeLength + P_MATRIX_WIDTH, P_MATRIX_WIDTH);
//    printMatrix(productMatrix, aCodeLength + P_MATRIX_WIDTH, aCodeLength + P_MATRIX_WIDTH);
}

void matrixMultiply(uint8_t *matrix1, uint8_t *matrix2, uint8_t *productMatrix, int32_t row1, int32_t col1, int32_t row2, int32_t col2) {

    int32_t i, j, k;

    // Multiplying matrix firstMatrix and secondMatrix and storing in array mult.
    for(i = 0; i < row1; ++i)
    {
        for(j = 0; j < col2; ++j)	
        {
            for(k=0; k < col1; ++k)
            {
                int productIndex = calcIndexW(i, j, row2);
                int mat1 = calcIndexW(i, k, col1);
                int mat2 = calcIndexW(k, j, col2);
                uint8_t mat1val = matrix1[mat1];
                uint8_t mat2val = matrix2[mat2];
                uint8_t productVal = productMatrix[productIndex];
                productVal ^= mat1val * mat2val;
                productMatrix[productIndex] = productVal;
            }
//            printMatrix(productMatrix, col2, row1);

        }
    }
}


void printLinearIndependentMatrix(){
    printMatrix(linearIndMatrix, P_MATRIX_WIDTH, aCodeLength);
}

void printParityCheckMatrix() {
    printMatrix(parityCheckMatrix, P_MATRIX_WIDTH, aCodeLength + P_MATRIX_WIDTH);
}

void printGeneratorMatrix() {
    printMatrix(generatorMatrix, aCodeLength + P_MATRIX_WIDTH, aCodeLength);
}

void printMatrix(uint8_t *matrix, int32_t matrixWidth, int32_t matrixLength){
    for (int i = 0; i < matrixLength; i++) {
        printf("| ");
        for (int j = 0; j < matrixWidth; j++) {
            printf("%0.1d", matrix[calcIndexW(i,j,matrixWidth)]);
            if ((j%8==7) || (j == matrixWidth-1)) {
                printf(" ");
            }
        }
        printf("|");
        if(i < matrixLength-1) {
            printf("\n");
        }
    }
}

void generateLinearIndependance(int32_t codeLength){
    static int32_t doesMatch = 0;
    static uint8_t *sum;
    sum = malloc(codeLength * sizeof(uint8_t));
    
    // progress through generation
    for (int iteration = 0; iteration < codeLength; iteration++) {
        doesMatch = 0;
        do {
            for (int row1 = 0; row1 < iteration; row1++) {
                // check if the string is an outright duplicate of another line
                if (!stringMatch(&linearIndMatrix[calcIndex(iteration, 0)],&linearIndMatrix[calcIndex(row1, 0)] , P_MATRIX_WIDTH)) {
                    doesMatch = 0;
                    for (int row2 = row1+1; row2 < iteration; row2++) {
                        // check if the row is a sum of any of the other lines

                        mod2add(sum, &linearIndMatrix[calcIndex(row1, 0)], &linearIndMatrix[calcIndex(row2, 0)], P_MATRIX_WIDTH);
                        doesMatch |= stringMatch(&linearIndMatrix[calcIndex(iteration, 0)], sum, P_MATRIX_WIDTH);
                        
                        if (doesMatch) break;
                    }
                    if (doesMatch) break;
                } else {
                    doesMatch = 1;
                    break;
                }
            }
            if (doesMatch) {
                incrementArrayValue(&linearIndMatrix[calcIndex(iteration, 0)], P_MATRIX_WIDTH);
            }
        } while (doesMatch);

    }
    free(sum);
}

int8_t stringMatch(uint8_t *str1, uint8_t *str2, int32_t strLength){
    static int8_t match = 0;
    match = 0;
    
    for (int32_t i = 0; i < strLength; i++) {
        if (str1[i] != str2[i]) {
            return 0;
        }
    }
    
    return 1;
}

void incrementArrayValue(uint8_t *strArr, int32_t strLength) {
    static int8_t carry;
    carry = 1;
    
    for (int i = 0 ; i < strLength; i++) {
        static int8_t temp;
        temp = strArr[i] ^ carry;
        carry = strArr[i] & carry;
        strArr[i] = temp;
    }
}

void mod2add(uint8_t *sum, uint8_t *str1, uint8_t *str2, int32_t strLength){

    for (int32_t i = 0; i < strLength; i++) {
        sum[i] = str1[i] ^ str2[i];
    }
}



void addError(uint8_t *code, int32_t codeLength) {
    int32_t codeIndex = rand() % codeLength;
    code[codeIndex] = ~code[codeIndex] & 0x01;
}
void addErrorIndex(uint8_t *code, int32_t codeLength, int32_t errorIndex) {
    code[errorIndex] = ~code[errorIndex] & 0x01;
}

void addErrors(uint8_t *code, int32_t codeLength, int32_t numErrors) {
    for (int i =0; i < numErrors; i++) {
        addError(code, codeLength);
    }
}

void generateIdentityMatrix(uint8_t *dest,int32_t width){
    for (int i = 0; i < width; i++) {
        dest[calcIndexW(i, i, width)] = 1;
    }
}

void generateGenerator(int32_t codeLength) {
    uint8_t *tempIdentity = malloc(codeLength * codeLength * sizeof(tempIdentity[0]));
    memset(tempIdentity, 0, codeLength * codeLength * sizeof(tempIdentity[0]));
    generateIdentityMatrix(tempIdentity, codeLength);
//    printMatrix(tempIdentity, codeLength, codeLength);
    for (int i = 0; i < codeLength; i++) {
        for (int j = 0; j < codeLength + P_MATRIX_WIDTH; j++) {
            if (j < codeLength) {
                generatorMatrix[calcIndexW(i, j,codeLength + P_MATRIX_WIDTH)] = tempIdentity[calcIndexW(i, j,codeLength)];
            } else {
                generatorMatrix[calcIndexW(i, j,codeLength + P_MATRIX_WIDTH)] = linearIndMatrix[calcIndex(i, j-codeLength)];
            }
        }
    }
    free(tempIdentity);
}

void generateParityCheck(int32_t codeLength) {
    uint8_t *tempIdentity = malloc(codeLength * codeLength * sizeof(tempIdentity[0]));
    memset(tempIdentity, 0, P_MATRIX_WIDTH * P_MATRIX_WIDTH * sizeof(tempIdentity[0]));
    generateIdentityMatrix(tempIdentity, P_MATRIX_WIDTH);
//    printMatrix(tempIdentity, P_MATRIX_WIDTH, P_MATRIX_WIDTH);
    for (int i = 0; i < codeLength + P_MATRIX_WIDTH; i++) {
        for (int j = 0; j < P_MATRIX_WIDTH; j++) {
            if (i < codeLength) {
                parityCheckMatrix[calcIndex(i, j)] = linearIndMatrix[calcIndex(i, j)];
            } else {
                parityCheckMatrix[calcIndex(i, j)] = tempIdentity[calcIndex(i-codeLength, j)];
            }
        }
    }
    free(tempIdentity);
}

int32_t syndromeMatch(uint8_t *sWord,int32_t sLength){
    
    static int8_t sMatch;
    for (int32_t row = 0; row < sLength; row++) {
        sMatch = 1;
        for (int32_t col = 0; col < P_MATRIX_WIDTH; col++) {
            if (linearIndMatrix[calcIndexW(row, col, P_MATRIX_WIDTH)] != sWord[col]) {
                sMatch = 0;
                break;
            }
        }
        if (sMatch == 1) {
            return row;
        }
    }
    
    return -1;
}
