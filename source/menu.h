/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef MENU_H_
#define MENU_H_

#include "RTC.h"
#include "fsl_debug_console.h"
#include "fsl_common.h"

void init_menu(void);

void set_hour(void);

void alarm_calendar_mode(void);

void select_rtc_compensation(void);

void alarm_match_month(void);

void alarm_match_day(void);

void alarm_match_hour(void);

void alarm_match_min(void);

void alarm_match_sec(void);

irtc_clock_select_t get_clock_selection (void);

void next_alarm(void);

#endif /* MENU_H_ */
