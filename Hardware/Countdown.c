#include "stm32f10x.h"
#include "OLED.h"
#include "OLED_Data.h"
#include "Key.h"
#include "Buzzer.h"
#include "RGB.h"
#include "Countdown.h"

/* States */
#define CD_SETTING  0   // User is setting the duration
#define CD_COUNTING 1   // Countdown is running
#define CD_ALARM    2   // Time is up, buzzer is sounding

static uint8_t  cd_state        = CD_SETTING;
static uint16_t cd_set_min      = 1;     // Minutes chosen by user (1~99)
static uint32_t cd_remaining    = 0;     // Remaining seconds while counting
static uint16_t cd_beep_counter = 0;     // Controls beep on/off pattern


/**
  * Function : Countdown tick — call from TIM2 IRQ every 1 ms
  * Arguments: none
  * Returns  : none
  * Note     : Decrements the remaining time every second while COUNTING.
  *            Drives the beep pattern (500 ms on / 500 ms off) while ALARM.
  */
void Countdown_Tick(void)
{
	static uint16_t ms = 0;

	if (cd_state == CD_COUNTING)
	{
		ms++;
		if (ms >= 1000)
		{
			ms = 0;
			if (cd_remaining > 0) cd_remaining--;
			if (cd_remaining == 0)
			{
				cd_state = CD_ALARM;
				RGB_Red();   // Red when time is up
			}
		}
	}
	else if (cd_state == CD_ALARM)
	{
		cd_beep_counter++;
		if      (cd_beep_counter < 500)  Buzzer_ON();
		else if (cd_beep_counter < 1000) Buzzer_OFF();
		else                             cd_beep_counter = 0;
	}
	else
	{
		ms = 0;
		cd_beep_counter = 0;
		Buzzer_OFF();
	}
}


/**
  * Function : Render the countdown screen for the current state
  */
static void Show_Countdown_UI(void)
{
	OLED_Clear();
	OLED_ShowImage(0, 0, 16, 16, Return);   // Back arrow top-left

	if (cd_state == CD_SETTING)
	{
		OLED_ShowString(20, 0, "Countdown", OLED_8X16);

		/* Display set time as MM:00 */
		OLED_ShowNum(8,  20, cd_set_min, 2, OLED_12X24);
		OLED_ShowChar(32, 20, ':', OLED_12X24);
		OLED_ShowNum(44, 20, 0,   2, OLED_12X24);
		OLED_ShowChar(80, 20, 'm', OLED_8X16);

		/* Button hints at bottom */
		OLED_ShowString(0,  48, "-1",    OLED_8X16);
		OLED_ShowString(44, 48, "Start", OLED_8X16);
		OLED_ShowString(104,48, "+1",    OLED_8X16);
	}
	else if (cd_state == CD_COUNTING)
	{
		OLED_ShowString(20, 0, "Countdown", OLED_8X16);

		uint16_t m = (uint16_t)(cd_remaining / 60);
		uint16_t s = (uint16_t)(cd_remaining % 60);
		OLED_ShowNum(8,  20, m, 2, OLED_12X24);
		OLED_ShowChar(32, 20, ':', OLED_12X24);
		OLED_ShowNum(44, 20, s, 2, OLED_12X24);

		OLED_ShowString(28, 48, "Cancel", OLED_8X16);
	}
	else  /* CD_ALARM */
	{
		OLED_ShowString(16, 12, "TIME UP!", OLED_8X16);
		OLED_ShowString(4,  40, "Press to stop", OLED_6X8);
	}

	OLED_Update();
}


/**
  * Function : Countdown page — entry point called from Menu()
  * Returns  : 0 when user exits
  *
  * Button mapping:
  *   SETTING  — Key1: -1 min  |  Key2: +1 min  |  Key3: start countdown
  *   COUNTING — Key3: cancel and return to menu
  *   ALARM    — Key3: stop buzzer and return to menu
  *
  * To exit from SETTING without starting, navigate cursor to the back
  * arrow (top-left) and press Key3 — same pattern as all other pages.
  * Here we keep it simple: Key1 in SETTING when minutes would go below 1
  * does nothing; to exit SETTING early hold Key3 (Key4 = long press).
  */
int Countdown(void)
{
	/* Reset state each time page is entered */
	cd_state     = CD_SETTING;
	cd_set_min   = 1;
	cd_remaining = 0;
	Buzzer_OFF();
	RGB_Green();   // Green when idle on countdown page

	while (1)
	{
		uint8_t key = Key_GetNum();

		if (cd_state == CD_SETTING)
		{
			if (key == 1)          // Key1: decrease minutes
			{
				if (cd_set_min > 1) cd_set_min--;
			}
			else if (key == 2)     // Key2: increase minutes
			{
				if (cd_set_min < 99) cd_set_min++;
			}
			else if (key == 3)     // Key3 short: start
			{
				cd_remaining = (uint32_t)cd_set_min * 60;
				cd_state     = CD_COUNTING;
				RGB_Yellow();  // Yellow while counting
				OLED_Clear();
				OLED_Update();
			}
			else if (key == 4)     // Key3 long press: exit page
			{
				Buzzer_OFF();
				RGB_Green();   // Back to green on exit
				OLED_Clear();
				OLED_Update();
				return 0;
			}
		}
		else if (cd_state == CD_COUNTING)
		{
			if (key == 3)          // Key3: cancel countdown, return to menu
			{
				cd_state = CD_SETTING;
				RGB_Green();   // Back to green on cancel
				OLED_Clear();
				OLED_Update();
				return 0;
			}
		}
		else  /* CD_ALARM */
		{
			if (key == 3)          // Key3: silence alarm, return to menu
			{
				Buzzer_OFF();
				RGB_Green();   // Back to green after alarm stopped
				cd_state = CD_SETTING;
				OLED_Clear();
				OLED_Update();
				return 0;
			}
		}

		Show_Countdown_UI();
	}
}
