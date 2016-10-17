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

#include "queue.h"
#include "LISAtransmitter.h"
#include "LISAreceiver.h"
#include "stdlib.h"
#include "string.h"

#include <cr_section_macros.h>

 #define TICKRATE0_HZ 400 // 200 bps
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

#define TRANSMIT_ENABLED
#define RECEIVE_ENABLED


uint8_t receiverBuffer[RECEIVER_BUFFER_SIZE];
static uint16_t packetCheckCounter = 0; // used to determine if enough bits have been received to check the buffer again


int sendCount=0;
int loopCount=0;

char *payloadStringReceived="JAMES 6912";
uint8_t *packet=NULL;
uint32_t packetSize=0;

void testTransmit() {

	  char * payloadString = malloc(strlen(payloadStringReceived)+1);
	  memcpy(payloadString,payloadStringReceived,strlen(payloadStringReceived)+1);


	  composePacket(payloadString, &packet, &packetSize);
	  //printReadyPacket(packet, 64);

	  sendPacket(packet,packetSize);
	  deletePacket(&packet,&packetSize);

}

// Transmit and Receive Test

void TIMER0_IRQHandler(void)
{

	if (Chip_TIMER_MatchPending(LPC_TIMER0, 1)) {
		Chip_TIMER_ClearMatch(LPC_TIMER0, 1);
	}
	//printf("%d",val);

	static int counter = 0;

	// broadcast on antenna
 	sendBit();
	// listen to receiver

	sendToBuffer(readBit(),receiverBuffer);
	packetCheckCounter = (packetCheckCounter+1)%(RECEIVER_BUFFER_SIZE<<3);

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

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, EXTERNAL_GREEN_LED_PORT, EXTERNAL_GREEN_LED_PIN);


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

    int counter=0;
	printf("STARTING SEND/RECEIVE LOOP\n");
	int first = 1;
	while(1) {

#ifdef TRANSMIT_ENABLED
		static uint8_t switch_state;
		switch_state =  Chip_GPIO_GetPinState(LPC_GPIO, TEST_SWITCH_PORT, TEST_SWITCH_PIN);
		static uint8_t transmit_locked;
		if(!switch_state) {
			transmit_locked = false;
			Board_LED_Set(1,true);
		}
		if (switch_state && !transmit_locked) {
			printf("prepping packet\n");
			testTransmit();
			transmit_locked = true;
			Board_LED_Set(1,false);

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
			index = findPrefix(receiverBuffer);
		}
		if (index > 0) {
			static uint8_t *foundPayload = NULL;;
			char * payloadCString= makeSubStringChar(&foundPayload,index, receiverBuffer, 128, 32);
			printf("PAYLOAD: %s\n",payloadCString);
			free(payloadCString);

		} else {
			//printf("no match in buffer!\n");
		}
#endif
		//printf("done searching\n");
	}









    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter



    printf("&----DONE WITH ENGINE TEST----\n");
    while(1) {
        __WFI();
  //  	printf("hello\n");
        i++ ;
    }
    return 0 ;
}


