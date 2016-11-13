/*
===============================================================================
 Name        : Engine.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

//#include "scrambler.h"

#include "queue.h"
#include "LISAtransmitter.h"
#include "LISAreceiver.h"
#include "stdlib.h"
#include "string.h"


#include <cr_section_macros.h>

#define SCRAMBLER_ORDER 7

 #define TICKRATE0_HZ 2000 // 200 bps
#define TICKRATE1_HZ 20
 #define TICKRATE2_HZ 200 // 9600 bps

 #define BLINKS_PER_SEC 5
 #define BLINK_DELAY (TICKRATE0_HZ/BLINKS_PER_SEC)

#define TX_PORT 0
#define TX_PIN 9

#define RX_PORT 0
#define RX_PIN 8

#define TEST_SWITCH_PORT 2
#define TEST_SWITCH_PIN 10

#define EXTERNAL_GREEN_LED_PORT 2
#define EXTERNAL_GREEN_LED_PIN 13

#define RECEIVER_BUFFER_SIZE 128
#define PAYLOAD_SIZE 32

#define TRANSMIT_ENABLED
#define RECEIVE_ENABLED
#define SCRAMBLE_ENABLED
#define SWITCH_TRANSMIT_ENABLED
#define ACK_MODE_ENABLED

uint8_t receiverBuffer[RECEIVER_BUFFER_SIZE];
static uint16_t packetCheckCounter = 0; // used to determine if enough bits have been received to check the buffer again


int sendCount=0;
int loopCount=0;

//char *payloadString="JAMES 6912";
char *payloadString="JAJAMES6912";

uint8_t *packet=NULL;
uint32_t packetSize=0;

int firstscram = 1;

void testTransmit() {


#ifdef SCRAMBLE_ENABLED

	scrambleShiftRegisterReset();

	char * payloadStringScrambled = malloc((strlen(payloadString)+1) * sizeof(char));
	memset(payloadStringScrambled,0,(strlen(payloadString)+1) * sizeof(char));

	scramble(payloadStringScrambled, payloadString, strlen(payloadString)+1);
//	printf("%s\n",payloadString);
	if (firstscram) {
		printf("SCRAM: %s\n",payloadStringScrambled);
//		printf("SCRAM BIN: ");
//		printArrayBin(payloadStringScrambled,11);
//		printf("\n");
		firstscram = 0;
	}
	composePacket(payloadStringScrambled, &packet, &packetSize);
	//printReadyPacket(packet, 64);
	free(payloadStringScrambled);

#else
	composePacket(payloadString, &packet, &packetSize);
#endif


	sendPacket(packet,packetSize);
	deletePacket(&packet,&packetSize);
}

// Transmit and Receive Test
static int32_t receive_enabled = 1;
static int32_t transmit_enabled = 0;

void TIMER0_IRQHandler(void)
{

	if (Chip_TIMER_MatchPending(LPC_TIMER0, 1)) {
		Chip_TIMER_ClearMatch(LPC_TIMER0, 1);
	}
	//printf("%d",val);

	static int counter = 0;

	// broadcast on antenna
#ifdef ACK_MODE_ENABLED
	if(!receive_enabled && transmit_enabled) {
		sendBit();
		if (getQueueSize() == 0) {
			receive_enabled = 1;
			transmit_enabled = 0;
		}
	}
	if(receive_enabled && !transmit_enabled) {
		sendToBuffer(readBit(),receiverBuffer);
	}

#else
	sendBit();

	sendToBuffer(readBit(),receiverBuffer);
#endif
	// listen to receiver

	packetCheckCounter = (packetCheckCounter+1)%(RECEIVER_BUFFER_SIZE<<2);

	if (counter == BLINK_DELAY) {
		Board_LED_Toggle(2);
		counter = 0;
	} else {
		counter++;
	}

}

void EINT3_IRQHandler(void) {
	static uint32_t counter=10;
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, RX_PORT, 1 << RX_PIN);
	//Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, TEST_SWITCH_PORT, 1 << TEST_SWITCH_PIN);
	int blinkslower= counter%10;
    if(blinkslower==0) {
		Chip_GPIO_SetPinToggle(LPC_GPIO, EXTERNAL_GREEN_LED_PORT , EXTERNAL_GREEN_LED_PIN );
    }
	/* Enable timer interrupt */
	/* Enable timer 1 clock */
	Chip_TIMER_Init(LPC_TIMER2);

	/* Timer rate is system clock rate */
	uint32_t timerFreq = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER2);
	printf("TIMER FREQUENCY %d\n",timerFreq);
	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset(LPC_TIMER2);
	Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);
	Chip_TIMER_SetMatch(LPC_TIMER2, 1, (timerFreq / TICKRATE2_HZ));
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 1);
	Chip_TIMER_Enable(LPC_TIMER2);
	NVIC_ClearPendingIRQ(TIMER2_IRQn);
	NVIC_EnableIRQ(TIMER2_IRQn);

    printf("EINT3_IRQHandler Activated\n");
	NVIC_DisableIRQ(EINT3_IRQn);

    counter++;
}

void resetBuffer() {
	memset(receiverBuffer, 0,RECEIVER_BUFFER_SIZE);
}



int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    printf("init!\n\n");

#endif
#endif

    Chip_GPIO_SetPinDIRInput(LPC_GPIO, RX_PORT, RX_PIN);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, TX_PORT, TX_PIN);

    Chip_GPIO_SetPinDIRInput(LPC_GPIO, TEST_SWITCH_PORT, TEST_SWITCH_PIN);


    Chip_GPIO_SetPinDIROutput(LPC_GPIO, EXTERNAL_GREEN_LED_PORT, EXTERNAL_GREEN_LED_PIN);

    scrambleShiftRegisterInit(SCRAMBLER_ORDER,0);
    descrambleShiftRegisterInit(SCRAMBLER_ORDER,0);



    uint32_t timerFreq;
    // setup timers

	/* Enable timer 0 clock */
	Chip_TIMER_Init(LPC_TIMER0);

	/* Timer rate is system clock rate */
	timerFreq = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER0);;
	printf("TIMER FREQUENCY %d\n",timerFreq);
	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset(LPC_TIMER0);
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);
	Chip_TIMER_SetMatch(LPC_TIMER0, 1, (timerFreq / TICKRATE0_HZ));
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 1);
	Chip_TIMER_Enable(LPC_TIMER0);

	/* Enable timer interrupt */
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);

	int32_t counter=0;
	printf("STARTING SEND/RECEIVE LOOP\n");
	int32_t first = 1;
	int32_t transmit_toggle = 1;

	while(1) {

#ifdef TRANSMIT_ENABLED
		static uint8_t switch_state;
		switch_state =  Chip_GPIO_GetPinState(LPC_GPIO, TEST_SWITCH_PORT, TEST_SWITCH_PIN);
		if(counter % 100000 == 0) {
			//Board_LED_Toggle(1);
		}



		static int32_t currQueueSize;
		currQueueSize = getQueueSize();
		if (currQueueSize == 0) { // wait until the queue size is almost empty

#ifdef SWITCH_TRANSMIT_ENABLED

			if(switch_state && transmit_toggle) {
				testTransmit();
				transmit_toggle = 0;
				transmit_enabled = 1;
				receive_enabled = 0;
				Board_LED_Set(1,true);

			}else if (switch_state == 0){
				transmit_toggle = 1;
				Board_LED_Set(1,false);

			}
#else
			testTransmit();
#endif
		}
		if(first){
			NVIC_EnableIRQ(TIMER0_IRQn);
			first =0;
		}
#endif

		sendCount++;
		counter++;
		loopCount++;
		int32_t index = -1;

#ifdef RECEIVE_ENABLED
		if(packetCheckCounter==0) {
			if(bufferIndexChanged()) {
				index = findPrefix(receiverBuffer);
			}
		}
		if (index > 0) {
#ifdef SCRAMBLE_ENABLED
			static uint8_t scrambledString[PAYLOAD_SIZE];
			memset(scrambledString,0,PAYLOAD_SIZE * sizeof(uint8_t));
			makeSubString(scrambledString,index, receiverBuffer, RECEIVER_BUFFER_SIZE, PAYLOAD_SIZE);
			resetBuffer();

			static uint8_t descrambledString[PAYLOAD_SIZE];
			memset(descrambledString,0,PAYLOAD_SIZE * sizeof(uint8_t));
			//printArrayBin(descrambledString, 32);
		    descrambleShiftRegisterReset();
		    int endOfStringIndex;
		    for (int i =0; i < PAYLOAD_SIZE;i++) {
		    	if (scrambledString[i] == '\n') {
		    		endOfStringIndex = i;
		    		break;
		    	}
		    }
		    memset(scrambledString + endOfStringIndex,0,PAYLOAD_SIZE-endOfStringIndex-1);
			descramble(descrambledString,scrambledString,endOfStringIndex+1);
			//printArrayBin(descrambledString, 32);
			char * payloadCString = makeSubStringChar(0, descrambledString, RECEIVER_BUFFER_SIZE, PAYLOAD_SIZE);

//			printf("INDEX: %d\t  SCRAM PAYLOAD: %s\n",index,scrambledString);
//			printArrayBin(scrambledString,10);
#ifdef ACK_MODE_ENABLED

			testTransmit();
			transmit_enabled = 1;
			receive_enabled = 0;

#endif
			printf("INDEX: %d\tDESCRAM PAYLOAD: %s\n",index,descrambledString);
#else
			char * payloadCString= makeSubStringChar(index, receiverBuffer, RECEIVER_BUFFER_SIZE, PAYLOAD_SIZE);
			resetBuffer();
			printf("INDEX: %d\tPAYLOAD: %s\n",index,payloadCString);

#endif
			free(payloadCString);

		} else {
			//printf("no match in buffer!\n");
		}
#endif
		//printf("done searching\n");
	}

    printf("&----DONE WITH ENGINE TEST----\n");

    return 0 ;
}


