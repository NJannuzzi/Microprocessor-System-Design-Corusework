/* ========================================
 *
 *Nicholas Jannuzzi
 *CMPE121/L Spring 2019
 *Part 2: Transmission/Receiving using ISR's
 *A modification to part 1 to operate using ISR's this time
 *This portion of the exercise has us determine how many times the ISR was triggered instead of calculating the run time of the transfer
 *
 *
 *
 *
 * ========================================
*/
#include "project.h"
#define T 1
#define F 0
#define ARRAYLENGTH 4096
//#define FORCEERRORS 1
uint8 SArray[ARRAYLENGTH];
uint8 DArray[ARRAYLENGTH];
uint8 ISR_Transmission = F;
uint8 ISR_Receive = F;
int Tx;
int Rx;
int i = 0;
int j = 0;
int k = 0;
int Errors = 0;


//Transmission Interrupt Service Routine
//Transmission ISR is set to go off whenever the FIFO empties
//This ISR will go high every time a transmission occurs, since the FIFO empties once every transmission
CY_ISR(Tx_ISR)
{

    //set flag to transmit
    //had the FIFO loading within here, but it would bug out
    //load 4 bytes at a time to optimize
    ISR_Transmission = T;
    
    Tx++;
    
}

//Receiving Interrupt Service Routine
//This ISR activates whenever a byte is received, so once every transmission 
CY_ISR(Rx_ISR)
{      
    while(((UART_ReadRxStatus() & UART_RX_STS_FIFO_NOTEMPTY)))
    {
        DArray[k] = UART_ReadRxData();
        k++;
    }  
   
    //increment the number of interrupts counter
    Rx++; 
    if(k == ARRAYLENGTH)
    {
        ISR_Receive = T;
        Rx_ISR_Disable();
    }
}

int main(void)
{

    CyGlobalIntEnable; 
    //initialization of modules
    //populate arrays initially
    for(i = 0; i < ARRAYLENGTH; i++)
    {
        //Source array will contain modulo of 256
        SArray[i] = i%256;
        //destination array will contain zeroes
        DArray[i] = 0;
    }
    LCD_Start();
    LCD_Position(0u,0u);
    Rx_ISR_StartEx(Rx_ISR);
    Tx_ISR_StartEx(Tx_ISR); 
    UART_Start();
    //looping to check flags set in ISR and load data for transmission
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
    
    //Error Forcing for checkoff demonstration
    #ifdef FORCEERRORS
        //set random array contnets to values we know are wrong

        DArray[0] = 5;
        
    #endif
    
    
    //Error Checking by comparing array index contents
    //same method as in part 
    
    while(ISR_Receive == F);
    for(i = 0; i < ARRAYLENGTH; i++)
    {
        //check if elements, don't match, increment error counter accordingly
        if(SArray[i] != DArray[i])
        {
            Errors++;   
        }
    }
    
    //LCD Display Control
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


