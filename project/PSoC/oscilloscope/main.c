/* ========================================
 *CMPE121/L Spring 2019
 * Nick Jannuzzi 
 * Lab Project: Create a dual channel oscilloscope using the PSoC 5 and the Raspberry Pi
 * The PSoC side handles sampling the potentiometers, which move the two channels up and down, as well and streams data to the Pi using a USB bulk transfer
 * ========================================
*/
//Libraries to include
#include "project.h"

//Macro Definitions
#define ARRAY_SIZE 64
#define PINGFLAG 1
#define PONGFLAG 0
#define TRUE 1
#define FALSE 0
#define POTMIN 0
#define POTMAX 255

//USBFS Definitions
#define USBFS_DEVICE (0u)
#define IN_EP_NUM (1u)
#define IN2_EP_NUM (2u)
#define OUT_EP_NUM (3u)
#define BUFFER_SIZE (64u)

//I2C definitions
#define RD_BUFFER_SIZE        (2u)
#define WR_BUFFER_SIZE        (2u)

//DMA definitions
#define DMA1_BYTES_PER_BURST 1
#define DMA1_REQUEST_PER_BURST 1
#define DMA1_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA1_DST_BASE (CYDEV_SRAM_BASE)

#define DMA2_BYTES_PER_BURST 1
#define DMA2_REQUEST_PER_BURST 1
#define DMA2_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA2_DST_BASE (CYDEV_SRAM_BASE)

//Ping-Pong Buffer Arrays
uint8 PPB1[ARRAY_SIZE];
uint8 PPB2[ARRAY_SIZE];
uint8 PPB3[ARRAY_SIZE];
uint8 PPB4[ARRAY_SIZE];
//DMA variable declarations
//Channel 1
uint8 DMA1_Chan;
uint8 DMA1_TD[2];
//Channel 2
uint8 DMA2_Chan;
uint8 DMA2_TD[2];
//USBFS definitions
#define USBFS_DEVICE (0u)
#define IN_EP_NUM (1u)
#define IN2_EP_NUM (2u)
#define BUFFER_SIZE (64u)
//flags for DMA
char DMA_FLAG = PINGFLAG;
char DMA_FLAG2 = PINGFLAG;
char SAMPLE_FLAG = FALSE;
//USB Flags
int IN1Flag = FALSE;
int IN2Flag = FALSE;
//Define Interrupts to use on PSoC
//DMA interrupts
//Assist with toggling flags to use the Ping Pong buffer method from lab exercises 4/5
//DMA interrupt #1
CY_ISR(DMA1_ISR)
{
    //toggle the flags to control which array we transfer to
    if(DMA_FLAG == PINGFLAG)DMA_FLAG = PONGFLAG;
    else if(DMA_FLAG == PONGFLAG)DMA_FLAG = PINGFLAG;
    IN1Flag = TRUE;
}

//DMA interrupt #2
CY_ISR(DMA2_ISR)
{
    //toggle the flags to control which array we transfer to
    if(DMA_FLAG2 == PINGFLAG)DMA_FLAG2 = PONGFLAG;
    else if(DMA_FLAG2 == PONGFLAG)DMA_FLAG2 = PINGFLAG;
    IN2Flag = TRUE;
}

//Timer ISR
CY_ISR(SAMPLE_ISR)
{
    //sets flag to sample potentiometer values
    SAMPLE_FLAG = TRUE;     
    Sampling_Timer_ReadStatusRegister();
}


int main(void)
{
    //initialize global interrupt
    CyGlobalIntEnable; 
    //initialize components
    
    //ISRs
    DMA1_ISR_StartEx(DMA1_ISR);
    DMA2_ISR_StartEx(DMA2_ISR);
    SAMPLE_ISR_StartEx(SAMPLE_ISR);
    
    //MUX
    AMux_Start();
    AMux_FastSelect(0);
    //SAR ADCs
    ADC_SAR_1_Start();
    ADC_SAR_2_Start();
    ADC_SAR_1_StartConvert();
    ADC_SAR_2_StartConvert();
    
    //Delta Sigma ADC
    ADC_DelSig_Start();
    ADC_DelSig_StartConvert();
     
    //I2C
    uint8 read_buffer[RD_BUFFER_SIZE];
    uint8 write_buffer[WR_BUFFER_SIZE];
    uint8 readbyteCount = 0u;
    uint8 potval1,potval2 = 0;
    // Command register to capture writes from I2C
    int Sample_Rate = 0; 
    // Set up slave write data buffer
    I2CS_SlaveInitWriteBuf((uint8 *) write_buffer, WR_BUFFER_SIZE);   
    // Set up slave read data buffer
    I2CS_SlaveInitReadBuf((uint8 *) read_buffer, RD_BUFFER_SIZE);
    // Start I2C slave component
    I2CS_Start();
    
    
    //Timer
    Sampling_Timer_Start();
    
    
    //USBFS
    USBFS_Start(USBFS_DEVICE, USBFS_5V_OPERATION);
    

    
    //USB setup stuff
    while(0u == USBFS_GetConfiguration()){}
    //USBFS_EnableOutEP(OUT_EP_NUM);
    //DMA Setup
    /* DMA Configuration for DMA1 */
    DMA1_Chan = DMA1_DmaInitialize(DMA1_BYTES_PER_BURST, DMA1_REQUEST_PER_BURST, 
        HI16(DMA1_SRC_BASE), HI16(DMA1_DST_BASE));
    DMA1_TD[0] = CyDmaTdAllocate();
    DMA1_TD[1] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA1_TD[0], ARRAY_SIZE, DMA1_TD[1], DMA1__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA1_TD[1], ARRAY_SIZE, DMA1_TD[0], DMA1__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA1_TD[0], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)PPB1));
    CyDmaTdSetAddress(DMA1_TD[1], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR), LO16((uint32)PPB2));
    CyDmaChSetInitialTd(DMA1_Chan, DMA1_TD[0]);
    CyDmaChEnable(DMA1_Chan, 1);

    /* DMA Configuration for DMA2 */
    DMA2_Chan = DMA2_DmaInitialize(DMA2_BYTES_PER_BURST, DMA2_REQUEST_PER_BURST, 
        HI16(DMA2_SRC_BASE), HI16(DMA2_DST_BASE));
    DMA2_TD[0] = CyDmaTdAllocate();
    DMA2_TD[1] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA2_TD[0], ARRAY_SIZE, DMA2_TD[1], DMA2__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA2_TD[1], ARRAY_SIZE, DMA2_TD[0], DMA2__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA2_TD[0], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)PPB3));
    CyDmaTdSetAddress(DMA2_TD[1], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)PPB4));  
    CyDmaChSetInitialTd(DMA1_Chan, DMA2_TD[0]);
    CyDmaChEnable(DMA2_Chan, 1);
    
    //main loop
    for(;;)
    {
        
        //USBFS code begin    
        //EP 1 USB code
        //check if we have an empty buffer and the flag is set
        if(USBFS_IN_BUFFER_EMPTY == USBFS_GetEPState(IN_EP_NUM))
        {
           if(IN1Flag == TRUE)
            {
                   //cycle thorugh cascade buffer setup and load endpoint for USB transfer 
                   if(DMA_FLAG == PONGFLAG)
                   {
                        USBFS_LoadInEP(IN_EP_NUM,PPB1,ARRAY_SIZE);
                       // IN1Flag = FALSE;
                   }else if(DMA_FLAG == PINGFLAG)
                   {
                        USBFS_LoadInEP(IN_EP_NUM,PPB2,ARRAY_SIZE);
                        //IN1Flag = FALSE; 
                   }
            }
        IN1Flag = FALSE;   
        }
        
    
        //EP 2 USB code
        //do nothing for some time
        if(USBFS_IN_BUFFER_EMPTY == USBFS_GetEPState(IN2_EP_NUM))
        {
            if(IN2Flag == TRUE)
            {
                   //cycle thorugh cascade buffer setup and load endpoint for USB transfer             
                   if(DMA_FLAG2 == PONGFLAG)
                   {
                        USBFS_LoadInEP(IN2_EP_NUM,PPB3,ARRAY_SIZE);
                        //IN2Flag = FALSE; 
                   }else if(DMA_FLAG2 == PINGFLAG)
                   {
                        USBFS_LoadInEP(IN2_EP_NUM,PPB4,ARRAY_SIZE);
                        //IN2Flag = FALSE; 
                   }
            }   
        IN2Flag = FALSE;  
        }
        //USBFS code ends    
            
            
      
        //I2C main loop code    
         /* Check if the command register has been written */
         if(I2CS_SlaveStatus() & I2CS_SSTAT_RD_CMPLT)
           {
                // Copy command into command register
                Sample_Rate = write_buffer[0];
                
                // Clear status
                I2CS_SlaveClearWriteStatus();
                I2CS_SlaveClearWriteBuf();
                //taking sample rate value from Pi command parser struct and setting clock divider 
                //switch statement provides a case for each input

                    //clock driving DMA request runs at a frequency of 24MHz
                    //divider value scales it down appropriately to the desired clock value
                if(Sample_Rate == 100)
                {
                    DMA_CLK_SetDividerValue(1);    
                }else if (Sample_Rate == 50)
                {
                    DMA_CLK_SetDividerValue(2);   
                }else if (Sample_Rate == 20)
                {
                    DMA_CLK_SetDividerValue(5);   
                }else if (Sample_Rate == 10)
                {
                    DMA_CLK_SetDividerValue(10);   
                }else if (Sample_Rate == 1)
                {
                    DMA_CLK_SetDividerValue(100);   
                }

                
            }   
            
            
         /* Check if the slave buffer has been read */
         if(I2CS_SlaveStatus() & I2CS_SSTAT_WR_CMPLT)
            {
                readbyteCount = I2CS_SlaveGetReadBufSize();
                /* If both bytes of the read buffer have been read */
                if(readbyteCount == RD_BUFFER_SIZE)
                {
                    // Clear read status
                    I2CS_SlaveClearReadStatus();
                    I2CS_SlaveClearReadBuf();
                    /* Disable the interrupts when writing to the Read buffer
                     * so that the Master cannot read the slave buffer during the act of writing
                     * to the buffer.
                     */
                    CyGlobalIntDisable;
                    SAMPLE_FLAG = TRUE;
                    if(SAMPLE_FLAG == TRUE)
                    {
                        //sampling the potentiometers
           
                        //set the mux to the channnel corresponding to potentiometer 1
                        AMux_FastSelect(0);                        
                        potval1 = ADC_DelSig_GetResult16();
                        //set the mux to the channnel corresponding to potentiometer 2    
                        if(potval1 > POTMAX)
                        {
                            potval1 = POTMAX;
                        }
                        if(potval1 < POTMIN)
                        {
                            potval1 = POTMIN;
                        }
                        AMux_FastSelect(1);
                        potval2 = ADC_DelSig_GetResult16();
                        //saturation(Over/underflow handle
                        //2nd potentiometer
                        //2nd potentiometer
                        
                        if(potval2 > POTMAX)
                        {
                            potval2 = POTMAX; 
                        }
                        if(potval2 > POTMIN)
                        {
                            potval2 = POTMIN;    
                        }
                        //put the potentiometer values into buffers
                        read_buffer[0] = potval1;
                        read_buffer[1] = potval2;

                        // Turn the interrupts back on
                        SAMPLE_FLAG = FALSE;
                    }
                    CyGlobalIntEnable;
                }
            }
    //End I2C code
    
    }
}

    
    
    





