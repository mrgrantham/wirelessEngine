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

 #define TICKRATE1_HZ 4
 #define TICKRATE2_HZ 10 // 9600 bps

#define TX_PORT 0
#define TX_PIN 9

#define RX_PORT 0
#define RX_PIN 8

#define TEST_SWITCH_PORT 2
#define TEST_SWITCH_PIN 10
// TODO: insert other include files here

// TODO: insert other definitions and declarations here

uint8_t receiverBuffer[1024];

char *payloadStringReceived;
uint8_t *packet;
uint8_t packetSize=0;

LPC_GPIOINT_T GPIOINT_SETUP;


void engineTest() {
	  payloadStringReceived = "JAMES 6912";
	  char * payloadString = malloc(strlen(payloadStringReceived)+1);
	  memcpy(payloadString,payloadStringReceived,strlen(payloadStringReceived)+1);


	  composePacket(payloadString, &packet, &packetSize);
	  //printReadyPacket(packet, 64);

	  push(0x1);
	  push(0x1);
	  push(0x1);
	  push(0x1);
	  push(0x1);
	  push(0x1);
	  push(0x1);
	  push(0x1);
	  push(0x1);
	  push(0x1);
	  push(0x1);
	  push(0x1);

	  sendPacket(packet);
	  deletePacket(&packet,&packetSize);
	  //printTransmissionLine();

	  // recieving side
	  // printTransmissionLine();
	  /*
	  uint8_t aBit = 0;
	  aBit = readBit();
	  while (aBit != 0xff) {
	    sendToBuffer(aBit,receiverBuffer);
	    aBit = readBit();
	  }




	  printf("stuff\n");
	  //printBuffer(receiverBuffer);

	  uint32_t index;
	  index = findPrefix(receiverBuffer);
	  printf("\nbitIndex of match %d\n",index);
	  uint8_t *foundPayload = makeSubString(index, receiverBuffer, 1024, 32);
	  printArrayChar(foundPayload, 32);

	  findPacket();
	*/
}

// Transmit and Receive Test
void TIMER1_IRQHandler(void)
{
	static bool On = false;

   if (Chip_TIMER_MatchPending(LPC_TIMER1, 1)) {
      Chip_TIMER_ClearMatch(LPC_TIMER1, 1);
      On = (bool) !On;
      Board_LED_Toggle(0);
     }
   printf("TIMER1\n");

}

// Receive
void TIMER2_IRQHandler(void)
{
    static bool On = false;
    uint8_t val = sendBit();
    //printf("%d",val);
  if (val) {
   Chip_TIMER_ClearMatch(LPC_TIMER2, 1);
   if(On) {
	   //Chip_GPIO_SetPinOutLow(LPC_GPIO, TX_PORT, TX_PIN);
	   Chip_GPIO_SetPinOutHigh(LPC_GPIO, TX_PORT, TX_PIN);

   } else {
	   Chip_GPIO_SetPinOutLow(LPC_GPIO, TX_PORT, TX_PIN);

   }
   On = (bool) !On;
   Board_LED_Toggle(1);
  }
  //printf("TIMER2\n");
  //engineTest();


}

void GPIO_IRQ_HANDLER(void) {

	printf("EINT3_IRQHandler Activated\n");
	Chip_GPIOINT_ClearIntStatus(&GPIOINT_SETUP, GPIOINT_PORT2, TEST_SWITCH_PIN);
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

    engineTest();
    engineTest();
    engineTest();


    uint32_t timerFreq;
    // setup timers
    /*
    Chip_TIMER_Init(LPC_TIMER1);
    timerFreq = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER1);
    Chip_TIMER_Reset(LPC_TIMER1);
    Chip_TIMER_MatchEnableInt(LPC_TIMER1, 1);
    Chip_TIMER_SetMatch(LPC_TIMER1, 1, (timerFreq / TICKRATE1_HZ));
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER1, 1);
    Chip_TIMER_Enable(LPC_TIMER1);
    NVIC_EnableIRQ(TIMER1_IRQn);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    */
    Chip_TIMER_Init(LPC_TIMER2);
    timerFreq = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER2);
    //printf("timerFreq: %d\n",timerFreq);
    Chip_TIMER_Reset(LPC_TIMER2);
    Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);
    Chip_TIMER_SetMatch(LPC_TIMER2, 1, (timerFreq / TICKRATE2_HZ));
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 1);
    Chip_TIMER_Enable(LPC_TIMER2);
    NVIC_EnableIRQ(TIMER2_IRQn);
    NVIC_ClearPendingIRQ(TIMER2_IRQn);

    // TODO: insert code here

    // setup external interrupt for receiver
    // setup switch to test external interrupt
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, TEST_SWITCH_PORT, TEST_SWITCH_PIN);
    /*
    while(1){
    	if(Chip_GPIO_ReadPortBit(LPC_GPIO, TEST_SWITCH_PORT, TEST_SWITCH_PIN)){
    		EINT3_IRQHandler();
    	}
    }
    */

    Chip_GPIOINT_Init(LPC_GPIOINT);
    Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT2, (1<<TEST_SWITCH_PIN));
    Chip_GPIOINT_SetIntRising(LPC_GPIOINT, GPIOINT_PORT2, (1<<TEST_SWITCH_PIN));

    NVIC_ClearPendingIRQ(EINT3_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);







    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter

    Board_LED_Set(0, true);
    Board_LED_Set(1, true);
    Board_LED_Set(2, true);

    printf("&----DONE WITH ENGINE TEST----\n");
    while(1) {
        __WFI();
  //  	printf("hello\n");
        i++ ;
    }
    return 0 ;
}


