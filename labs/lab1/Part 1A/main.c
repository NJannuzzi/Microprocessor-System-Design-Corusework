/* ========================================
 *
 * Nicholas Jannuzzi
 * CMPE121 Spring 2019
 * 
 *
 * 
 * Part 1A, toggling GPIO pin(s) connected to LED via 3 different methods
 * One Pin is connected to all three LED wires, light will flash white
 * ========================================
*/
#include "project.h"

#define LED_ON 1
#define LED_OFF 0
//change this variable to select which method to use
//1 = Per-Pin
//2 = Component API
//3 = Control Register
int Method = 1;
int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    //counter for the control register
    uint8 countcontrol = 0;
   
    
    for(;;)
    {
      //Per-Pin method  
        while(Method == 1)
        {
        //set pins          
//        CyPins_SetPin(Pin_Red_0);
//     
//        CyPins_SetPin(Pin_Green_0);
//        
//        CyPins_SetPin(Pin_Blue_0);
//        //manages delay between subsequnet pin commands
//        //use a delay of 1000ms = 1second
//        CyDelay(500);
//        //clear pins afterwards
//        CyPins_ClearPin(Pin_Red_0);
//       
//        CyPins_ClearPin(Pin_Green_0);
//        
//        CyPins_ClearPin(Pin_Blue_0);
//        //another delay of 1 second
//        CyDelay(500);
          //50ms is about as fast as we can see it turning on and off  
  
          CyPins_SetPin(Pin_Red_0);
          //CyDelay(50);  
          CyPins_ClearPin(Pin_Red_0);  
          //CyDelay(50);  
        
        }
        
        
        //Toggle pin as fast as possible
        
        
      //Component API Method
        while(Method == 2)
        {
          //toggle all LED every second
//      Pin_Red_Write(LED_ON);
//        Pin_Green_Write(LED_ON);
//        Pin_Blue_Write(LED_ON);    
//        CyDelay(1000);
//        Pin_Red_Write(LED_OFF);
//        Pin_Green_Write(LED_OFF);
//        Pin_Blue_Write(LED_OFF);    
//        CyDelay(1000);
        //Write to blue pin as fast as possible
        Pin_Green_Write(LED_ON);
        //CyDelay(50);   
        Pin_Green_Write(LED_OFF);
        //CyDelay(50);
        }
        
        
     //Control Register Method
        while(Method == 3)
        {
        //toogle all three LED at once
        //make sure to connect all pins to register
//        Control_Reg_Write(countcontrol%2);
//        CyDelay(1000);
//        countcontrol++;
        //flash blue LED fast
        Control_Reg_Write(countcontrol%2);
        CyDelay(0);
        countcontrol++;
        }
    }
}

/* [] END OF FILE */
