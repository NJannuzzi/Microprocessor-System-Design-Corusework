/* ========================================
 *
 * CMPE121/L Spring 2019
 * Nicholas Jannuzzi
 * Lab5 Part 1: Controlling LED via PWM and USB transfer
 * ========================================
*/
#include "project.h"
#include <cytypes.h>
#define USBFS_DEVICE  (0u)
#define IN_EP_NUM     (1u)
#define POTMIN 0
#define POTMAX 255
#define LENGTH 1
#define T 1
#define F 0
int16 pot = 0;
uint8 pot2 = 0;
//everything goes in interrupt
CY_ISR(Timer_ISR)
{
   pot = ADC_Read16();
   //saturate data 
   if(pot > POTMAX)pot = POTMAX;
   if(pot < POTMIN)pot = POTMIN; 
   pot2 = pot;
   Timer_ReadStatusRegister(); 
}

int main(void)
{   
    //start everything up
    CyGlobalIntEnable;
    Timer_ISR_StartEx(Timer_ISR);
    USBFS_Start(0u, USBFS_5V_OPERATION);     
    ADC_Start();
    ADC_StartConvert();
    Timer_Start();
    //wait until host enumerates before proceeding
    while (!USBFS_GetConfiguration()){}
    for(;;)
    {
        //load into in endpoint for transmission
        USBFS_LoadInEP(IN_EP_NUM, &pot2, LENGTH);
          
    }  
     
}

/* [] END OF FILE */
