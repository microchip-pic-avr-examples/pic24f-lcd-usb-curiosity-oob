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

#ifndef LED3_BLUE_H
#define LED3_BLUE_H

#include <xc.h>
#include <stdbool.h>
#include "mcc_generated_files/mccp6_compare.h"

static bool ismccp6Enabled = false;

void LED3_BLUE_On(void) 
{
    MCCP6_COMPARE_Start();
    ismccp6Enabled = true;
}

void LED3_BLUE_Off(void) 
{
    MCCP6_COMPARE_Stop();
    ismccp6Enabled = false;
}

void LED3_BLUE_Toggle(void)
{
    if (ismccp6Enabled == true) {
        LED3_BLUE_Off();
    } else {
        LED3_BLUE_On();
    }
}

void LED3_BLUE_SetIntensity(uint16_t new_intensity) 
{
    MCCP6_COMPARE_DualCompareValueSet(0x0, new_intensity);
}
#endif
