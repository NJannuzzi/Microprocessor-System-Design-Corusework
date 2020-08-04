/* ========================================
 *
 *Nicholas Jannuzzi
 *CMPE121/L
 *Part 3: UART transfer with and without hardware control flow
 * ========================================
*/
#include "project.h"
#define ARRAYLENGTH 4096
#define T 1
#define F 0
//#define FORCE 1
int i = 0;
int k = 0;
int Tx;
int Rx;
int Errors = 0;
uint8 ISR_Transmission = F;
uint8 ISR_Receive = F;
uint8 SArray[ARRAYLENGTH];
uint8 DArray[ARRAYLENGTH];
uint8 SISR = F;
uint8 DISR = F;
uint32 Time = 0;

//ISR's
//Transmission Interrupt Service Routine
//Transmission ISR is set to go off whenever the FIFO empties
//This ISR will go high every time a transmission occurs, since the FIFO empties once every transmission
CY_ISR(Tx_ISR)
{
    ISR_Transmission = T;
    Tx++;
}

//Timer ISR
//does the same thing as the rx isr in part 2, but now attaches to the timer's TC output
CY_ISR(Timer_ISR)
{
        Timer_ReadStatusRegister();
        if((UART_ReadRxStatus() & UART_RX_STS_FIFO_NOTEMPTY))
        {
            DArray[k] = UART_ReadRxData();
            k++;
            
        }
       
        Rx++;
        //disable ISR once done
        if(k == ARRAYLENGTH)
        {
            Timer_ISR_Disable();   
        }
        
    
    
}
int main(void)
{
    //Initialization of modules and ISR's
    CyGlobalIntEnable; 
    //populate arrays with initial values
    for(i = 0; i < ARRAYLENGTH; i++)
    {
        //Source array will contain modulo of 256
        SArray[i] = i%256;
        //destination array will contain zeroes
        DArray[i] = 0;
    }
    //start everything
    UART_Start();
    LCD_Start();
    LCD_Position(0u,0u);
    Tx_ISR_StartEx(Tx_ISR);
    Timer_ISR_StartEx(Timer_ISR); 
    Timer_Start();   
    
    //sets TX buffer to data for transmission 4 bytes at a time   
    for(i =0; i < ARRAYLENGTH;i+=4)
    {
        while(ISR_Transmission == F);
        if(ISR_Transmission == T)
        {
            ISR_Transmission = F;
            UART_WriteTxData(SArray[i]);
            UART_WriteTxData(SArray[i+1]);
            UART_WriteTxData(SArray[i+2]);
            UART_WriteTxData(SArray[i+3]);
        }        
    }
    //error forcing for checkoff
    #ifdef FORCE
        DArray[0] = 23;
        DArray[100] = 1;
    #endif
    //delay after finished
    CyDelay(50);
    //error checking
    for(i = 0; i <ARRAYLENGTH; i++)
    {
        //check if elements, don't match, increment error counter accordingly
        if(SArray[i] != DArray[i])
        {
         Errors++;   
        }
    }
    //LCD dipslay stuff
    LCD_ClearDisplay();
    LCD_PrintString("Tx:");
    LCD_Position(0,3);
    LCD_PrintNumber(Tx);
    LCD_Position(0,8);
    LCD_PrintString("Rx:");
    LCD_Position(0,11);
    LCD_PrintNumber(Rx);
    LCD_Position(1,0);
    LCD_PrintString("Errors:");
    LCD_Position(1,8);
    LCD_PrintNumber(Errors);
    
}


