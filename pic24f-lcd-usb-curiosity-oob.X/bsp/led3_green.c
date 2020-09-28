/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

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

#ifndef LED3_GREEN_H
#define LED3_GREEN_H

#include <xc.h>
#include <stdbool.h>
#include "mcc_generated_files/mccp5_compare.h"

static bool ismccp5Enabled = false;

void LED3_GREEN_On(void)
{
	MCCP5_COMPARE_Start();
    ismccp5Enabled = true;
}

void LED3_GREEN_Off(void)
{
	MCCP5_COMPARE_Stop();
    ismccp5Enabled = false;
}

void LED3_GREEN_Toggle(void)
{
	if(ismccp5Enabled == true)
    {
        LED3_GREEN_Off();
    }
    else
    {
        LED3_GREEN_On();
    }
}

void LED3_GREEN_SetIntensity(uint16_t new_intensity)
{    
    MCCP5_COMPARE_DualCompareValueSet(0x0,new_intensity);
}

#endif
