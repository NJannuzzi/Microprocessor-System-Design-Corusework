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
#define PINGFLAG 1
#define PONGFLAG 0

#define ARRAY_SIZE 64
/* Defines for DMA */
#define DMA_BYTES_PER_BURST 1
#define DMA_REQUEST_PER_BURST 1
#define DMA_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_DST_BASE (CYDEV_SRAM_BASE)
#define DMA2_BYTES_PER_BURST 1
#define DMA2_REQUEST_PER_BURST 1
#define DMA2_SRC_BASE (CYDEV_SRAM_BASE)
#define DMA2_DST_BASE (CYDEV_PERIPH_BASE)
char DMA_FLAG = PINGFLAG;
char DMA_FLAG2 = PONGFLAG;
uint8 PING[ARRAY_SIZE];
uint8 PONG[ARRAY_SIZE];
uint8 PING2[ARRAY_SIZE];
uint8 PONG2[ARRAY_SIZE];
int ADCVAL = 0;
int i= 0;
int k = 0;
/* Variable declarations for DMA */
/* Move these variable declarations to the top of the function */
uint8 DMA_Chan;
uint8 DMA_TD[2];

uint8 DMA2_Chan;
uint8 DMA2_TD[2];


CY_ISR(TX_ISR)
{
        //check flag value, write corresponding array accordingly
        //One for Ping, one for Pong
       
    while(UART_ReadTxStatus() & UART_TX_STS_FIFO_NOT_FULL)
    {
        if(DMA_FLAG == PINGFLAG)
        {
            //iterate through and load 4 at a time
            
                UART_PutChar(PING[i]);
                UART_PutChar(PING[i+1]);
                UART_PutChar(PING[i+2]);
                UART_PutChar(PING[i+3]);
                i+=4;
            
        }
        if(DMA_FLAG == PONGFLAG)
        {
            //iterate through and load 4 at a time
            
                UART_PutChar(PONG[i]);  
                UART_PutChar(PONG[i+1]);
                UART_PutChar(PONG[i+2]);
                UART_PutChar(PONG[i+3]);
                i+=4;
        }
        if(i == ARRAY_SIZE)i = 0;
    }          
}

CY_ISR(RX_ISR)
{
    
    //read data, put it into second buffer arrays
        if(DMA_FLAG2 == PINGFLAG)
        {
            //iterate through and read everything
          
                PING2[k] = UART_ReadRxData();   
            
        }
        if(DMA_FLAG2 == PONGFLAG)
        {
            //iterate through and read everything
            
                PONG2[k] =  UART_ReadRxData();  
            
        }
        k++;
        if(k == ARRAY_SIZE)k = 0;
    
}

CY_ISR(DMA_ISR)
{
   
        //toggle the flag to determine which DMA we want to use
        //alternates the array we use for the DMA
        if(DMA_FLAG == PONGFLAG)DMA_FLAG = PINGFLAG;
        if(DMA_FLAG == PINGFLAG)DMA_FLAG = PONGFLAG;
  
    
}


CY_ISR(DMA2_ISR)
{

        //toggle the flag to determine which DMA we want to use
        //alternates the array we use for the DMA
        if(DMA_FLAG2 == PONGFLAG)DMA_FLAG2 = PINGFLAG;
        if(DMA_FLAG2 == PINGFLAG)DMA_FLAG2 = PONGFLAG;  

    
    
}
int main(void)
{
    //set up everything
    CyGlobalIntEnable;
    TX_ISR_StartEx(TX_ISR);
    RX_ISR_StartEx(RX_ISR);
    DMA_ISR_StartEx(DMA_ISR);
    DMA2_ISR_StartEx(DMA2_ISR);
    UART_Start();
    ADC_Start();
    ADC_StartConvert();
    VDAC_Start();
    
    
    
    //DMA Setup Stuff
    DMA_Chan = DMA_DmaInitialize(DMA_BYTES_PER_BURST, DMA_REQUEST_PER_BURST, 
        HI16(DMA_SRC_BASE), HI16(DMA_DST_BASE));
    DMA_TD[0] = CyDmaTdAllocate();
    DMA_TD[1] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_TD[0], 64, DMA_TD[1], DMA__TD_TERMOUT_EN | CY_DMA_TD_TERMOUT1_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_TD[1], 64, DMA_TD[0], DMA__TD_TERMOUT_EN | CY_DMA_TD_TERMOUT1_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA_TD[0], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)PING));
    CyDmaTdSetAddress(DMA_TD[1], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)PONG));
    CyDmaChSetInitialTd(DMA_Chan, DMA_TD[0]);
    CyDmaChEnable(DMA_Chan, 1);
    //CyDmaChSetRequest(DMA_Chan, CPU_REQ);
  

    
    DMA2_Chan = DMA2_DmaInitialize(DMA2_BYTES_PER_BURST, DMA2_REQUEST_PER_BURST, 
        HI16(DMA2_SRC_BASE), HI16(DMA2_DST_BASE));
    DMA2_TD[0] = CyDmaTdAllocate();
    DMA2_TD[1] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA2_TD[0], 64, DMA2_TD[1], DMA2__TD_TERMOUT_EN | CY_DMA_TD_TERMOUT1_EN | CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetConfiguration(DMA2_TD[1], 64, DMA2_TD[0], DMA2__TD_TERMOUT_EN | CY_DMA_TD_TERMOUT1_EN | CY_DMA_TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA2_TD[0], LO16((uint32)PING2), LO16((uint32)VDAC_Data_PTR));
    CyDmaTdSetAddress(DMA2_TD[1], LO16((uint32)PONG2), LO16((uint32)VDAC_Data_PTR));
    CyDmaChSetInitialTd(DMA2_Chan, DMA2_TD[0]);
    CyDmaChEnable(DMA2_Chan, 1);
    //CyDmaChSetRequest(DMA2_Chan, CPU_REQ);
    
    //idle loop
    for(;;)
    {
        
    }
}

/* [] END OF FILE */
