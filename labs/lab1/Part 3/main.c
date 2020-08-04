/* ========================================
 *
 * Nicholas Jannuzzi
 * CMPE121 Spring 2019
 *
 * Part 3: Frequency Generator
 * Uses potentiometer to control ADC and PWM modules to generate a range of frequencies from 1KHz to 1MHz
 *
 * ========================================
*/
#include "project.h"
#include "stdio.h"
#define T 1
#define F 0
#define POT_MIN 1
#define ZERO 0
#define POT_MAX 65535
#define PMIN 1
#define PMAX 100
#define TWO 2
#define MEGAHERTZ 12000
#define DIV500 5000
#define ONE 1
#define FMAX 1000
#define FMIN 1
#define BUSDIV 6
uint16 runningcount = 0;
int Pot = 0;
uint32 Freq = 0;
uint32 PWMres = 0;
uint32 Count_for_ISR = 0;
uint32 FE = 0;
uint32 FA = 0;
uint32 newval = 0;
uint32 Duty = 0;
uint32 Period = 0;
char PString[100];
char FString[100];


//comment one ISR out before uncommenting the other
//ISR method one; clearing the counter
//CY_ISR(Main_ISR)
//{
//    //get count and store
//    runningcount = Counter_1_ReadCounter();
//    //round up by adding one and divide by two
//    FA = (runningcount + ONE) /TWO;
//    if(FA > FMAX)
//        {
//                FA = FMAX;
//        }
//    if(FA < FMIN)
//        {
//                FA = FMIN;       
//        }
//    //clear counter by writing a 0
//    Counter_1_WriteCounter(0);
//    //get status of the timer register
//    Timer_1_ReadStatusRegister();
//
//}

//ISR method two: tracking the ISR and subtracting
CY_ISR(Main_ISR)
{
    //get count and copy it for usage in this ISR
    Count_for_ISR = runningcount;  
    runningcount = Counter_1_ReadCounter();
    //set new value as difference between new and old count
    newval = runningcount - Count_for_ISR;
    //round up by adding one and divide by two
    FA = (ONE + newval)/TWO;
      if(FA > FMAX)
        {
                FA = FMAX;
        }  
      if(FA < FMIN)
        {
                FA = FMIN;        
        }
    Timer_1_ReadStatusRegister();
    
}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    //start LCD, PWM, ADC, ISR, and Timer
    LCD_Char_Start();
    PWM_1_Start();
    ADC_DelSig_1_Start();
    ADC_DelSig_1_StartConvert();
    Main_ISR_StartEx(Main_ISR);
    Timer_1_Start();
    Counter_1_Start();
       
   
    for(;;)
    {
        LCD_Char_ClearDisplay();
        //sample pot value
        Pot = ADC_DelSig_1_GetResult32();
        //over/under flow saturation handling, identical to part 1B
        if(Pot < POT_MIN)
        {
            Period = POT_MIN; 
            Pot = POT_MIN;
            
        }
        else if(Pot > POT_MAX)
        {
            Period = POT_MAX; 
            Pot = POT_MAX;
        }
        else
        {
        Period = Pot/BUSDIV;
        }
        //pwm adjusting using pot value
        PWM_1_WritePeriod(Period);
        PWM_1_WriteCompare(Period/TWO);
        //calculate frequency
        //Freq = MegaHertz/Pot;
        //set LCD to print raw potvalue and expected frequency
        
        if(FE > FMAX)
        {
                FE = FMAX;
        }
        else if(FE < FMIN)
        {
                FE = FMIN;
        }
        else
        {
            FE = ONE + (MEGAHERTZ/Period);
        }
        
        LCD_Char_Position(0,0);
        sprintf(PString,"P%lu", Pot);
        LCD_Char_PrintString(PString);   
        //Print Frequency that is actually given
        LCD_Char_Position(1,0);
        //Expected and Actual
        sprintf(FString,"A%luK  E%luK",FA,FE);
        LCD_Char_PrintString(FString);
        CyDelay(150);
    }
}
/* [] END OF FILE */
