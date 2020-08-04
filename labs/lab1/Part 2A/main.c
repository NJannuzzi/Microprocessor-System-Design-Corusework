/* ========================================
 *
 * 
 * Nicholas Jannuzzi
 * CMPE121 Spring 2019
 *
 *
 * Part 2A: Ultrasonic Ping Sensor to detet objects within 100cm and light an LED for 5 seconds if triggered
 *
 * ========================================
*/
#include "stdio.h"
#include "project.h"
int time = 0;
int flighttime = 0;
int distance = 0;
int EchoT1 = 0;
int EchoT2 = 0;
int EchoTD = 0;
char DS[100];
char TS[100];
//speed of sound in m/s
#define SoundSpeed 340
//meter to cm conversion
#define M2CM 100
//#define for 1 and 2 
#define DIV 2
#define ONE 1
//interrupt to measure echo pulse width
//use falling edge by inverting a rising edge ISR connected to inverter
CY_ISR(EchoTISR)
{
//measure the time at the falling edge
EchoT1 = TimerEcho_ReadCounter();
//calculate pulse width from the echo time
flighttime = 65535 - EchoT1;
//pulse width(flighttime) to distance conversion
distance = (flighttime * SoundSpeed/DIV) * M2CM;
//reset the counter value when we leave so we are ready for next detection
TimerEcho_WriteCounter(ONE);


}
//handles delay between cycles
CY_ISR(Echo_ISR)
{
//500ms waiting period between cycles
CyDelay(500);    
Echo_Pin_ClearInterrupt();    
}

//interrupt to handle LED control
//this interrupt will remain active while detecting an object, and for 5 seconds after last detection
CY_ISR(LED_ISR)
{
    //drive pin high as soon as we enter interrupt
    CyPins_SetPin(LED_Pin_0);
}
int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    //initialize components
    LCD_Char_Start();
    Counter8_Start();
    Echo_ISR_StartEx(Echo_ISR);
    LED_ISR_StartEx(LED_ISR);
    EchoTISR_StartEx(EchoTISR);
   //initial write of registers for the trigger and counter 
   TrigRegister_Write(1);
   CounterRegister_Write(1); 
    //main code loop
    for(;;)
    {
    
    
        
        
        
        
        
        
       
        
        
        
   //lighting up the LED   
    //Write results to strings to print in decimal value to LCD    
    sprintf(DS,"Dist: %dCM",distance);
    sprintf(TS,"Time: %dMS",time);
    //Print distance and LED time to LCD Display 
    LCD_Char_ClearDisplay();
    LCD_Char_Position(0,0);
    LCD_Char_PrintString(DS);
//    LCD_Char_PrintInt16(distance);
//    LCD_Char_PrintString("CM");
    LCD_Char_Position(1,0);
    LCD_Char_PrintString(TS);
//    LCD_Char_PrintInt16(test);
//    LCD_Char_PrintString("MS");
    

}
}
/* [] END OF FILE */
