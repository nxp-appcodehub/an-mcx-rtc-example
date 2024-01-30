/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RTC_H_
#define RTC_H_

#include "fsl_irtc.h"
#include "pin_mux.h"
#include "board.h"
#include "fsl_vbat.h"

#define WAKE_TIMER_INTERVAL					60
#define WAKE_TIMER_INTERRUPT_ENABLE 		0x200000

typedef enum {RTC_SETTED_TIME = 0, RTC_TIME_READ, RTC_ALARM} ALARM_SELECT;

typedef enum {ALARM_INTERRUPT = 0, WAKE_TIMER_INTERRUPT} RTC_INTERRUPT_SELECT;

void init_rtc(irtc_alarm_match_t alarm, irtc_clock_select_t clock);

uint8_t set_time(uint8_t second, uint8_t minute, uint8_t hour ,uint8_t day, uint8_t month, uint16_t year);

uint8_t set_alarm(uint8_t second, uint8_t minute, uint8_t hour ,uint8_t day, uint8_t month, uint16_t year);

void read_rtc_time(void);

irtc_datetime_t* get_rtc_time(void);

void print_time(ALARM_SELECT selector);

uint8_t get_alarm_status(void);

void clear_alarm_status(void);

void set_coarse_compensation(void);

void set_fine_compensation(void);

void alarm_loop(void);

void alarm_time_loop(uint8_t min, uint8_t sec);

void set_wake_timer(void);

void enable_rtc_interrupts(RTC_INTERRUPT_SELECT interrupt);

void change_alarm_mode(irtc_alarm_match_t alarm);

#endif /* RTC_H_ */
