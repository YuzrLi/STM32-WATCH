#include "stm32f10x.h"
#include "Buzzer.h"

static uint8_t alert_active = 0;
static uint16_t alert_counter = 0;

void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

void Buzzer_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

void Buzzer_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

/* Start continuous alert beeping — driven by Buzzer_Tick */
void Buzzer_Alert_Start(void)
{
	alert_active  = 1;
	alert_counter = 0;
}

/* Stop alert beeping */
void Buzzer_Alert_Stop(void)
{
	alert_active = 0;
	Buzzer_OFF();
}

/* Call from TIM2 IRQ every 1ms — produces 150ms on / 150ms off beep pattern */
void Buzzer_Tick(void)
{
	if (!alert_active) return;

	alert_counter++;
	if      (alert_counter < 150)  Buzzer_ON();
	else if (alert_counter < 300)  Buzzer_OFF();
	else                           alert_counter = 0;
}
