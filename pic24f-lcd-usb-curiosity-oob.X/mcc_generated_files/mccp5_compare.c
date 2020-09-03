/**
  MCCP5 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    mccp5.c

  @Summary
    This is the generated driver implementation file for the MCCP5 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides implementations for driver APIs for MCCP5. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.169.0
        Device            :  PIC24FJ512GU410
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.50
        MPLAB             :  MPLAB X v5.40
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

#include "mccp5_compare.h"

/** OC Mode.

  @Summary
    Defines the OC Mode.

  @Description
    This data type defines the OC Mode of operation.

*/

static uint16_t         gMCCP5Mode;

/**
  Section: Driver Interface
*/


void MCCP5_COMPARE_Initialize (void)
{
    // CCPON enabled; MOD Dual Edge Compare; CCSEL disabled; CCPSIDL disabled; T32 16 Bit; CCPSLP disabled; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; 
    CCP5CON1L = (0x8004 & 0x7FFF); //Disabling CCPON bit
    //RTRGEN disabled; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; OPS Each Time Base Period Match; SYNC MCCP5 Timer; OPSSRC Timer Interrupt Event; 
    CCP5CON1H = 0x00;
    //ASDGM disabled; SSDG disabled; ASDG 0; PWMRSEN disabled; 
    CCP5CON2L = 0x00;
    //ICGSM Level-Sensitive mode; ICSEL ICM5; AUXOUT Disabled; OCAEN enabled; OCBEN disabled; OENSYNC disabled; 
    CCP5CON2H = 0x100;
    //DT 0; 
    CCP5CON3L = 0x00;
    //OETRIG disabled; OSCNT None; POLACE disabled; POLBDF disabled; PSSBDF Tri-state; OUTM Steerable single output; PSSACE Tri-state; 
    CCP5CON3H = 0x00;
    //ICDIS disabled; SCEVT disabled; TRSET disabled; ICOV disabled; ASEVT disabled; ICGARM disabled; TRCLR disabled; 
    CCP5STATL = 0x00;
    //TMR 0; 
    CCP5TMRL = 0x00;
    //TMR 0; 
    CCP5TMRH = 0x00;
    //PR 16383; 
    CCP5PRL = 0x3FFF;
    //PR 0; 
    CCP5PRH = 0x00;
    //CMP 0; 
    CCP5RA = 0x00;
    //CMP 8191; 
    CCP5RB = 0x1FFF;
    //BUF 0; 
    CCP5BUFL = 0x00;
    //BUF 0; 
    CCP5BUFH = 0x00;

    CCP5CON1Lbits.CCPON = 0x1; //Enabling CCP

    gMCCP5Mode = CCP5CON1Lbits.MOD;

}

void __attribute__ ((weak)) MCCP5_COMPARE_CallBack(void)
{
    // Add your custom callback code here
}

void MCCP5_COMPARE_Tasks( void )
{
    if(IFS1bits.CCP5IF)
    {
		// MCCP5 COMPARE callback function 
		MCCP5_COMPARE_CallBack();
		
        IFS1bits.CCP5IF = 0;
    }
}

void __attribute__ ((weak)) MCCP5_COMPARE_TimerCallBack(void)
{
    // Add your custom callback code here
}


void MCCP5_COMPARE_TimerTasks( void )
{
    if(IFS2bits.CCT5IF)
    {
		// MCCP5 COMPARE Timer callback function 
		MCCP5_COMPARE_TimerCallBack();
	
        IFS2bits.CCT5IF = 0;
    }
}

void MCCP5_COMPARE_Start( void )
{
    /* Start the Timer */
    CCP5CON1Lbits.CCPON = true;
}

void MCCP5_COMPARE_Stop( void )
{
    /* Start the Timer */
    CCP5CON1Lbits.CCPON = false;
}

void MCCP5_COMPARE_SingleCompare16ValueSet( uint16_t value )
{   
    CCP5RA = value;
}


void MCCP5_COMPARE_DualCompareValueSet( uint16_t priVal, uint16_t secVal )
{

    CCP5RA = priVal;

    CCP5RB = secVal;
}

void MCCP5_COMPARE_DualEdgeBufferedConfig( uint16_t priVal, uint16_t secVal )
{

    CCP5RA = priVal;

    CCP5RB = secVal;
}

void MCCP5_COMPARE_CenterAlignedPWMConfig( uint16_t priVal, uint16_t secVal )
{

    CCP5RA = priVal;

    CCP5RB = secVal;
}

void MCCP5_COMPARE_EdgeAlignedPWMConfig( uint16_t priVal, uint16_t secVal )
{

    CCP5RA = priVal;

    CCP5RB = secVal;
}

void MCCP5_COMPARE_VariableFrequencyPulseConfig( uint16_t priVal )
{
    CCP5RA = priVal;
}

bool MCCP5_COMPARE_IsCompareCycleComplete( void )
{
    return(IFS1bits.CCP5IF);
}

bool MCCP5_COMPARE_TriggerStatusGet( void )
{
    return( CCP5STATLbits.TRIG );
    
}

void MCCP5_COMPARE_TriggerStatusSet( void )
{
    CCP5STATLbits.TRSET = 1;
}

void MCCP5_COMPARE_TriggerStatusClear( void )
{
    /* Clears the trigger status */
    CCP5STATLbits.TRCLR = 0;
}

bool MCCP5_COMPARE_SingleCompareStatusGet( void )
{
    return( CCP5STATLbits.SCEVT );
}

void MCCP5_COMPARE_SingleCompareStatusClear( void )
{
    /* Clears the trigger status */
    CCP5STATLbits.SCEVT = 0;
    
}
/**
 End of File
*/
