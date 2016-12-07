//
//  linearBlockCoder.h
//  receiverModule
//
//  Created by DEV on 11/16/16.
//  Copyright © 2016 DEV. All rights reserved.
//

#ifndef linearBlockCoder_h
#define linearBlockCoder_h

#include <stdio.h>

void initLinearBlockCoder(int32_t codeLength);
void chainEncoding(char *sourceString, char *encodedString,int sourceStringLength,int encodeStringLength);
void chainDecoding(char *sourceString, char *decodedString,int sourceStringLength,int decodeStringLength);
void encode(uint8_t * messageSource, uint8_t *messageCoded, int32_t messageBitLength,int32_t codedMessageBitLength);
void decode(uint8_t * messageSource, uint8_t *messageDecoded, int32_t messageBitLength,int32_t decodedMessageBitLength);
void printParityCheckMatrix();
void printGeneratorMatrix();
void printLinearIndependentMatrix();
void generateLinearIndependance(int32_t codeBitLength);
void incrementArrayValue(uint8_t *strArr, int32_t strBitLength);
int8_t stringMatch(uint8_t *str1, uint8_t *str2, int32_t strBitLength);
void mod2add(uint8_t *sum, uint8_t *str1, uint8_t *str2, int32_t strLength);
void addError(uint8_t *code, int32_t codeLength);
void addErrors(uint8_t *code, int32_t codeLength, int32_t numErrors);
void addErrorIndex(uint8_t *code, int32_t codeLength, int32_t errorIndex);
void addErrorsStr(char *code, int32_t codeLength, int32_t numErrors);
void addErrorStrIndex(char *code, int32_t messageLength,int32_t errorIndex);
void generateIdentityMatrix(uint8_t *dest,int32_t width);
void generateGenerator(int32_t codeLength);
void generateParityCheck(int32_t codeLength);
void printMatrix(uint8_t *matrix, int32_t matrixWidth, int32_t matrixLength);
void matrixMultiply(uint8_t *matrix1, uint8_t *matrix2, uint8_t *productMatrix, int32_t row1, int32_t col1, int32_t row2, int32_t col2);
void convertToString(char *aString, uint8_t *matrix, int32_t length);
void convertTo1DMatrix(char *aString, uint8_t *matrix, int32_t length);
void convertTo1DMatrixStartBit(char *aString, uint8_t *matrix, int32_t bitLength,int32_t startBitIndex);
void checkParityGenerator();
int32_t syndromeMatch(uint8_t *sWord,int32_t sLength);

#endif /* linearBlockCoder_h */
