/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "menu.h"

void init_menu(void)
{
	int8_t character;
	irtc_clock_select_t clock;
	RTC_INTERRUPT_SELECT interrupt = ALARM_INTERRUPT;
	PRINTF("\r\nSelect an alarm mode\r\n1)\t alarm n seconds\r\n2)\t alarm n seconds & minutes\r\n3)\t alarm n seconds & min & hour");
	PRINTF("\r\n4)\t alarm to second & minute & hour & day \r\n5)\t alarm to second & minute & hour & day & month"
			"\r\n6)\t alarm to second & minute & hour & day & month & year"
			"\r\n7)\t interrupt loop using the wake timer \r\n");
	do
	{
		character = GETCHAR();
		PUTCHAR(character);
		switch (character)
		{
		case '1':
			PRINTF("\r\n\t alarm each n seconds\r\n");
			clock = get_clock_selection();
			init_rtc(kRTC_MatchSecMinHr, clock);
			select_rtc_compensation();
			set_hour();
			alarm_match_sec();
			break;
		case '2':
			PRINTF("\r\n\t alarm each n seconds & n min\r\n");
			clock = get_clock_selection();
			init_rtc(kRTC_MatchSecMinHr, clock);
			select_rtc_compensation();
			set_hour();
			alarm_match_min();
			break;
		case '3':
			PRINTF("\r\n\t alarm to match seconds & min & hour\r\n");
			clock = get_clock_selection();
			init_rtc(kRTC_MatchSecMinHr, clock);
			select_rtc_compensation();
			set_hour();
			alarm_match_hour();
			break;
		case '4':
			PRINTF("\r\n\t alarm to match seconds & min & hour & day\r\n");
			clock = get_clock_selection();
			init_rtc(kRTC_MatchSecMinHrDay, clock);
			select_rtc_compensation();
			set_hour();
			alarm_match_day();
			break;
		case '5':
			PRINTF("\r\n\t alarm to match seconds & min & hour & day & month \r\n");
			clock = get_clock_selection();
			init_rtc(kRTC_MatchSecMinHrDayMnth, clock);
			select_rtc_compensation();
			set_hour();
			alarm_match_month();
			break;
		case '6':
			PRINTF("\r\n\t alarm to match seconds & min & hour & day & month & year \r\n");
			clock = get_clock_selection();
			init_rtc(kRTC_MatchSecMinHrDayMnthYr, clock);
			select_rtc_compensation();
			set_hour();
			alarm_calendar_mode();
			break;
		case '7':
			PRINTF("\r\n\t interrupt loop using the wake timer \r\n");
			clock = get_clock_selection();
			init_rtc(kRTC_MatchSecMinHrDayMnthYr, clock);
			select_rtc_compensation();
			set_hour();
			set_wake_timer();
			interrupt = WAKE_TIMER_INTERRUPT;
			break;
		default:
			PRINTF("\r\nNon selectable option, select an available option\r\n");
			break;
		}
	}while(('0' > character) || ( '7' < character));
	enable_rtc_interrupts(interrupt);
	PRINTF("\r\n\tWait for alarm trigger\r\n\n");
}

void alarm_match_sec(void)
{
	uint8_t i=0;
	uint8_t buff[15] = {0};
	uint8_t sec, min;
	PRINTF("Set the alarm seconds\r\n");
	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				sec = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				sec = (buff[1] - '0');
			}
			if((99 < sec) || (0 == sec))
			{
				PRINTF("Set betwen 1-99 seconds\r\n");
				PRINTF("Set the RTC seconds\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	min = 0;
	IRTC_SetWriteProtection(RTC, false);	// unlock registers for writing
	alarm_time_loop(min, sec);
	PRINTF("\r\n");
	print_time(RTC_ALARM);
}

void alarm_match_min(void)
{
	uint8_t i=0;
	uint8_t buff[15] = {0};
	uint8_t sec, min;

	PRINTF("Set the alarm seconds\r\n");
	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				sec = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				sec = (buff[1] - '0');
			}
			if((99 < sec) || (0 == sec))
			{
				PRINTF("Set betwen 1-99 seconds\r\n");
				PRINTF("Set the alarm seconds\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	PRINTF("\r\nSet the alarm minutes\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				min = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				min = (buff[1] - '0');
			}
			if((99 < min) || (0 == min))
			{
				PRINTF("Set betwen 1-99 minutes\r\n");
				PRINTF("Set the alarm minutes\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	IRTC_SetWriteProtection(RTC, false);	// unlock registers for writing
	alarm_time_loop(min, sec);
	PRINTF("\r\n");
	print_time(RTC_ALARM);
}

void set_hour(void)
{
	uint8_t i=0;
	uint8_t buff[15] = {0};
	uint8_t sec, min, hour, day, month;
	uint16_t year;
	PRINTF("\r\n\nFirst set the RTC time\r\n");
	PRINTF("Set the RTC seconds\r\n");
	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				sec = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				sec = (buff[1] - '0');
			}
			if(60 <= sec)
			{
				PRINTF("Set betwen 0-59 seconds\r\n");
				PRINTF("Set the RTC seconds\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	PRINTF("\r\nSet the RTC minutes\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				min = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				min = (buff[1] - '0');
			}
			if(60 <= min)
			{
				PRINTF("Set betwen 0-59 minutes\r\n");
				PRINTF("Set the RTC minutes\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));



	PRINTF("\r\nSet the RTC hour\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				hour = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				hour = (buff[1] - '0');
			}
			if(24 <= hour)
			{
				PRINTF("Set betwen 0-24 hour\r\n");
				PRINTF("Set the RTC hour\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));


	PRINTF("\r\nSet the RTC day\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				day = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				day = (buff[1] - '0');
			}
			if(31 < day)
			{
				PRINTF("Set betwen 1-31 days\r\n");
				PRINTF("Set the RTC day\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	PRINTF("\r\nSet the RTC month\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				month = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				month = (buff[1] - '0');
			}
			if(12 < month)
			{
				PRINTF("Set betwen 1-12 months\r\n");
				PRINTF("Set the RTC month\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));


	PRINTF("\r\nSet the RTC year\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(5 == i)
			{
				year = (buff[1] - '0')*1000 + (buff[2] - '0')*100 +(buff[3] - '0')*10 + (buff[4] - '0');
			}
			else
			{

				PRINTF("Set year format to YYYY\r\n");
				PRINTF("Set the RTC year\r\n");
				for(i=0;i<6;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	IRTC_SetWriteProtection(RTC, false);	// unlock registers for writing
	set_time(sec, min, hour, day, month, year);
	PRINTF("\r\n\n");
	print_time(RTC_SETTED_TIME);
    /* Enable RTC alarm interrupt */
	IRTC_EnableInterrupts(RTC, kIRTC_AlarmInterruptEnable);

	/* Enable at the NVIC */
	EnableIRQ(RTC_IRQn);
}

void alarm_match_hour(void)
{
	uint8_t i=0;
	uint8_t buff[15] = {0};
	uint8_t sec, min, hour, day, month;
	uint16_t year;
	PRINTF("Set the alarm seconds\r\n");
	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				sec = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				sec = (buff[1] - '0');
			}
			if(60 <= sec)
			{
				PRINTF("Set betwen 0-59 seconds\r\n");
				PRINTF("Set the alarm seconds\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	PRINTF("\r\nSet the alarm minutes\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				min = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				min = (buff[1] - '0');
			}
			if(60 <= min)
			{
				PRINTF("Set betwen 0-59 minutes\r\n");
				PRINTF("Set the alarm minutes\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));



	PRINTF("\r\nSet the alarm hour\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				hour = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				hour = (buff[1] - '0');
			}
			if(24 <= hour)
			{
				PRINTF("Set betwen 0-24 hour\r\n");
				PRINTF("Set the alarm hour\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	day = 10;

	month = 10;


	year = 2001;
	IRTC_SetWriteProtection(RTC, false);	// unlock registers for writing
	set_alarm(sec, min, hour, day, month, year);
	PRINTF("\r\n");
	print_time(RTC_ALARM);
    /* Enable RTC alarm interrupt */
	IRTC_EnableInterrupts(RTC, kIRTC_AlarmInterruptEnable);

	/* Enable at the NVIC */
	EnableIRQ(RTC_IRQn);
}

void alarm_match_day(void)
{
	uint8_t i=0;
	uint8_t buff[15] = {0};
	uint8_t sec, min, hour, day, month;
	uint16_t year;
	PRINTF("Set the alarm seconds\r\n");
	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				sec = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				sec = (buff[1] - '0');
			}
			if(60 <= sec)
			{
				PRINTF("Set betwen 0-59 seconds\r\n");
				PRINTF("Set the alarm seconds\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	PRINTF("\r\nSet the alarm minutes\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				min = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				min = (buff[1] - '0');
			}
			if(60 <= min)
			{
				PRINTF("Set betwen 0-59 minutes\r\n");
				PRINTF("Set the alarm minutes\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));



	PRINTF("\r\nSet the alarm hour\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				hour = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				hour = (buff[1] - '0');
			}
			if(24 <= hour)
			{
				PRINTF("Set betwen 0-24 hour\r\n");
				PRINTF("Set the alarm hour\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));


	PRINTF("\r\nSet the alarm day\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				day = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				day = (buff[1] - '0');
			}
			if(31 < day)
			{
				PRINTF("Set betwen 1-31 days\r\n");
				PRINTF("Set the alarm day\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	month = 5;
	year = 2001;
	IRTC_SetWriteProtection(RTC, false);	// unlock registers for writing
	set_alarm(sec, min, hour, day, month, year);
	PRINTF("\r\n");
	print_time(RTC_ALARM);
    /* Enable RTC alarm interrupt */
	IRTC_EnableInterrupts(RTC, kIRTC_AlarmInterruptEnable);

	/* Enable at the NVIC */
	EnableIRQ(RTC_IRQn);
}

void alarm_match_month(void)
{
	uint8_t i=0;
	uint8_t buff[15] = {0};
	uint8_t sec, min, hour, day, month;
	uint16_t year;
	PRINTF("Set the alarm seconds\r\n");
	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				sec = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				sec = (buff[1] - '0');
			}
			if(60 <= sec)
			{
				PRINTF("Set betwen 0-59 seconds\r\n");
				PRINTF("Set the alarm seconds\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	PRINTF("\r\nSet the alarm minutes\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				min = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				min = (buff[1] - '0');
			}
			if(60 <= min)
			{
				PRINTF("Set betwen 0-59 minutes\r\n");
				PRINTF("Set the alarm minutes\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));



	PRINTF("\r\nSet the alarm hour\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				hour = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				hour = (buff[1] - '0');
			}
			if(24 <= hour)
			{
				PRINTF("Set betwen 0-24 hour\r\n");
				PRINTF("Set the alarm hour\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));


	PRINTF("\r\nSet the alarm day\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				day = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				day = (buff[1] - '0');
			}
			if(31 < day)
			{
				PRINTF("Set betwen 1-31 days\r\n");
				PRINTF("Set the alarm day\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	PRINTF("\r\nSet the alarm month\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				month = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				month = (buff[1] - '0');
			}
			if(12 < month)
			{
				PRINTF("Set betwen 1-12 months\r\n");
				PRINTF("Set the alarm month\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	year = 2001;
	IRTC_SetWriteProtection(RTC, false);	// unlock registers for writing
	set_alarm(sec, min, hour, day, month, year);
	PRINTF("\r\n");
	print_time(RTC_ALARM);
    /* Enable RTC alarm interrupt */
	IRTC_EnableInterrupts(RTC, kIRTC_AlarmInterruptEnable);

	/* Enable at the NVIC */
	EnableIRQ(RTC_IRQn);
}

void alarm_calendar_mode(void)
{
	uint8_t i=0;
	uint8_t buff[15] = {0};
	uint8_t sec, min, hour, day, month;
	uint16_t year;
	PRINTF("Set the alarm seconds\r\n");
	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				sec = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				sec = (buff[1] - '0');
			}
			if(60 <= sec)
			{
				PRINTF("Set betwen 0-59 seconds\r\n");
				PRINTF("Set the alarm seconds\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	PRINTF("\r\nSet the alarm minutes\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				min = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				min = (buff[1] - '0');
			}
			if(60 <= min)
			{
				PRINTF("Set betwen 0-59 minutes\r\n");
				PRINTF("Set the alarm minutes\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));



	PRINTF("\r\nSet the alarm hour\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				hour = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				hour = (buff[1] - '0');
			}
			if(24 <= hour)
			{
				PRINTF("Set betwen 0-24 hour\r\n");
				PRINTF("Set the alarm hour\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));


	PRINTF("\r\nSet the alarm day\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				day = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				day = (buff[1] - '0');
			}
			if(31 < day)
			{
				PRINTF("Set betwen 1-31 days\r\n");
				PRINTF("Set the alarm day\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	PRINTF("\r\nSet the alarm month\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(3== i)
			{
				month = (buff[1] - '0')*10 + (buff[2] - '0');
			}
			else
			{
				month = (buff[1] - '0');
			}
			if(12 < month)
			{
				PRINTF("Set betwen 1-12 months\r\n");
				PRINTF("Set the alarm month\r\n");
				for(i=0;i<4;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));


	PRINTF("\r\nSet the alarm year\r\n");
	i=0;

	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);
		if('\r' == buff[i])
		{
			if(5 == i)
			{
				year = (buff[1] - '0')*1000 + (buff[2] - '0')*100 +(buff[3] - '0')*10 + (buff[4] - '0');
			}
			else
			{

				PRINTF("Set year format to YYYY\r\n");
				PRINTF("Set the alarm year\r\n");
				for(i=0;i<6;i++)
				{
					buff[i] = 0;
				}
				i=0;
			}
		}
	}while(('\r' != buff[i]));

	year = (buff[0] - '0')*1000 + (buff[1] - '0')*100 +(buff[2] - '0')*10 + (buff[3] - '0');
	IRTC_SetWriteProtection(RTC, false);	// unlock registers for writing
	set_alarm(sec, min, hour, day, month, year);
	PRINTF("\r\n");
	print_time(RTC_ALARM);
    /* Enable RTC alarm interrupt */
	IRTC_EnableInterrupts(RTC, kIRTC_AlarmInterruptEnable);

	/* Enable at the NVIC */
	EnableIRQ(RTC_IRQn);
}

void select_rtc_compensation(void)
{
	PRINTF("\r\nSelect RTC clock compensation\r\n\t1) Set coarse compensation\r\n\t2) Set fine compensation\r\n");
	uint8_t i=0;
	uint8_t buff[3] = {0};
	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);

		if(('\r' == buff[i]))
		{
			if(('1' != buff[i-1]) && (('2' != buff[i-1]) ))
			{
				i=0;
				PRINTF("\r\nInvalid selection, retry\r\nSelect RTC clock compensation\r\n\t1) Set coarse compensation\r\n\t2) Set fine compensation\r\n");
			}
		}
	}while(('\r' != buff[i]));
	IRTC_SetWriteProtection(RTC, false);	// unlock registers for writing
	if(('1' == buff[i-1]))
	{
		set_coarse_compensation();
		PRINTF("Coarse compensation set");
	}
	else
	{
		set_fine_compensation();
		PRINTF("Fine compensation set");
	}
}

irtc_clock_select_t get_clock_selection (void)
{
	PRINTF("\r\nSelect RTC clock source\r\n\t1) Set FRO 16Khz for VBAT and main\r\n\t2) Set XTAL32khz for VBAT and main\r\n");
	uint8_t i=0;
	uint8_t buff[3] = {0};
	irtc_clock_select_t clock_return;
	do
	{
		i++;
		buff[i] = GETCHAR();
		PUTCHAR(buff[i]);

		if(('\r' == buff[i]))
		{
			if(('1' != buff[i-1]) && (('2' != buff[i-1]) ))
			{
				i=0;
				PRINTF("\r\nInvalid selection, retry\r\n");
			}
		}
	}while(('\r' != buff[i]));
	if(('1' == buff[i-1]))
	{
		clock_return = kIRTC_Clk16K;
	}
	else
	{
		clock_return = kIRTC_Clk32K;
	}
	return clock_return;
}

void next_alarm(void)
{
	int8_t character;
	PRINTF("\r\nSelect the next option\r\n1)\t Display time\r\n2)\t alarm n seconds\r\n3)\t alarm n seconds & minutes\r\n4)\t alarm n seconds & min & hour");
	PRINTF("\r\n5)\t alarm to second & minute & hour & day \r\n6)\t alarm to second & minute & hour & day & month"
			"\r\n7)\t alarm to second & minute & hour & day & month & year"
			"\r\n");
	do
	{
		character = GETCHAR();
		PUTCHAR(character);
		switch (character)
		{
		case '1':
			PRINTF("\r\n\t Display time\r\n");
			print_time(RTC_TIME_READ);
			character = 0; // Change value to force another cycle
			PRINTF("\r\n\t Select another option\r\n");
			break;
		case '2':
			PRINTF("\r\n\t alarm for next n seconds\r\n");
			change_alarm_mode(kRTC_MatchSecMinHr);
			alarm_match_sec();
			break;
		case '3':
			PRINTF("\r\n\t alarm for next n seconds & n min\r\n");
			change_alarm_mode(kRTC_MatchSecMinHr);
			alarm_match_min();
			break;
		case '4':
			PRINTF("\r\n\t alarm to match seconds & min & hour\r\n");
			change_alarm_mode(kRTC_MatchSecMinHr);
			alarm_match_hour();
			break;
		case '5':
			PRINTF("\r\n\t alarm to match seconds & min & hour & day\r\n");
			change_alarm_mode(kRTC_MatchSecMinHrDay);
			alarm_match_day();
			break;
		case '6':
			PRINTF("\r\n\t alarm to match seconds & min & hour & day & month \r\n");
			change_alarm_mode(kRTC_MatchSecMinHrDayMnth);
			alarm_match_month();
			break;
		case '7':
			PRINTF("\r\n\t alarm to match seconds & min & hour & day & month & year \r\n");
			change_alarm_mode(kRTC_MatchSecMinHrDayMnthYr);
			alarm_calendar_mode();
			break;
		default:
			PRINTF("\r\nNon selectable option, select an available option\r\n");
			break;
		}
	}while(('0' > character) || ( '7' < character));
	enable_rtc_interrupts(ALARM_INTERRUPT);
	PRINTF("\r\n\t The time is\r\n");
	print_time(RTC_TIME_READ);
	PRINTF("\r\n\t control 0x%x\r\n", RTC->CTRL);
	PRINTF("\r\n\tWait for alarm trigger\r\n\n");
}
