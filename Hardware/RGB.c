#include "stm32f10x.h"
#include "RGB.h"

/**
  * Function : LED initialization
  * Arguments: none
  * Returns  : none
  * Note     : Three independent LEDs, all active-low (LOW = on, HIGH = off)
  *            PA8 = Green, PA9 = Yellow, PA10 = Red
  *            Hardware: LED anode -> pin, cathode -> GND (via resistor if available).
  */
void RGB_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Default: green on */
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);   // Green ON
	GPIO_SetBits(GPIOA, GPIO_Pin_9);     // Yellow OFF
	GPIO_SetBits(GPIOA, GPIO_Pin_10);    // Red OFF
}

/**
  * Function : Green on — idle / not counting
  */
void RGB_Green(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);   // Green ON
	GPIO_SetBits(GPIOA, GPIO_Pin_9);     // Yellow OFF
	GPIO_SetBits(GPIOA, GPIO_Pin_10);    // Red OFF
}

/**
  * Function : Yellow on — countdown running
  */
void RGB_Yellow(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_8);     // Green OFF
	GPIO_ResetBits(GPIOA, GPIO_Pin_9);   // Yellow ON
	GPIO_SetBits(GPIOA, GPIO_Pin_10);    // Red OFF
}

/**
  * Function : Red on — time up / alarm
  */
void RGB_Red(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_8);     // Green OFF
	GPIO_SetBits(GPIOA, GPIO_Pin_9);     // Yellow OFF
	GPIO_ResetBits(GPIOA, GPIO_Pin_10);  // Red ON
}

/**
  * Function : All LEDs off
  */
void RGB_Off(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_8);     // Green OFF
	GPIO_SetBits(GPIOA, GPIO_Pin_9);     // Yellow OFF
	GPIO_SetBits(GPIOA, GPIO_Pin_10);    // Red OFF
}
