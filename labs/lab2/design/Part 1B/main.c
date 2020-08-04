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
#include <cytypes.h>
#include "project.h"
#include <stdio.h>
#define POTMIN 5338
#define POTMAX 60445
#define POTRANGE 55112
#define TABLE_LENGTH 128
#define TABLE_EXPANDED_LENGTH 128*2
#define DMA_BYTES_PER_BURST 1
#define DMA_REQUEST_PER_BURST 1
#define CIRCLEDEG 360
#define CLOCKCONVERT 187500
#define THRESH 50
#define THRESH2 50
#define TRUE 1
#define FALSE 0
#define ZEROPOINT 128

/* Variable declarations for DMA . 
* These variables are defined as global variables to avoid "may be used before being set" warning 
* issued by the PSoC 5 compilers  MDK/RVDS.In this case these variables are automatically initialized to zero */
uint8 DMA_Chan;
uint8 DMA2_Chan;

//DMA Transaction Descriptors
uint8 DMA_TD[1];
uint8 DMA2_TD[1];
//varibales
uint32 Offset;
uint32 OldOffset;
uint32 Deg;
uint32 Shift;
int SCOUNT;
char str[100];
char str2[100];
char Pot_Has_Changed = FALSE;
int state;
////ISR to check if a value has changed by a quantity exceeding the threshold
CY_ISR (TISR)
{
    Shift+=1;
    //check if the threshold value has been exceeded
//    if((Offset - OldOffset > THRESH) || ( OldOffset - Offset > THRESH2))
//    {
//        Pot_Has_Changed = TRUE;
//        
//    }
//    else
//    {
//        Pot_Has_Changed = FALSE;   
//    }
    //set old value to current value so we can compare them later
    CyDmaChDisable(DMA2_Chan);
   
}


// This table stores the 128 points in Flash for smoother sine wave generation */
CYCODE const uint8 sineTable [TABLE_EXPANDED_LENGTH] = 
{
	128, 134, 140, 147, 153, 159, 165, 171,
	177, 183, 188, 194, 199, 204, 209, 214,
	218, 223, 227, 231, 234, 238, 241, 244,
	246, 248, 250, 252, 253, 254, 255, 255,
	255, 255, 255, 254, 253, 252, 250, 248,
	246, 244, 241, 238, 234, 231, 227, 223,
	218, 214, 209, 204, 199, 194, 188, 183, 
	177, 171, 165, 159, 153, 147, 140, 134, 
	128, 122, 115, 109, 103,  97,  91,  85,
	 79,  73,  68,  62,  57,  52,  47,  42,
	 37,  33,  29,  25,  22,  18,  15,  12,
	 10,   7,   6,   4,   2,   1,   1,   0,
	  0,   0,   1,   1,   2,   4,   6,   7,
	 10,  12,  15,  18,  22,  25,  29,  33,
	 37,  42,  47,  52,  57,  62,  68,  73,
	 79,  85,  91,  97, 103, 109, 115, 122,//1
    128, 134, 140, 147, 153, 159, 165, 171,
	177, 183, 188, 194, 199, 204, 209, 214,
	218, 223, 227, 231, 234, 238, 241, 244,
	246, 248, 250, 252, 253, 254, 255, 255,
	255, 255, 255, 254, 253, 252, 250, 248,
	246, 244, 241, 238, 234, 231, 227, 223,
	218, 214, 209, 204, 199, 194, 188, 183, 
	177, 171, 165, 159, 153, 147, 140, 134, 
	128, 122, 115, 109, 103,  97,  91,  85,
	 79,  73,  68,  62,  57,  52,  47,  42,
	 37,  33,  29,  25,  22,  18,  15,  12,
	 10,   7,   6,   4,   2,   1,   1,   0,
	  0,   0,   1,   1,   2,   4,   6,   7,
	 10,  12,  15,  18,  22,  25,  29,  33,
	 37,  42,  47,  52,  57,  62,  68,  73,
	 79,  85,  91,  97, 103, 109, 115, 122,//2

    
};	 
CYCODE const uint8 Flat[TABLE_EXPANDED_LENGTH] = 
{
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128
};
int main(void)
{
    CyGlobalIntEnable;
    VDAC8_Start();
    VDAC8_Offset_Start();
    Pot_ADC_Start();
    Pot_ADC_StartConvert();
    LCD_Start();
    Timer_1_Start();
//    Timer_1_Start();
//    TISR_StartEx(TISR);
    
    //this changes t frequency of the signals by altering the period of the timer connected to them
    //timer_clock_SetDividerValue(CLOCKCONVERT/5000);
    Timer_1_WritePeriod(16);
    /* Defines for DMA configuration */
    #if (defined(__C51__))  /* Source base address when PSoC3 is used */    
	    #define DMA_SRC_BASE (CYDEV_FLS_BASE)   
    #else  					/* Source base address when PSoC5 is used */
	    #define DMA_SRC_BASE (&sineTable[0])
	#endif    
    
	#define DMA_DST_BASE (CYDEV_PERIPH_BASE)  /* Destination base address */

    

    DMA_Chan = DMA_DmaInitialize(DMA_BYTES_PER_BURST, DMA_REQUEST_PER_BURST, HI16(DMA_SRC_BASE), HI16(DMA_DST_BASE) );
	DMA2_Chan = DMA2_DmaInitialize(DMA_BYTES_PER_BURST, DMA_REQUEST_PER_BURST, HI16(DMA_SRC_BASE), HI16(DMA_DST_BASE) );
	
    DMA_TD[0] = CyDmaTdAllocate();  								 
    DMA2_TD[0] = CyDmaTdAllocate();
    
	CyDmaTdSetConfiguration(DMA_TD[0], TABLE_LENGTH, DMA_TD[0], TD_INC_SRC_ADR); 	 
    CyDmaTdSetConfiguration(DMA2_TD[0], TABLE_LENGTH, DMA2_TD[0], TD_INC_SRC_ADR | DMA2__TD_TERMOUT_EN); 
	
    
    CyDmaTdSetAddress(DMA_TD[0], LO16((uint32)sineTable), LO16((uint32)VDAC8_Data_PTR) );
    CyDmaTdSetAddress(DMA2_TD[0], LO16((uint32)sineTable), LO16((uint32)VDAC8_Offset_Data_PTR));
    
   
    CyDmaChSetInitialTd(DMA_Chan, DMA_TD[0]); 	
	CyDmaChSetInitialTd(DMA2_Chan, DMA2_TD[0]);
    
    
    CyDmaChEnable(DMA_Chan, 1); 
    CyDmaChEnable(DMA2_Chan, 1); 
    


   
    for(;;)
    {
        
        //read data, handle under/overflow, same as before
        OldOffset = Offset;
        Offset = Pot_ADC_Read32();
       // CyDelay(5);
        
        int d = Offset - OldOffset;
         if(Offset < POTMIN)
        {
            Offset = 0;
        }
        else if(Offset > POTMAX)
        {
            Offset = 65535;
        }
         //map the pot value to a number spanning the table length
        //Shift = (Offset - POTMIN)*(TABLE_LENGTH)/(POTRANGE + 1);       
        //this shift is used to offset the table and shift the second wave
        if((Offset - OldOffset > THRESH) || ( OldOffset - Offset > THRESH2))
        {
            Pot_Has_Changed = TRUE;
            
        }
        else
        {
            Pot_Has_Changed = FALSE;   
        }
        
       
        //if this variable is true, we entered the ISR and detected a large enough shift in values
        if(Pot_Has_Changed == TRUE)
        {
            //reset the shift value
            //Shift = (Offset - POTMIN)*(TABLE_LENGTH)/(POTRANGE + 1);
            CyDmaChEnable(DMA2_Chan,FALSE);
            CyDmaTdSetAddress(DMA2_TD[0], LO16((uint32)Flat), LO16((uint32)VDAC8_Offset_Data_PTR));
//            Pot_Has_Changed = FALSE;
        }
        else if (Pot_Has_Changed == FALSE)
        {
            Shift = (Offset - POTMIN)*(TABLE_LENGTH)/(POTRANGE + 1);
            CyDmaTdSetAddress(DMA2_TD[0], LO16((uint32)&sineTable[360-Deg]), LO16((uint32)VDAC8_Offset_Data_PTR));   
        }
         //map the shifting value to full 360 degree rotation
        Deg = (Shift)*CIRCLEDEG/(TABLE_LENGTH - 1);
        LCD_Position(0u,0u);
        LCD_ClearDisplay();
        //LCD_PrintString("Phase Shift:");
        LCD_PrintNumber(Pot_Has_Changed);
        LCD_Position(1u,0u);
        sprintf(str,"%d Degrees",(int)Deg);
        LCD_PrintString(str);
    }

}
/* [] END OF FILE */
