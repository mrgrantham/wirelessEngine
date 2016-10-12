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

#include <cr_section_macros.h>

 #define TICKRATE1_HZ 10
 #define TICKRATE2_HZ 7

#define TX_PORT 0
#define TX_PIN 9

#define RX_PORT 0
#define RX_PIN 8
// TODO: insert other include files here

// TODO: insert other definitions and declarations here

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

void TIMER2_IRQHandler(void)
{
    static bool On = false;

  if (Chip_TIMER_MatchPending(LPC_TIMER2, 1)) {
   Chip_TIMER_ClearMatch(LPC_TIMER2, 1);
   On = (bool) !On;
   Board_LED_Toggle(1);
  }
  printf("TIMER2\n");
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
    Chip_GPIO_SetPinDIR(LPC_GPIO, TX_PORT, TX_PIN,0);


    uint32_t timerFreq;
    // setup timers
    Chip_TIMER_Init(LPC_TIMER1);
    timerFreq = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER1);
    Chip_TIMER_Reset(LPC_TIMER1);
    Chip_TIMER_MatchEnableInt(LPC_TIMER1, 1);
    Chip_TIMER_SetMatch(LPC_TIMER1, 1, (timerFreq / TICKRATE1_HZ));
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER1, 1);
    Chip_TIMER_Enable(LPC_TIMER1);
    NVIC_EnableIRQ(TIMER1_IRQn);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);

    Chip_TIMER_Init(LPC_TIMER2);
    timerFreq = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_TIMER2);
    Chip_TIMER_Reset(LPC_TIMER2);
    Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);
    Chip_TIMER_SetMatch(LPC_TIMER2, 1, (timerFreq / TICKRATE2_HZ));
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 1);
    Chip_TIMER_Enable(LPC_TIMER2);
    NVIC_EnableIRQ(TIMER2_IRQn);
    NVIC_ClearPendingIRQ(TIMER2_IRQn);
    // TODO: insert code here

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter

    Board_LED_Set(0, true);
    Board_LED_Set(1, true);
    Board_LED_Set(2, true);
    while(1) {
        __WFI();
    	printf("hello\n");
        i++ ;
    }
    return 0 ;
}
