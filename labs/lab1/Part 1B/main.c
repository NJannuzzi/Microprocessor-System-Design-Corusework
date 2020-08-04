/* ========================================
 *
 * Nicholas Jannuzzi
 * CMPE121 Spring 2016
 * 
 *
 * Lab Exercise 1, part 1B: PWM control of LED
 * 
 *
 * ========================================
*/
#include "project.h"
#define Pot_Min 1
#define Pot_Max 65535
int32 reading;

int main(void)
{
    //initialize modules
    PWM_1_Start();    
    //start ADC and start conversions
    ADC_DelSig_1_Start();
    ADC_DelSig_1_StartConvert();
    
    //main program loop
    for(;;)
    {
    
    //get reading via read function, using a 32 bit integer so we don't lose half our values 
    reading = ADC_DelSig_1_GetResult32();
        
    //saturation for values above and below the min and max values
    if(reading <= Pot_Min)
    {
    reading = Pot_Min;    
    }
    if(reading >= Pot_Max)
    {
    reading = Pot_Max;    
    }
    //reading = reading * PWM_MAX/Pot_Max;
    //write using pwm module
    
    PWM_1_WriteCompare(reading);
        
    }
    
}

/* [] END OF FILE */
