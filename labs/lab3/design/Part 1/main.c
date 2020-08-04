/* ========================================
 *
 * 
 * Nicholas Jannuzzi
 * CMPE121/l Spring 121
 * Part 1: Transmission/Receive via Polling
 * Continuously transmits a byte of data and also performs a transfer of array values along with error checking/forcing
 * 
 *
 * ========================================
*/
#include "project.h"
#define TRANSMIT 0xa5
#define T 1
#define F 0
#define ARRAYLENGTH 4096
#define TMAX 50000000
//#define FORCEERRORS 1
//#define CONTINUOUS 1
#define ARRAYTRANSMIT 1
#define MILLI 1000
uint32 runtime;
//source and destination arrays 

int i;
int j;
int Error;
int main(void)
{
    //define array in sram
    uint8 SArray[ARRAYLENGTH];
    uint8 DArray[ARRAYLENGTH];
    CyGlobalIntEnable; 
    //initializing components
    UART_Start();
    LCD_Start();
    LCD_Position(0,0);
    
    //continuous transmission of byte 0xa5
    #ifdef CONTINUOUS
        for(;;)
        {
            UART_WriteTxData(0xa5);  
            CyDelay(10);
        }
    #endif /* CONTINUOUS*/
    
    //singluar transmission from destination to source
    #ifdef ARRAYTRANSMIT
        //populate arrays initially
        for(i = 0; i < ARRAYLENGTH; i++)
        {
            //Source array will contain modulo of 256
            SArray[i] = i%256;
            //destination array will contain zeroes
            DArray[i] = 0;
        }
        
        //Start Timer at beginning of UART process
        Timer_Start();
        
        //transmission loop
        for(i = 0; i < ARRAYLENGTH; i++)
        {
            
            //only loop while not full in order to prevent overflowing
            if(UART_TX_STS_FIFO_NOT_FULL)
            {
                //put byte of data from source into transmission buffer
                UART_PutChar(SArray[i]);
                   
            } 
            //stall while FIFO still has data available
            while(~UART_ReadTxStatus() & UART_TX_STS_COMPLETE)
            {
                //need this to stall time
                
            }
            //get byte of data from the UART and put it into destination array
            DArray[i] = UART_GetChar();
        }
        //use this to force errors for checkoff demonstration
        #ifdef FORCEERRORS
        {
            DArray[400] = 0;
            DArray[1342] = 16;
            DArray[0] = 23;
        }
        #endif
        //done, stop timer
        Timer_Stop();
        //calculate the run time for the entire operation
        runtime = (TMAX - Timer_ReadCounter())/MILLI;
        
        //check if transmission was successfully done
        for(i = 0; i < ARRAYLENGTH; i++)
        {
           if(SArray[i] != DArray[i])
           {
                Error+=1;
            
           }              
        }  
        //Printing to the LCD
        LCD_ClearDisplay();
        LCD_PrintString("Errors:");
        LCD_Position(0,9);
        LCD_PrintNumber(Error);
        LCD_Position(1,0);
        LCD_PrintNumber(runtime);
        LCD_Position(1,5);
        LCD_PrintString("mS");
    
    #endif
}
    
/* [] END OF FILE */
