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

#include <stdio.h>
#include "timer_1ms.h"
#include "timer_1ms.h"
#include "lcd_demo.h"
#include "operational_mode.h"
#include "bsp/buttons.h"
#include "bsp/led1.h"
#include "bsp/led2.h"
#include "bsp/tc77.h"
#include "bsp/rgb_led3.h"
#include "mcc_generated_files/usb/usb_device.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/lcd.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/adc1.h"
#include "mcc_generated_files/interrupt_manager.h"
#include "mcc_generated_files/clock.h"
#include "mcc_generated_files/mccp5_compare.h"
#include "mcc_generated_files/mccp6_compare.h"
#include "mcc_generated_files/mccp4_compare.h"
#include "mcc_generated_files/tmr3.h"
#include "mcc_generated_files/spi1.h"
#include "mcc_generated_files/rtcc.h"
#include "mcc_generated_files/usb/usb_device.h"

//------------------------------------------------------------------------------
//Application related definitions
//------------------------------------------------------------------------------
#define BUTTON_DEBOUCE_TIME_MS      20
#define POTENTIOMETER_ADC_CHANNEL   5
#define SWITCH_DEBOUNCE_RATE  (uint32_t)1
#define INFO_PRINTOUT_UPDATE_RATE   (uint32_t)10
#define TEMPERATURE_UPDATE_TIMER_RATE    (uint32_t)1000
#define RED_COLOR_LED_INTENSITY     600
#define GREEN_COLOR_LED_INTENSITY   300
#define BLUE_COLOR_LED_INTENSITY    150

static uint16_t adc_samples[16];
static const uint16_t adc_sample_buffer_size = sizeof(adc_samples)/sizeof(uint16_t);


typedef enum
{
    BUTTON_COLOR_RED = 0,
    BUTTON_COLOR_GREEN = 1,
    BUTTON_COLOR_BLUE = 2
} BUTTON_COLOR;

enum DISPLAY_MODE
{
    DISPLAY_POT,
    DISPLAY_TEMPERATURE,
    DISPLAY_TIME,
    DISPLAY_PIC24
};

//------------------------------------------------------------------------------
//Private prototypes
//------------------------------------------------------------------------------
static void ButtonS1Debounce(void);
static void ButtonS2Debounce(void);
static void UpdatePrintout(void);
static void UpdateTemperature(void);
static void UpdatePotentiometer(void);
static void UpdateRGB(void);
static void UpdateUARTPrintout(void);
static void UpdateSegmentedLCD(void);

static void RegisterTimerCallBackFunctions(void);
static void USBPowerModeTask_Initialize(void);
static void USBPowerModeTask_Deinitialization(void);
static void USBPowerModeTasks(void);
static void PrintHeader(void);


static void PeripheralModules_Initialize(void);
//------------------------------------------------------------------------------
//Global variables
//------------------------------------------------------------------------------
static volatile BUTTON_COLOR button_color = BUTTON_COLOR_RED;
static volatile bool update_printout = true;
static volatile bool update_temperature = true;
static volatile enum DISPLAY_MODE display_mode = DISPLAY_PIC24;

static uint16_t potentiometer;
static uint16_t red = RED_COLOR_LED_INTENSITY;
static uint16_t green = GREEN_COLOR_LED_INTENSITY;
static uint16_t blue = BLUE_COLOR_LED_INTENSITY;
static double temperature;
static struct tm date_time;
static bool print_header = false;

const struct OPERATIONAL_MODE usb_operational_mode = {
    &USBPowerModeTask_Initialize,
    &USBPowerModeTask_Deinitialization,
    &USBPowerModeTasks
};

//------------------------------------------------------------------------------
//Functions
//------------------------------------------------------------------------------
static void USBPowerModeTask_Initialize(void)
{
    PeripheralModules_Initialize();
    LCD_CLEAR();
    
    display_mode = DISPLAY_PIC24;
    button_color = BUTTON_COLOR_RED;
    //Configure the pushbutton pins as digital inputs.
    LED1_Off();
    LED2_Off();
    // Set RGB Color
    RGB_LED3_SetColor(red, green, blue);
    RGB_LED3_On();
    //Register Timer call back functions
    RegisterTimerCallBackFunctions();
    
    USBDeviceInit();
    USBDeviceAttach();
   
	LCD_SetPowerMode(LCD_POWER_MODE_HIGH);
    LCD_DEMO_SetBatteryStatus(BATTERY_STATUS_UNKNOWN);
}

static void PeripheralModules_Initialize(void)
{
    PIN_MANAGER_Initialize();
    CLOCK_Initialize();
    INTERRUPT_Initialize();
    MCCP4_COMPARE_Initialize();
    MCCP5_COMPARE_Initialize();
    MCCP6_COMPARE_Initialize();
    SPI1_Initialize();
    ADC1_Initialize();
    TMR3_Initialize();
}

static void RegisterTimerCallBackFunctions(void)
{
     //Turn on a timer, so to generate periodic interrupts.
    TIMER_SetConfiguration(TIMER_CONFIGURATION_1MS);
    
    //Register the ButtonDebounce() callback function, so it gets called periodically
    //when the timer interrupts occur (in this case at 1:1 rate, so ButtonDebounce()
    //executes once per 1ms).
    TIMER_RequestTick(&ButtonS1Debounce, SWITCH_DEBOUNCE_RATE);
    TIMER_RequestTick(&ButtonS2Debounce, SWITCH_DEBOUNCE_RATE);
    TIMER_RequestTick(&UpdatePrintout, INFO_PRINTOUT_UPDATE_RATE);
    TIMER_RequestTick(&UpdateTemperature, TEMPERATURE_UPDATE_TIMER_RATE);
}

void USBPowerModeTasks(void)
{   
    PrintHeader();
    
    if(update_temperature == true)
    {
        update_temperature = false;
        temperature = TC77_GetTemperatureCelsius();
    }
    //Update Potentiometer value
    UpdatePotentiometer();
    //Update RGB LED value
    UpdateRGB();

    if(update_printout == true)
    {
        update_printout = false;
        RTCC_TimeGet(&date_time);
        UpdateUARTPrintout();
        UpdateSegmentedLCD();
    }
}
    
static void PrintHeader(void)
{
    if( (print_header == false) && (USBGetDeviceState() == CONFIGURED_STATE) )
    {
        printf("\033[2J");      //Clear screen
        printf("\033[0;0f");    //return cursor to 0,0
        printf("\033[?25l");    //disable cursor

        printf("------------------------------------------------------------------\r\n");
        printf("PIC24F LCD Curiosity Development Board Demo                       \r\n");
        printf("------------------------------------------------------------------\r\n");
        printf("S1 - controls LED1, changes active RGB color\r\n");
        printf("S2 - controls LED2, cycle what is on LCD display\r\n");
        printf("Potentiometer - controls active RGB color intensity\r\n");
        printf("\r\n");
        
        print_header = true;
    }
}
static void USBPowerModeTask_Deinitialization(void)
{
    TIMER_SetConfiguration(TIMER_CONFIGURATION_OFF);
    RGB_LED3_Off();
}

static void UpdatePotentiometer(void)
{
    static unsigned int current_sample = 0;
    uint32_t average = 0;
    
    volatile uint16_t i=0;
   //Enable ADC module
    ADC1_Enable();
    
    //Select the PotentioMeter ADC Channel
    ADC1_ChannelSelect(channel_AN5);
    //Start Sampling
    ADC1_SoftwareTriggerEnable();
    //ADC sampling delay
    for(i=0;i<10000;i++)
    {
        //Do Nothing
    }
    ADC1_SoftwareTriggerDisable();
    //Check if the ADC conversion is completed
    while(!ADC1_IsConversionComplete(channel_AN5))
    {
        //Do Nothing
    }
    // Get the Potentiometer ADC values 
   //Fetch an ADC sample from the potentiometer
    adc_samples[current_sample++] = ADC1_ConversionResultGet(channel_AN5);
    
    for(i=0; i<adc_sample_buffer_size; i++)
    {
        average += adc_samples[i];
    }
    
    if(current_sample >= adc_sample_buffer_size)
    {
        current_sample = 0;
    }
    
    potentiometer = average/adc_sample_buffer_size;
}

static void UpdateRGB(void)
{
    //Use the potentiometer ADC value to set the brightness of the currently
    //selected color channel on the RGB LED.  The "currently selected channel"
    //is manually selected by the user at runtime by pressing the pushbuttons.
    switch(button_color)
    {
        case BUTTON_COLOR_RED:
            red = (potentiometer);
            break;

        case BUTTON_COLOR_GREEN:
            green = (potentiometer);
            break;

        case BUTTON_COLOR_BLUE:
            blue = (potentiometer);
            break;

        default:
            break;
    }

    RGB_LED3_SetColor(red, green, blue);
}

static void UpdateUARTPrintout(void)
{
    printf("\033[8;0f");    //move cursor to row 0, column 0

    printf("Potentiometer: %i/4095    \r\n", potentiometer>>4);
    printf("Current color (r,g,b): %i, %i, %i            \r\n", red, green, blue);
    printf("Active color: ");

    switch(button_color)
    {
        case BUTTON_COLOR_RED:
            printf("red  \r\n");
            break;

        case BUTTON_COLOR_GREEN:
            printf("green\r\n");
            break;

        case BUTTON_COLOR_BLUE:
            printf("blue \r\n");
            break;

        default:
            break;
    }

    printf("Temperature: %.2f C                                               \r\n", temperature);
    printf("Date/Time: %04i/%02i/%02i %02i:%02i:%02i", 2000+date_time.tm_year, date_time.tm_mon, date_time.tm_mday, date_time.tm_hour, date_time.tm_min, date_time.tm_sec);              
}

static void UpdateSegmentedLCD(void)
{
    switch(display_mode)
    {
        case DISPLAY_PIC24:
            LCD_DEMO_PrintPIC24();
            break;

        case DISPLAY_POT:
            LCD_DEMO_PrintPot(potentiometer);
            break;

        case DISPLAY_TIME:
            LCD_DEMO_PrintTime(date_time.tm_hour, date_time.tm_min);
            break;

        case DISPLAY_TEMPERATURE:
            LCD_DEMO_PrintTemperature(temperature);
            break;

        default:
            LCD_DEMO_PrintPIC24();
            break;
    }
}

//Helper function that advances the currently selected RGB color channel that
//is to be adjusted next.  This function is called in response to user pushbutton
//press events.
static void ChangeColor(void)
{         
    switch(button_color)
    {
        case BUTTON_COLOR_RED:
            button_color = BUTTON_COLOR_GREEN;
            break;

        case BUTTON_COLOR_GREEN:
            button_color = BUTTON_COLOR_BLUE;
            break;

        case BUTTON_COLOR_BLUE:
            button_color = BUTTON_COLOR_RED;
            break;

        default:
            button_color = BUTTON_COLOR_RED;
            break;
    }
}

static void ChangeDisplayMode(void)
{
    switch(display_mode)
    {
        case DISPLAY_PIC24:
            display_mode = DISPLAY_POT;
            break;

        case DISPLAY_POT:
            display_mode = DISPLAY_TIME;
            break;

        case DISPLAY_TIME:
            display_mode = DISPLAY_TEMPERATURE;
            break;
            
        case DISPLAY_TEMPERATURE:
            display_mode = DISPLAY_PIC24;
            break;

        default:
            display_mode = DISPLAY_PIC24;
            break;
    }
}


//This callback function gets called periodically (1/1ms) by the timer interrupt event
//handler.  This function is used to periodically sample the pushbutton and implements
//a de-bounce algorithm to reject spurious chatter that can occur during press events.
static void ButtonS1Debounce(void)
{
    static uint16_t debounceCounter = 0;
    
    //Sample the button S1 to see if it is currently pressed or not.
    if(!BUTTON_IsPressed(BUTTON_S1))
    {
        //The button is currently pressed.  Turn on the general purpose LED.
        LED1_On();
        
        //Check if the de-bounce blanking interval has been satisfied.  If so,
        //advance the RGB color channel user control selector.
        if(debounceCounter == 0)
        {
            ChangeColor();   
        }
        
        //Reset the de-bounce countdown timer, so a new color change operation
        //won't occur until the button is released and remains continuously released 
        //for at least BUTTON_DEBOUCE_TIME_MS.
        debounceCounter = BUTTON_DEBOUCE_TIME_MS;
    }
    else
    {
        //The button is not currently pressed.  Turn off the LED.
        LED1_Off();  
        
        //Allow the de-bounce interval timer to count down, until it reaches 0.
        //Once it reaches 0, the button is effectively "re-armed".
        if(debounceCounter != 0)
        {
            debounceCounter--;
        }
    }  
}

//This callback function gets called periodically (1/1ms) by the timer interrupt event
//handler.  This function is used to periodically sample the pushbutton and implements
//a de-bounce algorithm to reject spurious chatter that can occur during press events.
static void ButtonS2Debounce(void)
{
    static uint16_t debounceCounter = 0;

    //Sample the button S2 to see if it is currently pressed or not.
    if(!BUTTON_IsPressed(BUTTON_S2))
    {
        //The button is currently pressed.  Turn on the general purpose LED.
        LED2_On();
        
        //Check if the de-bounce blanking interval has been satisfied.  If so,
        //advance the RGB color channel user control selector.
        if(debounceCounter == 0)
        {
            ChangeDisplayMode();   
        }
        
        //Reset the de-bounce countdown timer, so a new color change operation
        //won't occur until the button is released and remains continuously released 
        //for at least BUTTON_DEBOUCE_TIME_MS.
        debounceCounter = BUTTON_DEBOUCE_TIME_MS;
    }
    else
    {
        //The button is not currently pressed.  Turn off the LED.
        LED2_Off(); 
        
        //Allow the de-bounce interval timer to count down, until it reaches 0.
        //Once it reaches 0, the button is effectively "re-armed".
        if(debounceCounter != 0)
        {
            debounceCounter--;
        }
    }    
}

static void UpdatePrintout(void)
{
    update_printout = true;
}

static void UpdateTemperature(void)
{
    update_temperature = true;
}