#include "stm32f10x.h"
#include "Buzzer.h"

/**
  * Function : Buzzer initialization
  * Arguments: none
  * Returns  : none
  * Note     : Configures PA3 as push-pull output for active-low buzzer.
  *            Buzzer OFF by default on init.
  *            Hardware: buzzer I/O pin -> PA3, VCC -> 3.3V, GND -> GND.
  */
void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_3);    // Default OFF (active-low: HIGH = off)
}

/**
  * Function : Turn buzzer on (pull low to activate)
  */
void Buzzer_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

/**
  * Function : Turn buzzer off (pull high to deactivate)
  */
void Buzzer_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
}
