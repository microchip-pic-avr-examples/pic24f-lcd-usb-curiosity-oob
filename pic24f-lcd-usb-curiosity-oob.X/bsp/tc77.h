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

#ifndef TC77_H
#define TC77_H
/**
  @Summary
    This method returns the Temperature value of the Temperature Sensor TC77, interms of Celsius unit

  @Description
    This is the generated for TC77 Temperature Sensor driver.This method can be used 
	get the recorded temperature in Celsius format from TC77 Sensor.

  @Preconditions
   TC77_TEMPSENSOR_Initialize(void), should be called before using this.

  @Returns
    The function when called returns the Temperature value Sensed at that point of time in terms
	of Celsius format. The value returned will be a decimal number.

  @Param
    None.
*/

double TC77_GetTemperatureCelsius(void);
/**
  @Summary
    This methods turns off the TC77 temperature sensor 

  @Description
    This method can be used to turn off the TC77 Temperature sensor

  @Preconditions
    TC77 Temperature sensor should be configured to get the Temperature value  

  @Returns
    None
  @Param
    None.
 */
void TC77_Shutdown(void);

#endif
