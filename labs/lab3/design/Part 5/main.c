/* ========================================
 *
 *Nicholas Jannuzzi
 *CMPE121/L 
 *Part 5: Clock Tolerance experimentation
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
CY_ISR(RX_ISRS)
{
    //check for errors 
    if(Expected != UART_Sigmund_ReadRxData())
    {
        Errors++;
    }
    //increment expected value if they match
    else Expected++;
    //number of ISR triggers
    RXP++;
    
}

CY_ISR(ISR_TIMER)
{
    //calculates throughput in bytes/sec
    Throughput = RXP/1000;
    RXP = 0;
    
}

int main(void)
{
    CyGlobalIntEnable;
    //initialization
    RX_ISRS_StartEx(RX_ISRS);
    TX_ISRP_StartEx(TX_ISRP);
    //RX_ISRP_StartEx(RX_ISRP);
    ISR_TIMER_StartEx(ISR_TIMER);
    UART_Sigmund_Start();
    UART_Plato_Start();
    //start components
    Timer_Start();
    LCD_Start();
    LCD_Position(0u,0u);
    for(;;)
    {      

        //error checking
        LCD_ClearDisplay();      
        LCD_Position(0,0);
        LCD_PrintString("E:");
        LCD_Position(0,3);
        //saturation for errors
        if(Errors > 1000) Errors = 1000;         
        LCD_PrintNumber(Errors);
        LCD_Position(1,0);
        LCD_PrintString("TP:");
        LCD_Position(1,3);  
        LCD_PrintNumber(Throughput);
        LCD_PrintString("Kb/s");
        CyDelay(1);
        
    }
}
