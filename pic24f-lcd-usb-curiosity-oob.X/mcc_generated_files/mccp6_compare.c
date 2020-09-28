/**
  MCCP6 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    mccp6.c

  @Summary
    This is the generated driver implementation file for the MCCP6 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides implementations for driver APIs for MCCP6. 
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

#include "mccp6_compare.h"

/** OC Mode.

  @Summary
    Defines the OC Mode.

  @Description
    This data type defines the OC Mode of operation.

*/

static uint16_t         gMCCP6Mode;

/**
  Section: Driver Interface
*/


void MCCP6_COMPARE_Initialize (void)
{
    // CCPON enabled; MOD Dual Edge Compare; CCSEL disabled; CCPSIDL disabled; T32 16 Bit; CCPSLP disabled; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; 
    CCP6CON1L = (0x8004 & 0x7FFF); //Disabling CCPON bit
    //RTRGEN disabled; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; OPS Each Time Base Period Match; SYNC MCCP6 Timer; OPSSRC Timer Interrupt Event; 
    CCP6CON1H = 0x00;
    //ASDGM disabled; SSDG disabled; ASDG 0; PWMRSEN disabled; 
    CCP6CON2L = 0x00;
    //ICGSM Level-Sensitive mode; ICSEL ICM6; AUXOUT Disabled; OCAEN enabled; OCBEN disabled; OENSYNC disabled; 
    CCP6CON2H = 0x100;
    //DT 0; 
    CCP6CON3L = 0x00;
    //OETRIG disabled; OSCNT None; POLACE disabled; POLBDF disabled; PSSBDF Tri-state; OUTM Steerable single output; PSSACE Tri-state; 
    CCP6CON3H = 0x00;
    //ICDIS disabled; SCEVT disabled; TRSET disabled; ICOV disabled; ASEVT disabled; ICGARM disabled; TRCLR disabled; 
    CCP6STATL = 0x00;
    //TMR 0; 
    CCP6TMRL = 0x00;
    //TMR 0; 
    CCP6TMRH = 0x00;
    //PR 16383; 
    CCP6PRL = 0x3FFF;
    //PR 0; 
    CCP6PRH = 0x00;
    //CMP 0; 
    CCP6RA = 0x00;
    //CMP 8191; 
    CCP6RB = 0x1FFF;
    //BUF 0; 
    CCP6BUFL = 0x00;
    //BUF 0; 
    CCP6BUFH = 0x00;

    CCP6CON1Lbits.CCPON = 0x1; //Enabling CCP

    gMCCP6Mode = CCP6CON1Lbits.MOD;

}

void __attribute__ ((weak)) MCCP6_COMPARE_CallBack(void)
{
    // Add your custom callback code here
}

void MCCP6_COMPARE_Tasks( void )
{
    if(IFS1bits.CCP6IF)
    {
		// MCCP6 COMPARE callback function 
		MCCP6_COMPARE_CallBack();
		
        IFS1bits.CCP6IF = 0;
    }
}

void __attribute__ ((weak)) MCCP6_COMPARE_TimerCallBack(void)
{
    // Add your custom callback code here
}


void MCCP6_COMPARE_TimerTasks( void )
{
    if(IFS3bits.CCT6IF)
    {
		// MCCP6 COMPARE Timer callback function 
		MCCP6_COMPARE_TimerCallBack();
	
        IFS3bits.CCT6IF = 0;
    }
}

void MCCP6_COMPARE_Start( void )
{
    /* Start the Timer */
    CCP6CON1Lbits.CCPON = true;
}

void MCCP6_COMPARE_Stop( void )
{
    /* Start the Timer */
    CCP6CON1Lbits.CCPON = false;
}

void MCCP6_COMPARE_SingleCompare16ValueSet( uint16_t value )
{   
    CCP6RA = value;
}


void MCCP6_COMPARE_DualCompareValueSet( uint16_t priVal, uint16_t secVal )
{

    CCP6RA = priVal;

    CCP6RB = secVal;
}

void MCCP6_COMPARE_DualEdgeBufferedConfig( uint16_t priVal, uint16_t secVal )
{

    CCP6RA = priVal;

    CCP6RB = secVal;
}

void MCCP6_COMPARE_CenterAlignedPWMConfig( uint16_t priVal, uint16_t secVal )
{

    CCP6RA = priVal;

    CCP6RB = secVal;
}

void MCCP6_COMPARE_EdgeAlignedPWMConfig( uint16_t priVal, uint16_t secVal )
{

    CCP6RA = priVal;

    CCP6RB = secVal;
}

void MCCP6_COMPARE_VariableFrequencyPulseConfig( uint16_t priVal )
{
    CCP6RA = priVal;
}

bool MCCP6_COMPARE_IsCompareCycleComplete( void )
{
    return(IFS1bits.CCP6IF);
}

bool MCCP6_COMPARE_TriggerStatusGet( void )
{
    return( CCP6STATLbits.TRIG );
    
}

void MCCP6_COMPARE_TriggerStatusSet( void )
{
    CCP6STATLbits.TRSET = 1;
}

void MCCP6_COMPARE_TriggerStatusClear( void )
{
    /* Clears the trigger status */
    CCP6STATLbits.TRCLR = 0;
}

bool MCCP6_COMPARE_SingleCompareStatusGet( void )
{
    return( CCP6STATLbits.SCEVT );
}

void MCCP6_COMPARE_SingleCompareStatusClear( void )
{
    /* Clears the trigger status */
    CCP6STATLbits.SCEVT = 0;
    
}
/**
 End of File
*/
