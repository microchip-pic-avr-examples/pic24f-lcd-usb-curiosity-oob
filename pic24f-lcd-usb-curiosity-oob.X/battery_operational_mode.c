/*******************************************************************************
Copyright 2019 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/

#include "operational_mode.h"
#include "power.h"
#include "lcd1.h"
#include "adc.h"
#include "mcc_generated_files/rtcc.h"
#include "tc77.h"
#include "led1.h"
#include "led2.h"
#include "rgb_led3.h"
#include "lcd_demo.h"

#include <xc.h>

static void Initialize(void);
static void Deinitialize(void);
static void Tasks(void);
static void UpdateBatteryStatusIcon(void);

static struct tm date_time;

const struct OPERATIONAL_MODE battery_operational_mode = {
    &Initialize,
    &Deinitialize,
    &Tasks
};

static void Initialize(void)
{        
    LCD_DEMO_LowPowerModeEnable(true);
    
    TC77_Shutdown();
    
    LED1_Off();
    LED2_Off();
    
    RGB_LED3_Off();
}

static void Tasks(void)
{
    LED1_On();
    
    POWER_SetMode(POWER_MODE_LOW);
    
    RTCC_TimeGet(&date_time);
    LCD_DEMO_PrintTime(date_time.tm_hour, date_time.tm_min);

    //Enable and configure the ADC so it can sample the battery voltage.
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_BAND_GAP);
    
    UpdateBatteryStatusIcon();
    
    POWER_SetMode(POWER_MODE_SLEEP);
    
    LED1_Off();
       
    /* We will be woken up by one of the interrupts that are enabled:
     *  - RTCC (1 time per minute)
     *  - Interrupt on pin change for the USB power detection.
     */
    Sleep();   
}

static void Deinitialize(void)
{    
    POWER_SetMode(POWER_MODE_FULL);
}

static void UpdateBatteryStatusIcon(void)
{
    double battery_voltage = POWER_GetBatteryVoltage();
    enum BATTERY_STATUS battery_status = BATTERY_STATUS_UNKNOWN;
    
    if(battery_voltage < 2.6)
    {
        battery_status = BATTERY_STATUS_CRITICAL;
    }
    else if(battery_voltage < 2.7)
    {
        battery_status = BATTERY_STATUS_LOW;
    }
    else if(battery_voltage < 2.8)
    {
        battery_status = BATTERY_STATUS_MEDIUM;
    }
    else
    {
        battery_status = BATTERY_STATUS_FULL;
    }
    
    LCD_DEMO_SetBatteryStatus(battery_status);
}
