/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "RTC.h"
#include "fsl_debug_console.h"


irtc_datetime_t datetime, alarmDatetime, getdatetime;
irtc_config_t irtcConfig;
uint8_t alarmHappen = false;
static uint8_t min_alarm_loop = 0;
static uint8_t sec_alarm_loop = 0;
static uint8_t hour_alarm_loop = 0;
static void (*alarm_reset)(void) = NULL;

/*!
 * @brief ISR for Alarm interrupt
 *
 * This function change state of busyWait.
 */
void RTC_IRQHandler(void)
{
    uint32_t flags = IRTC_GetStatusFlags(RTC);
    if (0U != flags)
    {
        alarmHappen = (0U != (flags & kIRTC_AlarmFlag));
        /* Unlock to allow register write operation */
        IRTC_SetWriteProtection(RTC, false);
        /*Clear all irtc flag */
        IRTC_ClearStatusFlags(RTC, flags);

        if(flags & kIRTC_WakeTimerFlag)
        {
        	IRTC_SetWakeupCount(RTC, true, WAKE_TIMER_INTERVAL);
        	alarmHappen = true;
        }

        if(NULL != alarm_reset)
        {
        	alarm_reset();
        }
        IRTC_GetDatetime(RTC, &alarmDatetime);
        alarm_loop();
    }
    SDK_ISR_EXIT_BARRIER;
}

void init_rtc(irtc_alarm_match_t alarm, irtc_clock_select_t clock)
{
    /* Init the IRTC module */
    /*
     * irtcConfig.wakeupSelect = true;
     * irtcConfig.timerStdMask = false;
     * irtcConfig.alrmMatch = depends on alarm parameter;
     */
    IRTC_GetDefaultConfig(&irtcConfig);
    irtcConfig.alrmMatch = alarm;
    irtcConfig.disableClockOutput = false;
	/* Enable 16k to domain Vbat and Main */
	CLOCK_SetupClk16KClocking(kCLOCK_Clk16KToVbat|kCLOCK_Clk16KToMain);
	if(kIRTC_Clk32K == clock)
	{
		/* Enable 32k to domain Vbat and Main */
		CLOCK_SetupOsc32KClocking(kCLOCK_Osc32kToAll);
		irtcConfig.clockSelect = kIRTC_Clk32K;
	}
	else
	{
		irtcConfig.clockSelect = kIRTC_Clk16K;
	}

    if (IRTC_Init(RTC, &irtcConfig) == kStatus_Fail)
    {
    	PRINTF("RTC init failed \r\n");
    	while(1);
    }
    IRTC_EnableSubsecondCounter(RTC, true);

}

uint8_t set_time(uint8_t second, uint8_t minute, uint8_t hour ,uint8_t day, uint8_t month, uint16_t year)
{
	uint8_t status;
	datetime.second = second;
	datetime.minute = minute;
	datetime.hour = hour;
	datetime.day = day;
	datetime.month = month;
	datetime.year = year;

	status = IRTC_SetDatetime(RTC, &datetime);
	return status;
}

uint8_t set_alarm(uint8_t second, uint8_t minute, uint8_t hour ,uint8_t day, uint8_t month, uint16_t year)
{
	uint8_t status;
	alarmDatetime.second = second;
	alarmDatetime.minute = minute;
	alarmDatetime.hour = hour;
	alarmDatetime.day = day;
	alarmDatetime.month = month;
	alarmDatetime.year = year;

	status = IRTC_SetAlarm(RTC, &alarmDatetime);
	return status;
}

void enable_rtc_interrupts(RTC_INTERRUPT_SELECT interrupt)
{
    /* Unlock to allow register write operation */
    IRTC_SetWriteProtection(RTC, false);
    if(ALARM_INTERRUPT == interrupt)
    {
        /* Enable RTC alarm interrupt */
    	IRTC_EnableInterrupts(RTC, kIRTC_AlarmInterruptEnable);
    }
    else
    {
    	/* Enable RTC wake timer interrupt */
		IRTC_EnableInterrupts(RTC, kIRTC_WakeTimerInterruptEnable);
    }

	/* Enable at the NVIC */
	EnableIRQ(RTC_IRQn);
}

irtc_datetime_t* get_rtc_time(void)
{
	return &getdatetime;
}

void read_rtc_time(void)
{
	IRTC_GetDatetime(RTC, &getdatetime);
}


void clear_alarm_status(void)
{
	alarmHappen = false;
}

void print_time(ALARM_SELECT selector)
{
	switch(selector)
	{
	case RTC_SETTED_TIME:
		PRINTF("\r\nSetted Time (YYYY/MM/DD HH:MM:SS)is %d/%d/%d %d:%d:%d\r\n", datetime.year, datetime.month, datetime.day,
		           datetime.hour, datetime.minute, datetime.second);
		break;


	case RTC_TIME_READ:
		read_rtc_time();
		PRINTF("\r\nTime is (YYYY/MM/DD HH:MM:SS)is %d/%d/%d %d:%d:%d\r\n", getdatetime.year, getdatetime.month, getdatetime.day,
				           getdatetime.hour, getdatetime.minute, getdatetime.second);

		break;

	case RTC_ALARM:
		PRINTF("\r\n\r\nAlarm is set to (YYYY/MM/DD HH:MM:SS)is %d/%d/%d %d:%d:%d\r\n", alarmDatetime.year, alarmDatetime.month, alarmDatetime.day,
						           alarmDatetime.hour, alarmDatetime.minute, alarmDatetime.second);
	default:
		break;
	}
}

uint8_t get_alarm_status(void)
{
	return alarmHappen;
}


void set_coarse_compensation(void)
{
	/*
	 * Set coarse compensation
	 * Compensation value is  two's complement value that modifies the 1hz clock by adding or removing cycles
	 * Compensation interval duration in sec for the corection to be applied
	 *
	 */
	uint8_t compensation_val_2_complement = 9U;// 9 compliments for a 6 cycles corrections each second
	uint8_t interva_seconds = 1;
	IRTC_SetCoarseCompensation(RTC, compensation_val_2_complement, interva_seconds);
}

void set_fine_compensation(void)
{
	/*
	 * Set fine compensation
	 * RTC
	 * Two compliment od 115U is 140U
	 * 115U number of clock cycles of a fixed 4.194304Mhz clock that have to be added.
	 * Add previous fractional value
	 */
	uint8_t compensation_val = 115U;
	uint8_t compensation_val_2_complement = 140U;
	IRTC_SetFineCompensation(RTC, compensation_val_2_complement, compensation_val, true);
}

void alarm_loop(void)
{
	uint8_t help_var =0, help_var2=0;
	help_var = alarmDatetime.second + sec_alarm_loop;
	if(60 <= help_var)
	{
		help_var -= 60;
		alarmDatetime.second = help_var;
		help_var2 = 1;
	}
	else
	{
		alarmDatetime.second = help_var;
	}

	help_var = alarmDatetime.minute + min_alarm_loop + help_var2;

	if(60 <= help_var)
	{
		help_var -= 60;
		alarmDatetime.minute = help_var;
		help_var2 = 1;
	}
	else
	{
		alarmDatetime.minute = help_var;
		help_var2 = 0;
	}

	help_var = alarmDatetime.hour + hour_alarm_loop + help_var2;

	if(24 <= help_var)
	{
		help_var -= 24;
		alarmDatetime.hour = help_var;
	}
	else
	{
		alarmDatetime.hour = help_var;
	}

	IRTC_SetAlarm(RTC, &alarmDatetime);
}

void alarm_time_loop(uint8_t min, uint8_t sec)
{
	IRTC_GetDatetime(RTC, &datetime);
	uint8_t extra_min = sec/60;
	sec -= extra_min * 60;
	min += extra_min;
	uint8_t extra_hour = min/60;
	min -= extra_hour * 60;
	hour_alarm_loop = extra_hour;
	sec_alarm_loop = sec;
	min_alarm_loop = min;
	alarm_reset = alarm_loop;
	alarmDatetime = datetime;
	alarm_loop();
    /* Enable RTC alarm interrupt */
	IRTC_EnableInterrupts(RTC, kIRTC_AlarmInterruptEnable);

	/* Enable at the NVIC */
	EnableIRQ(RTC_IRQn);
}

void set_wake_timer(void)
{
    /* Unlock to allow register write operation */
    IRTC_SetWriteProtection(RTC, false);
	IRTC_SetWakeupCount(RTC, true, WAKE_TIMER_INTERVAL);
}

void change_alarm_mode(irtc_alarm_match_t alarm)
{
    uint16_t reg;
    /* Unlock to allow register write operation */
    IRTC_SetWriteProtection(RTC, false);
	/* Issue a software reset */
	IRTC_Reset(RTC);
	reg = RTC->CTRL;
	reg &= ~((uint16_t)RTC_CTRL_ALM_MATCH_MASK);
	reg |= (RTC_CTRL_ALM_MATCH(alarm));
	RTC->CTRL = reg;
}
