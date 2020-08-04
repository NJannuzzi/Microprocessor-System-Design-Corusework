/* ========================================
 *
 *CMPE121/L Spring 2019 
 *Nicholas Jannuzzi
 * Lab 5 Part 2: Reconstructing 100Hz sine wave using USB transfer and DMA 
 * ========================================
*/
#include "project.h"
#define PINGFLAG 1
#define PONGFLAG 0

#define ARRAY_SIZE 64
#define USBFS_DEVICE (0u)
#define IN_EP_NUM (1u)
#define OUT_EP_NUM (2u)
#define BUFFER_SIZE (64u)
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
int T;
int R;
/* Variable declarations for DMA */
/* Move these variable declarations to the top of the function */
uint8 DMA_Chan;
uint8 DMA_TD[2];

uint8 DMA2_Chan;
uint8 DMA2_TD[2];
CY_ISR(DMA_ISR)
{
        //toggle the flag to determine which DMA we want to use
        //alternates the array we use for the DMA
        if(DMA_FLAG == PONGFLAG)DMA_FLAG = PINGFLAG;
        else if(DMA_FLAG == PINGFLAG)DMA_FLAG = PONGFLAG;
        //set flag
        T = 1;
    
}


CY_ISR(DMA2_ISR)
{
        //toggle the flag to determine which DMA we want to use
        //alternates the array we use for the DMA
        if(DMA_FLAG2 == PONGFLAG)DMA_FLAG2 = PINGFLAG;
        else if(DMA_FLAG2 == PINGFLAG)DMA_FLAG2 = PONGFLAG;  
        //set flag
        R = 1;
    
    
}
int main(void)
{
    uint16 length;
    //set up everything
    CyGlobalIntEnable;
    DMA_ISR_StartEx(DMA_ISR);
    DMA2_ISR_StartEx(DMA2_ISR);
    USBFS_Start(USBFS_DEVICE, USBFS_5V_OPERATION);
    
    
    ADC_Start();
    ADC_StartConvert();
    VDAC_Start();

    while(0u == USBFS_GetConfiguration()){}
    USBFS_EnableOutEP(OUT_EP_NUM);
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
    
    //mainloop
    for(;;)
    {   
        //check if anything changed content wise
        if(0u != USBFS_IsConfigurationChanged())
        {
            //re enable once we configure
            if(0u != USBFS_GetConfiguration())
            {
             //enables receiving data from host
             USBFS_EnableOutEP(OUT_EP_NUM);   
            }
        }
        //check if we actually receive data
        if(USBFS_OUT_BUFFER_FULL == USBFS_GetEPState(OUT_EP_NUM))
        {
            //read length of received data
            length = USBFS_GetEPCount(OUT_EP_NUM); 
            while(R == 0){}
            if(DMA_FLAG2 == PONGFLAG)
            {
                //use DMA to copy data from OUT buffer, flag determines which one we use
                USBFS_ReadOutEP(OUT_EP_NUM,PONG2,length);
                R = 0;
            }
            if(DMA_FLAG2 == PINGFLAG)
            {
                //make sure we reset the flag in either case
                USBFS_ReadOutEP(OUT_EP_NUM,PING2,length);
                R = 0;
            } 
            while(USBFS_OUT_BUFFER_FULL == USBFS_GetEPState(OUT_EP_NUM)){}
            //reenable out like in example
            USBFS_EnableOutEP(OUT_EP_NUM);
         }
        
        
        while(USBFS_IN_BUFFER_EMPTY != USBFS_GetEPState(IN_EP_NUM)){} 
            //do nothing
            while(T == 0){}
            if(DMA_FLAG == PONGFLAG)
            {
            //use DMA to copy data from IN buffer, flag determines which we use
            USBFS_LoadInEP(IN_EP_NUM,PONG,ARRAY_SIZE);
            T = 0;
            }
            //also need to make sure we reset flag
            if(DMA_FLAG == PINGFLAG)
            {
            USBFS_LoadInEP(IN_EP_NUM,PING,ARRAY_SIZE);
            T = 0;
            } 
    }
}

