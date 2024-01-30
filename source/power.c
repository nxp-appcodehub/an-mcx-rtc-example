/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "power.h"

void VBAT0_IRQHandler(void)
{
    if (VBAT_GetStatusFlags(VBAT0) & kVBAT_StatusFlagWakeupPin)
    {
        VBAT_ClearStatusFlags(VBAT0, kVBAT_StatusFlagWakeupPin);
    }
}

void set_VBAT_config(void)
{
    /* Disable Bandgap to save current consumption. */
    if (VBAT_CheckBandgapEnabled(VBAT0))
    {
        VBAT_EnableBandgapRefreshMode(VBAT0, false);
        VBAT_EnableBandgap(VBAT0, false);
    }
}

void set_SPC_config(void)
{
    status_t status;

    spc_active_mode_regulators_config_t activeModeRegulatorOption;

    /* Disable all modules that controlled by SPC in active mode.. */
    SPC_DisableActiveModeAnalogModules(SPC0, kSPC_controlAllModules);

    /* Disable LVDs and HVDs */
    SPC_EnableActiveModeCoreHighVoltageDetect(SPC0, false);
    SPC_EnableActiveModeCoreLowVoltageDetect(SPC0, false);
    SPC_EnableActiveModeSystemHighVoltageDetect(SPC0, false);
    SPC_EnableActiveModeSystemLowVoltageDetect(SPC0, false);
    SPC_EnableActiveModeIOHighVoltageDetect(SPC0, false);
    SPC_EnableActiveModeIOLowVoltageDetect(SPC0, false);

    activeModeRegulatorOption.bandgapMode = kSPC_BandgapEnabledBufferDisabled;
    activeModeRegulatorOption.lpBuff      = false;
    /* DCDC output voltage is 1.1V in active mode. */
    activeModeRegulatorOption.DCDCOption.DCDCVoltage             = kSPC_DCDC_NormalVoltage;
    activeModeRegulatorOption.DCDCOption.DCDCDriveStrength       = kSPC_DCDC_LowDriveStrength;
    activeModeRegulatorOption.SysLDOOption.SysLDOVoltage         = kSPC_SysLDO_NormalVoltage;
    activeModeRegulatorOption.SysLDOOption.SysLDODriveStrength   = kSPC_SysLDO_LowDriveStrength;
    activeModeRegulatorOption.CoreLDOOption.CoreLDOVoltage       = kSPC_CoreLDO_MidDriveVoltage;
    activeModeRegulatorOption.CoreLDOOption.CoreLDODriveStrength = kSPC_CoreLDO_NormalDriveStrength;

    status = SPC_SetActiveModeRegulatorsConfig(SPC0, &activeModeRegulatorOption);
    /* Disable Vdd Core Glitch detector in active mode. */
    SPC_DisableActiveModeVddCoreGlitchDetect(SPC0, true);
    if (status != kStatus_Success)
    {
        PRINTF("Fail to set regulators in Active mode.");
        return;
    }
    while (SPC_GetBusyStatusFlag(SPC0))
        ;

    SPC_DisableLowPowerModeAnalogModules(SPC0, kSPC_controlAllModules);
    SPC_SetLowPowerWakeUpDelay(SPC0, 0xFF);
    spc_lowpower_mode_regulators_config_t lowPowerRegulatorOption;

    lowPowerRegulatorOption.lpIREF      = false;
    lowPowerRegulatorOption.bandgapMode = kSPC_BandgapDisabled;
    lowPowerRegulatorOption.lpBuff      = false;
    /* Enable Core IVS, which is only useful in power down mode.  */
    lowPowerRegulatorOption.CoreIVS = true;
    /* DCDC output voltage is 1.0V in some low power mode(Deep sleep, Power Down). DCDC is disabled in Deep Power Down.
     */
    lowPowerRegulatorOption.DCDCOption.DCDCVoltage             = kSPC_DCDC_MidVoltage;
    lowPowerRegulatorOption.DCDCOption.DCDCDriveStrength       = kSPC_DCDC_LowDriveStrength;
    lowPowerRegulatorOption.SysLDOOption.SysLDODriveStrength   = kSPC_SysLDO_LowDriveStrength;
    lowPowerRegulatorOption.CoreLDOOption.CoreLDOVoltage       = kSPC_CoreLDO_MidDriveVoltage;
    lowPowerRegulatorOption.CoreLDOOption.CoreLDODriveStrength = kSPC_CoreLDO_LowDriveStrength;

    status = SPC_SetLowPowerModeRegulatorsConfig(SPC0, &lowPowerRegulatorOption);
    /* Disable Vdd Core Glitch detector in low power mode. */
    SPC_DisableLowPowerModeVddCoreGlitchDetect(SPC0, true);
    if (status != kStatus_Success)
    {
        PRINTF("Fail to set regulators in Low Power Mode.");
        return;
    }
    while (SPC_GetBusyStatusFlag(SPC0))
        ;

    /* Disable LDO_CORE since it is bypassed. */
    SPC_EnableCoreLDORegulator(SPC0, false);

    /* Enable Low power request output to observe the entry/exit of
     * low power modes(including: deep sleep mode, power down mode, and deep power down mode).
     */
    spc_lowpower_request_config_t lpReqConfig = {
        .enable   = true,
        .override = kSPC_LowPowerRequestNotForced,
        .polarity = kSPC_LowTruePolarity,
    };

    SPC_SetLowPowerRequestConfig(SPC0, &lpReqConfig);
}

void setCMCConfiguration(void)
{
    /* Disable low power debug. */
    CMC_EnableDebugOperation(CMC0, true);
    /* Allow all power mode */
    CMC_SetPowerModeProtection(CMC0, kCMC_AllowAllLowPowerModes);

    /* Disable flash memory accesses and place flash memory in low-power state whenever the core clock
       is gated. And an attempt to access the flash memory will cause the flash memory to exit low-power
       state for the duration of the flash memory access. */
    CMC_ConfigFlashMode(CMC0, true, true, false);
}

void set_wake_up(void)
{
	WUU_SetInternalWakeUpModulesConfig(WUU0, WUU0_WAKEUP_RTC_IDX, kWUU_InternalModuleInterrupt);

	/* Isolate some power domains that are not used in low power modes.*/
	SPC_SetExternalVoltageDomainsConfig(SPC0, APP_SPC_RTC_LPISO_VALUE, 0x0U);

}

void APP_EnterPowerDownMode(void)
{
    cmc_power_domain_config_t config;

    /* Power off some RAM blocks. */
    //CMC_PowerOffSRAMLowPowerOnly(CMC0, APP_RAM_ARRAYS_PD);

    config.clock_mode  = kCMC_GateAllSystemClocksEnterLowPowerMode; // kCMC_GateNoneClock kCMC_GateAllSystemClocksEnterLowPowerMode
    config.main_domain = kCMC_DeepPowerDown; // kCMC_DeepPowerDown
    config.wake_domain = kCMC_DeepPowerDown; //
    /* Power off some RAM blocks which are powered by VBAT? */

    CMC_EnterLowPowerMode(CMC0, &config);
}
