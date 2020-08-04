/* ========================================
 *
 *Nicholas Jannuzzi
 *CMPE121/L
 *Part 4:Data Transmission between two UART modules
 *System will display no errors and a 3Kb/s throughput when connected, and will accrue errors before re-syncing when reconnecting it
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#define ARRAYLENGTH 4096
#define T 1
#define F 0
uint8 SArray[ARRAYLENGTH];
uint8 DArray[ARRAYLENGTH];
uint8 count = 0;
uint8 Expected = 0;
int Errors = 0;
int Prime;
int Throughput;
int RXP;
char strp[100];
//Primary TX ISR
CY_ISR(TX_ISRP)
{
    //write to Tx buffer on primary UART
    UART_Plato_WriteTxData(count);    
    count++;
    
}
//Primary RX ISR
CY_ISR(RX_ISRP)
{
    //check for errors 
    if(Expected != UART_Plato_ReadRxData())
    {
        Errors++;
    }
    //increment expected value if they match
    else Expected++;
    //number of ISR triggers
    RXP++;
    
}

//Secondary RX ISR
CY_ISR(RX_ISRS)
{
    //echo the data into the secondary Tx FIFO from the secondary Rx
    UART_Sigmund_WriteTxData(UART_Sigmund_ReadRxData());
    
}
//generate interrupt every second
CY_ISR(ISR_TIMER)
{
    //calculates throughput in bytes/sec
    Throughput = RXP;
    RXP = 0;
    
}

int main(void)
{
    CyGlobalIntEnable;
    //initialization
    RX_ISRS_StartEx(RX_ISRS);
    TX_ISRP_StartEx(TX_ISRP);
    RX_ISRP_StartEx(RX_ISRP);
    ISR_TIMER_StartEx(ISR_TIMER);
    UART_Plato_Start();
    UART_Sigmund_Start();
    Timer_Start();
    LCD_Start();
    LCD_Position(0u,0u);
    for(;;)
    {
        //the only thing in here is the LCD display component
        LCD_ClearDisplay();  
        LCD_Position(0,0);
        LCD_PrintString("E:");
        LCD_Position(0,3);
        //saturation for errors
        if(Errors > 1000) Errors = 1000;         
        LCD_PrintNumber(Errors);
        LCD_Position(1,0);
        LCD_PrintString("TP:");
        LCD_Position(1,6);  
        LCD_PrintNumber(Throughput);
        LCD_PrintString("b/s");
        CyDelay(1);
        
    }
}


