/**
 * @file    MKL46Z256xxx4_cvicenie_2.cpp
 * @brief   Application entry point.
 */

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_pit.h"

volatile bool pitIsrFlag = false;

void delay(uint32_t t) {
	volatile uint32_t i = 0;
	for (i = 0; i < 500000 * t; ++i) { //200000 -> 200ms // 20Mhz clozk
		__asm("NOP");
		/* delay */
	}
}

char *MORSE_TABLE[] =
{ 		"._", //A
		"_...", //B
		"_._.", //C
		"_..", //D
		".", //E
		".._.", //F
		"__.", //G
		"....", //H
		"..", //I
		".___", //J
		"_._", //K
		"._..", //L
		"__", //M
		"_.", //N
		"___", //O
		".__.", //P
		"__._", //Q
		"._.", //R
		"...", //S
		"_", //T
		".._", //U
		"..._", //V
		".__", //W
		"_.._", //X
		"_.__", //Y
		"__..", //Z
		"_____", //0
		".____", //1
		"..___", //2
		"...__", //3
		"...._", //4
		".....", //5
		"_....", //6
		"__...", //7
		"___..", //8
		"____.", //9
		};

#define LED_OFF()	GPIO_SetPinsOutput(BOARD_INITPINS_LED_GREEN_GPIO, BOARD_INITPINS_LED_GREEN_GPIO_PIN_MASK);
#define LED_ON() 	GPIO_ClearPinsOutput(BOARD_INITPINS_LED_GREEN_GPIO, BOARD_INITPINS_LED_GREEN_GPIO_PIN_MASK);

extern "C" void PIT_IRQHandler()
{
	/* Clear interupt flag. */
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
	pitIsrFlag = true;

}

void delay_pin(int i)
{
	/* Start channel 0 */
	PIT_StartTimer(PIT, kPIT_Chnl_0);
	for (int j = 0; j < i; j++)
	{
		while(!pitIsrFlag)
		{
			__asm("NOP");
		}
		pitIsrFlag = false;
	}
	PIT_StopTimer(PIT, kPIT_Chnl_0);
}

int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    /* Structure of initialize PIT */
    pit_config_t pitConfig;

    PIT_GetDefaultConfig(&pitConfig);
    /* Init pit module */
    PIT_Init(PIT, &pitConfig);
    /* Set timer period for channel 0 */
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,
    		USEC_TO_COUNT(150000u,CLOCK_GetBusClkFreq()));

    /* Enable timer interrupts for channel 0 */
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(PIT_IRQn);

    PRINTF("Hello World\n");
    char *text = "A2C4E6";

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
	while (1) {
		char *pt = text;
		while (*pt != 0)
		{
			if (*pt != ' ')
			{
				PRINTF("%c\n",*pt);
				//char *c = *pt - 'A' >= 0 ? MORSE_TABLE[*pt - 'A'] : MORSE_TABLE[*pt - 'A']
				int tmp = *pt - 'A';

				/* Nič lepšie ma nenápadlo.
				 Kedže čísla sú kodované do 5 znakov (písmena od 2 až 4)
				 dalo by sa to kontrolovať nejak cez dlžku pôvodného textu
				*/
				if(tmp < 0)
					tmp = tmp + 45 - 2 ;
				char *c = MORSE_TABLE[tmp];

				while (*c != 0)
				{
					LED_ON();
					if (*c == '.')
						delay(1); 	//.
					else
						delay(3); 	// _

					LED_OFF();
					delay(1);
					c++;
				}
				delay(2); 			// 1 + 2 = 3
			} else
				// Medzera
				delay(7);

			pt++;
		}
		delay(4); //  3 + 4 = 7

	}
	return 0;
}
