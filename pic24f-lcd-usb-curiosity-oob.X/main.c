/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.165.1
        Device            :  PIC24FJ128GL306
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.41
        MPLAB 	          :  MPLAB X v5.30
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include <stddef.h>

#include "application/usb_operational_mode.h"
#include "application/battery_operational_mode.h"
#include "mcc_generated_files/rtcc.h"
#include "bsp/power.h"
#include "application/lcd_demo.h"
#include "bsp/build_time.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/lcd_segments.h"

static void SwitchOperatoinalMode(enum POWER_SOURCE new_source);

static enum POWER_SOURCE current_source;
static const struct OPERATIONAL_MODE *operational_mode;

/*******************************************************************************
  GETTING STARTED
  -----------------------------------------------------------------------------
  To run this demo, please refer to the readme.txt file that is provided with
  this project.  You can find this file attached to the project in the 
  "Documentation" logical folder in the project view in the IDE.
  
  You can also locate the readme.txt file on next to the project folder where
  this demo was extracted.  
  
  The readme.txt contains the details of how to run the demo.
  
  There is an additional hardware.txt file also in the "Documentation" logical
  folder in the project that summarizes the hardware connections of the board.
 ******************************************************************************/

int main(void)
{
    struct tm build_time;
    
    enum POWER_SOURCE new_source;
    
    SYSTEM_Initialize();
    
    LCD_MICROCHIP1_On();
    BUILDTIME_Get(&build_time);
    RTCC_TimeSet(&build_time);
    
    operational_mode = NULL;
    current_source = POWER_SOURCE_UNKNOWN;
    
    new_source = POWER_GetSource();
    
    while(1)
    {
        if(new_source != current_source)
        {
            current_source = new_source;
            SwitchOperatoinalMode(new_source);
        }
        
        do
        {
            operational_mode->Tasks();
            new_source = POWER_GetSource();
            
        } while( current_source == new_source );
    }
    
    return 0;
}

static void SwitchOperatoinalMode(enum POWER_SOURCE new_source)
{
    if(operational_mode != NULL)
    {
        operational_mode->Deinitialize();
    }
    
    switch(new_source)
    {
        case POWER_SOURCE_USB:
            operational_mode = &usb_operational_mode;
            break;

        case POWER_SOURCE_BATTERY:
            operational_mode = &battery_operational_mode;
            break;
            
        default:
            break;
    }
    
    if(operational_mode != NULL)
    {
        operational_mode->Initialize();
    }
}