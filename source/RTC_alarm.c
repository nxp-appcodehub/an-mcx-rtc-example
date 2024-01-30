/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/**
 * @file    RTC_alarm.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "board.h"
//#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MCXN947_cm33_core0.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_lpuart.h"
#include "fsl_common.h"
#include "RTC.h"
#include "menu.h"
#include "power.h"
#define TEST_TIME 0
#define GPIO_PIN_TOGGLE 1
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* TO UPDATE */
#define APP_DEBUG_CONSOLE_RX_PORT   PORT1
#define APP_DEBUG_CONSOLE_RX_GPIO   GPIO1
#define APP_DEBUG_CONSOLE_RX_PIN    8U
#define APP_DEBUG_CONSOLE_RX_PINMUX kPORT_MuxAlt2
/* TO UPDATE */
#define APP_DEBUG_CONSOLE_TX_PORT   PORT1
#define APP_DEBUG_CONSOLE_TX_GPIO   GPIO1
#define APP_DEBUG_CONSOLE_TX_PIN    9U
#define APP_DEBUG_CONSOLE_TX_PINMUX kPORT_MuxAlt2

/*GIPO TOGGLE PIN*/
#define BOARD_GPIO_TOGGLE    GPIO0
#define BOARD_GPIO_TOGGLE_PIN  30
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void APP_PowerPreSwitchHook(void);

void APP_DeinitDebugConsole(void);


/*******************************************************************************
 * Variables
 ******************************************************************************/
irtc_datetime_t time;
uint8_t set = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/




/*******************************************************************************/


int main(void) {
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();


    /* attach FRO 12M to FLEXCOMM4 (debug console) */

	CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

#if GPIO_PIN_TOGGLE == 1

    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        0,
    };

    /* enable clock for GPIO*/
	CLOCK_EnableClock(kCLOCK_Gpio0);

    /* Init output LED GPIO. */
	GPIO_PinInit(BOARD_GPIO_TOGGLE, BOARD_GPIO_TOGGLE_PIN, &led_config);
	//set toggle

	GPIO_PortToggle(BOARD_GPIO_TOGGLE, 1u << BOARD_GPIO_TOGGLE_PIN);
#endif

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    set_VBAT_config();
    set_SPC_config();

    if ((CMC_GetSystemResetStatus(CMC0) & kCMC_WakeUpReset) != 0UL)
    {
    	//This code section will only be reached after wake up from Power Down/Deep Power down
        /* Release the I/O pads and certain peripherals to normal run mode state, for in Power Down mode
         * they will be in a latched state. */
    	CLOCK_EnableClock(kCLOCK_Rtc0);
		SPC_ClearPeriphIOIsolationFlag(SPC0);
		PRINTF("RTC Example finished, alarmed reached\r\n");
#if TEST_TIME == 1
		alarm_time_loop(0, 5);
		print_time(RTC_SETTED_TIME);
		PRINTF("\tNEXT STEP \r\n");
#else


		if(WAKE_TIMER_INTERRUPT_ENABLE && (IRTC_GetEnabledInterrupts(RTC)))
		{
			/* Unlock to allow register write operation */
			IRTC_SetWriteProtection(RTC, false);
			//reload wake timer
			IRTC_SetWakeupCount(RTC, true, WAKE_TIMER_INTERVAL);
			PRINTF("Reload Wake timer\r\n");
		}
		else
		{
			//display menu for setting a new alarm
			print_time(RTC_TIME_READ);
			next_alarm();
		}
#endif

    }
    else
    {
   //init rtc config
	PRINTF("\r\nRTC comensation example!!\r\n");

#if TEST_TIME == 1
	PRINTF("\tPress any button to continue\r\n");
	GETCHAR();
	uint8_t clock_select = kIRTC_Clk16K;
	init_rtc(kRTC_MatchSecMinHrDayMnthYr, clock_select);
	IRTC_SetWriteProtection(RTC, false);	// unlock registers for writing
	set_fine_compensation();
	set_time(5, 5, 5, 29, 6, 2001);
	alarm_time_loop(0, 5);
	print_time(RTC_SETTED_TIME);
	PRINTF("\tNEXT STEP \r\n");
#else
    init_menu();
#endif
    }

    while(1)
    {
		if ((CMC_GetSystemResetStatus(CMC0) & kCMC_WakeUpReset) != 0UL)
		{
			/* Close ISO flags. */
			SPC_ClearPeriphIOIsolationFlag(SPC0);


		}
		/* Clear CORE_MAIN power domain's low power request flag. */
		SPC_ClearPowerDomainLowPowerRequestFlag(SPC0, kSPC_PowerDomain0);
		/* Clear CORE_WAKE power domain's low power request flag. */
		SPC_ClearPowerDomainLowPowerRequestFlag(SPC0, kSPC_PowerDomain1);
		SPC_ClearLowPowerRequest(SPC0);

		/* Normal start. */
		setCMCConfiguration();

		set_wake_up();
		PRINTF("ENTRY DEEP POWER DOWN MODE \r\n");

#if GPIO_PIN_TOGGLE == 1
	//set toggle to off
	GPIO_PortToggle(BOARD_GPIO_TOGGLE, 1u << BOARD_GPIO_TOGGLE_PIN);
#endif
		APP_PowerPreSwitchHook();
		APP_EnterPowerDownMode();

    }
    return 0 ;
}




static void APP_PowerPreSwitchHook(void)
{
    /* Wait for debug console output finished. */
    while (!(kLPUART_TransmissionCompleteFlag & LPUART_GetStatusFlags((LPUART_Type *)BOARD_DEBUG_UART_BASEADDR)))
    {
    }
    APP_DeinitDebugConsole();

    /* Disable unused clocks */
    CLOCK_DisableClock(kCLOCK_Rom);
    CLOCK_DisableClock(kCLOCK_PkcRam);
    CLOCK_DisableClock(kCLOCK_Gdet);
    CLOCK_DisableClock(kCLOCK_Pkc);
    CLOCK_DisableClock(kCLOCK_Css);

#if GPIO_PIN_TOGGLE == 1
    CLOCK_DisableClock(kCLOCK_Port0);
    /* enable clock for GPIO*/
    CLOCK_DisableClock(kCLOCK_Gpio0);
#endif
}

void APP_DeinitDebugConsole(void)
{
    DbgConsole_Deinit();
    PORT_SetPinMux(APP_DEBUG_CONSOLE_RX_PORT, APP_DEBUG_CONSOLE_RX_PIN, kPORT_PinDisabledOrAnalog);
    PORT_SetPinMux(APP_DEBUG_CONSOLE_TX_PORT, APP_DEBUG_CONSOLE_TX_PIN, kPORT_PinDisabledOrAnalog);
}


