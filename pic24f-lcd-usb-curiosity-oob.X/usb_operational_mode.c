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

#include "led1.h"
#include "led2.h"
#include "buttons.h"
#include "adc.h"
#include "rgb_led3.h"
#include "timer_1ms.h"
#include "mcc_generated_files/usb/usb.h"
#include "lcd1.h"
#include "operational_mode.h"
#include "mcc_generated_files/rtcc.h"
#include "tc77.h"
#include "lcd_demo.h"

//------------------------------------------------------------------------------
//Application related definitions
//------------------------------------------------------------------------------
#define BUTTON_DEBOUCE_TIME_MS      20

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
static void UpdatePotentiometerValue(void);
static void UpdateRGB(void);
static void UpdateUARTPrintout(void);
static void UpdateSegmentedLCD(void);

static void Initialize(void);
static void Deinitialize(void);
static void Tasks(void);
static void PrintHeader(void);

//------------------------------------------------------------------------------
//Global variables
//------------------------------------------------------------------------------
static volatile BUTTON_COLOR button_color = BUTTON_COLOR_RED;
static volatile bool update_printout = true;
static volatile bool update_temperature = true;
static volatile enum DISPLAY_MODE display_mode = DISPLAY_PIC24;
static bool print_header = false;

static uint16_t potentiometer;
static uint16_t red = 600;
static uint16_t green = 300;
static uint16_t blue = 150;
static double temperature;
static struct tm date_time;
static uint16_t adc_samples[16];
static const uint16_t adc_sample_buffer_size = sizeof(adc_samples)/sizeof(uint16_t);

const struct OPERATIONAL_MODE usb_operational_mode = {
    &Initialize,
    &Deinitialize,
    &Tasks
};

//------------------------------------------------------------------------------
//Functions
//------------------------------------------------------------------------------
static void Initialize(void)
{
    unsigned int i;
    //Configure the pushbutton pins as digital inputs.
    BUTTON_Enable(BUTTON_S1);
    BUTTON_Enable(BUTTON_S2);
   
    LED1_Off();
    LED2_Off();
    
    RGB_LED3_SetColor(red, green, blue);
    RGB_LED3_On();
    
    //Enable and configure the ADC so it can sample the potentiometer.
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);
    
    //Turn on a timer, so to generate periodic interrupts.
    TIMER_SetConfiguration(TIMER_CONFIGURATION_1MS);
    
    //Register the ButtonDebounce() callback function, so it gets called periodically
    //when the timer interrupts occur (in this case at 1:1 rate, so ButtonDebounce()
    //executes once per 1ms).
    TIMER_RequestTick(&ButtonS1Debounce, 1);
    TIMER_RequestTick(&ButtonS2Debounce, 1);
    TIMER_RequestTick(&UpdatePrintout, 10);
    TIMER_RequestTick(&UpdateTemperature, 1000);
       
    USBDeviceInit();
    USBDeviceAttach();
        
    LCD1_Initialize();
    LCD_DEMO_LowPowerModeEnable(false);
    LCD_DEMO_SetBatteryStatus(BATTERY_STATUS_UNKNOWN);
    
    TC77_Initialize();
    
    /* Pre-fill the ADC sample buffer */
    for(i=0; i<adc_sample_buffer_size; i++)
    {
        adc_samples[i] = ADC_Read16bit(ADC_CHANNEL_POTENTIOMETER);
    }
    
    print_header = false;
}

void Tasks(void)
{   
    PrintHeader();
    
    if(update_temperature == true)
    {
        update_temperature = false;
        temperature = TC77_GetTemperatureCelsius();
    }

    UpdatePotentiometerValue();

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

static void Deinitialize(void)
{
    TIMER_SetConfiguration(TIMER_CONFIGURATION_OFF);
    RGB_LED3_Off();
}

static void UpdatePotentiometerValue(void)
{
    static unsigned int current_sample = 0;
    uint32_t average = 0;
    unsigned int i;
    
    //Fetch an ADC sample from the potentiometer
    adc_samples[current_sample++] = ADC_Read16bit(ADC_CHANNEL_POTENTIOMETER);
    
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
            LCD_DEMO_PrintPot(potentiometer>>4);
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
    if(BUTTON_IsPressed(BUTTON_S1))
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
    if(BUTTON_IsPressed(BUTTON_S2))
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