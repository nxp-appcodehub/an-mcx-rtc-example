/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef POWER_H_
#define POWER_H_

#include "fsl_spc.h"
#include "fsl_wuu.h"
#include "fsl_vbat.h"
#include "fsl_cmc.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"

#define VBAT0_IRQN        VBAT0_IRQn
#define VBAT0_IRQ_HANDLER VBAT0_IRQHandler

#define APP_SPC_RTC_LPISO_VALUE         (0x1EU) /* VDD_USB, VDD_P2, VDD_P3, VDD_P4. */
#define APP_RAM_ARRAYS_PD (0x3F0077FE)

#define WUU0_WAKEUP_RTC_IDX  	2U
#define WUU0_WAKEUP_BUTTON_IDX  7U /* P1_3, SW4 on EVK board. */
#define WUU0_IRQN               WUU_IRQn
#define WUU0_IRQ_HANDLER        WUU_IRQHandler
#define WUU0_WAKEUP_BUTTON_NAME "SW4"

void set_VBAT_config(void);

void set_SPC_config(void);

void setCMCConfiguration(void);

void set_wake_up(void);

void APP_EnterPowerDownMode(void);

#endif /* POWER_H_ */
