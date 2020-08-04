/* ========================================
 *
 * Nicholas Jannuzzi
 * CMPE121 Spring 2019
 * Lab Exercise 2 Part 2
 *
 * 
 *
 * ========================================
*/
#include <cytypes.h>
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#define T 1
#define F 0
#define ROW 4
#define BLOCKSIZE 16384
#define ARRAYHEIGHT 4095
#define DMA_1_BYTES_PER_BURST 124
#define DMA_1_REQUEST_PER_BURST 0
#define DMA_1_SRC_BASE (CYDEV_SRAM_BASE)
#define DMA_1_DST_BASE (CYDEV_SRAM_BASE)
#define MAX32 4294967296
#define NANO 2400  

uint8 DMA_1_Chan;
uint8 DMA_1_TD[5];
uint8 Transfer_Complete;
uint16 TimeCount;
uint16 TimeCount2;
int Errors = 0;
int Errors2;
int IFLAG = 0;
int state;
int test;
CY_ISR (MAIN_ISR)
{
    
    //set a flag to say were done 
    IFLAG = 1;
    //stop timer
    TransferTimer_Stop();
    TimeCount = (TransferTimer_ReadPeriod() - TransferTimer_ReadCounter())/NANO;
    //clear the register
    state = TransferTimer_ReadStatusRegister();
}

int main(void)
{
    uint8 ArraySource[BLOCKSIZE];
    uint8 ReceiveSource[BLOCKSIZE] = {0};
    for(int i = 0; i < BLOCKSIZE; i++)
   {
       ArraySource[i] = i % 256;  
       
    }
    MAIN_ISR_StartEx(MAIN_ISR);
    LCD_Start();

    /* DMA Configuration for DMA_1 */
    DMA_1_Chan = DMA_1_DmaInitialize(DMA_1_BYTES_PER_BURST, DMA_1_REQUEST_PER_BURST, 
        HI16(DMA_1_SRC_BASE), HI16(DMA_1_DST_BASE));
    DMA_1_TD[0] = CyDmaTdAllocate();
    DMA_1_TD[1] = CyDmaTdAllocate();
    DMA_1_TD[2] = CyDmaTdAllocate();
    DMA_1_TD[3] = CyDmaTdAllocate();
    DMA_1_TD[4] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_1_TD[0], 4092, DMA_1_TD[1], CY_DMA_TD_SWAP_EN | CY_DMA_TD_SWAP_SIZE4 | CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT);
    CyDmaTdSetConfiguration(DMA_1_TD[1], 4092, DMA_1_TD[2], CY_DMA_TD_SWAP_EN | CY_DMA_TD_SWAP_SIZE4 | CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT);
    CyDmaTdSetConfiguration(DMA_1_TD[2], 4092, DMA_1_TD[3], CY_DMA_TD_SWAP_EN | CY_DMA_TD_SWAP_SIZE4 | CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT);
    CyDmaTdSetConfiguration(DMA_1_TD[3], 4092, DMA_1_TD[4], CY_DMA_TD_SWAP_EN | CY_DMA_TD_SWAP_SIZE4 | CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT);
    CyDmaTdSetConfiguration(DMA_1_TD[4], 16, CY_DMA_DISABLE_TD, CY_DMA_TD_SWAP_EN | CY_DMA_TD_SWAP_SIZE4 | CY_DMA_TD_INC_SRC_ADR | CY_DMA_TD_INC_DST_ADR | CY_DMA_TD_AUTO_EXEC_NEXT | DMA_1__TD_TERMOUT_EN);
    CyDmaTdSetAddress(DMA_1_TD[0], LO16((uint32)&ArraySource[0]), LO16((uint32)&ReceiveSource[0]));
    CyDmaTdSetAddress(DMA_1_TD[1], LO16((uint32)&ArraySource[4092]), LO16((uint32)&ReceiveSource[4092]));
    CyDmaTdSetAddress(DMA_1_TD[2], LO16((uint32)&ArraySource[4092*2]), LO16((uint32)&ReceiveSource[4092*2]));
    CyDmaTdSetAddress(DMA_1_TD[3], LO16((uint32)&ArraySource[4092*3]), LO16((uint32)&ReceiveSource[4092*3]));
    CyDmaTdSetAddress(DMA_1_TD[4], LO16((uint32)&ArraySource[4092*4]), LO16((uint32)&ReceiveSource[4092*4]));
    CyDmaChSetInitialTd(DMA_1_Chan, DMA_1_TD[0]);
    TransferTimer_Start();
    CyDmaChEnable(DMA_1_Chan, 1);
    CyDmaChSetRequest(DMA_1_Chan,CPU_REQ);
   
    
    
    CyGlobalIntEnable; 
   
    //populate the source array with modulus of 256 and receive array with 0 in one loop upon startup 
    
    
   
    //loop to determine how many errors have occured when using DMA method
    
    if(IFLAG == 1)
    {
        //int R = 1;
        int FR = 0;
        //read the counter for time value
        //TimeCount = (MAX32 - TransferTimer_ReadCounter());
        
        //loop to iterate through and check if values match between source and destination
        for (int R = 0; R < (BLOCKSIZE/4 - 1); R++)
        {
            for(FR = 0; FR < 4; FR++)
            {
                if(ArraySource[(4 * R)+ FR] != ReceiveSource[(4*R)+ (3 - FR)])
                {
                    Errors++;
                    test = R;
                }
                
            }
          
                    
               
        }
           
            
    }

    
    
    //Software loop Method Goes Here
    //clear destination array
    
    for(int i = 0; i < BLOCKSIZE; i++)
    {
        ReceiveSource[i] = 0;
    }
    TransferTimer_Start();
    //Perfrom the same DMA transfer using a loop this time to populate the destination array
    
    for(int o = 0; o < BLOCKSIZE;o+=4)
    {
        ReceiveSource[o] = ArraySource[o+3];
        ReceiveSource[o+1] = ArraySource[o+2];
        ReceiveSource[o+2] = ArraySource[o+1];
        ReceiveSource[o+3] = ArraySource[o];
    }
    TimeCount2 = (TransferTimer_ReadPeriod() - TransferTimer_ReadCounter())/NANO;
    //perform the same error checking as we did in the DMA method
    for (int R = 0; R < (BLOCKSIZE/4 - 1); R++)
        {
            for(int FR = 0; FR < 4; FR++)
            {
                if(ArraySource[(4 * R)+ FR] != ReceiveSource[(4*R)+ (3 - FR)])
                {
                    Errors2++;
                    test = R;
                }
                
            }
          
                    
               
        }
        
        //LCD Print Statements
        
        //Prints DMA error
        LCD_Position(0,0);
        LCD_ClearDisplay();
        LCD_PrintString("E1:");
        LCD_Position(0,4);
        LCD_PrintNumber(Errors);
        //Prints Software Error
        LCD_Position(0,6);
        LCD_PrintString("E2:");
        LCD_Position(0,10);
        LCD_PrintNumber(Errors2);
        
        //Prints DMA time
        LCD_Position(1,0);
        LCD_PrintString("TD:");
        LCD_Position(1,3);
        LCD_PrintDecUint16(TimeCount);

        
        
        //Prints SOftware Time
        LCD_Position(1,8);
        LCD_PrintString("TS:");
        LCD_Position(1,11);
        LCD_PrintDecUint16(TimeCount2);

        
        CyDelay(1);
}

