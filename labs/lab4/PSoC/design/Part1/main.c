/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"



#define POT_MAX 255
#define POT_MIN 0


int pot = 0;


CY_ISR(Sample_ISR)
{
    //samples the pot every millisecond using tc of timer to trigger ISR
    pot = ADC_GetResult32();
    if(pot > POT_MAX) pot = POT_MAX;
    if(pot < POT_MIN) pot = POT_MIN;
    //checks if their is space in FIFO to load data    
    
    UART_WriteTxData(pot);
      
}


int main(void)
{
    CyGlobalIntEnable;
    //initialization
    LCD_Start();
    ADC_Start();
    ADC_StartConvert();
    Sample_ISR_StartEx(Sample_ISR);
    Timer_Start();
    UART_Start();
    for(;;)
    {
        LCD_ClearDisplay();
        LCD_Position(0,0);
        LCD_PrintNumber(pot);
        
        
    }
}

