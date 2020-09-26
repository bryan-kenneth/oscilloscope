/* ========================================
 *
 *
 * ======================================== */

#include "project.h"
#include <stdio.h>

#include <string.h>

// USB device number.
#define USBFS_DEVICE (0u)

// Active endpoint of USB device.
#define CH1_IN_EP    (1u)
#define CH2_IN_EP    (2u)

#define RD_BUFFER_SIZE        (2u)
#define WR_BUFFER_SIZE        (1u)

#define BUFF_SIZE 64

// Prototypes
void CH1_DMA_config(void);
void CH2_DMA_config(void);
void HandleSampleRate(void);

uint8 read_buffer[RD_BUFFER_SIZE] = {0};
uint8 write_buffer[WR_BUFFER_SIZE] = {0};

CY_ISR(TIMER_I) {
    /* Check if the slave buffer has been read */
    if(I2CS_SlaveStatus() & I2CS_SSTAT_RD_CMPLT) {
        int read_count = I2CS_SlaveGetReadBufSize();
        /* If both bytes of the read buffer have been read */
        if(read_count == RD_BUFFER_SIZE) {
            // Clear read status
            I2CS_SlaveClearReadStatus();
            I2CS_SlaveClearReadBuf();
            /* Disable the interrupts when writing to the Read buffer
            * so that the Master cannot read the slave buffer during the act of writing
            * to the buffer. */
            
            uint8 ch1_pot, ch2_pot;
            POT_MUX_FastSelect(0);
            ch1_pot = POT_ADC_Read16();
            if (ch1_pot == 255) ch1_pot = 0;
            
            POT_MUX_FastSelect(1);
            ch2_pot = POT_ADC_Read16();
            if (ch2_pot == 255) ch2_pot = 0;
            
            CyGlobalIntDisable;
            
            read_buffer[0] = ch1_pot;
            read_buffer[1] = ch2_pot;
         
            // Turn the interrupts back on
            CyGlobalIntEnable;
        }
    }
}

uint8 ch1_sel = 0,            ch2_sel = 0;          // Keeping track of which buffer the DMA is filling
uint8 ch1_flag[2] = {0, 0},   ch2_flag[2] = {0, 0}; // 1 when full
uint8 ch1_buff[2][BUFF_SIZE], ch2_buff[2][BUFF_SIZE];

CY_ISR(CH1_DMA_I) {
    ch1_flag[ch1_sel] = 1;
    ch1_sel = !ch1_sel;
}

CY_ISR(CH2_DMA_I) {
    ch2_flag[ch2_sel] = 1;
    ch2_sel = !ch2_sel;
}

int main(void)
{   
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    LCD_Start();
    
    // Set up slave write data buffer
    I2CS_SlaveInitWriteBuf((uint8 *) write_buffer, WR_BUFFER_SIZE);   
    // Set up slave read data buffer
    I2CS_SlaveInitReadBuf((uint8 *) read_buffer, RD_BUFFER_SIZE);
    // Start I2C slave component
    I2CS_Start();
    
    // Start USBFS operation with 5V operation.
    USBFS_Start(USBFS_DEVICE, USBFS_5V_OPERATION);
    // Wait until device is enumberated by host.
    while (!USBFS_GetConfiguration()){};
    
    // Interrupts
    Ch1_Interrupt_StartEx(CH1_DMA_I);
    Ch2_Interrupt_StartEx(CH2_DMA_I);
    Timer_Interrupt_StartEx(TIMER_I);
    
    // Potentiometer ADC
    POT_ADC_Start();
    POT_ADC_StartConvert();
    
    // DMA initialization
    CH1_ADC_Start();
    CH2_ADC_Start();
    CH1_ADC_StartConvert();
    CH2_ADC_StartConvert();
    CH1_DMA_config();
    CH2_DMA_config();
    
    // Timer initialization
    I2C_Timer_Start();
    DMA_Timer_Start();
    
    for(;;)
    {   
        // if sample rate is availible
        if (I2CS_SlaveStatus() & I2CS_SSTAT_WR_CMPLT) {
            HandleSampleRate();
        }
        
        // Check if CH1 IN buffer is empty (host has read data).
        if (USBFS_IN_BUFFER_EMPTY == USBFS_GetEPState(CH1_IN_EP)) {
            // Fill CH1 IN buffer if read_flag high for either buffer
            if (ch1_flag[0]) {
                USBFS_LoadInEP(CH1_IN_EP, ch1_buff[0], BUFF_SIZE);
                ch1_flag[0] = 0; // done reading and transmitting
            }
            if (ch1_flag[1]) {
                USBFS_LoadInEP(CH1_IN_EP, ch1_buff[1], BUFF_SIZE);
                ch1_flag[1] = 0; // done reading and transmitting
            }
        }
        
        // Check if CH2 IN buffer is empty (host has read data).
        if (USBFS_IN_BUFFER_EMPTY == USBFS_GetEPState(CH2_IN_EP)) {
            // Fill CH2 IN buffer if read_flag high for either buffer
            if (ch2_flag[0]) {
                USBFS_LoadInEP(CH2_IN_EP, ch2_buff[0], BUFF_SIZE);
                ch2_flag[0] = 0; // done reading and transmitting
            }
            if (ch2_flag[1]) {
                USBFS_LoadInEP(CH2_IN_EP, ch2_buff[1], BUFF_SIZE);
                ch2_flag[1] = 0; // done reading and transmitting
            }
        }
        
    }
}

/* Defines for CH1_DMA */
#define CH1_DMA_BYTES_PER_BURST 1
#define CH1_DMA_REQUEST_PER_BURST 1
#define CH1_DMA_SRC_BASE (CYDEV_PERIPH_BASE)
#define CH1_DMA_DST_BASE (CYDEV_SRAM_BASE)

void CH1_DMA_config() {
    /* Variable declarations for DMA_1 */
    uint8 CH1_DMA_Chan;
    uint8 CH1_DMA_TD[2];
    
    /* DMA Configuration for CH1_DMA */
    CH1_DMA_Chan = CH1_DMA_DmaInitialize(CH1_DMA_BYTES_PER_BURST, CH1_DMA_REQUEST_PER_BURST, 
        HI16(CH1_DMA_SRC_BASE), HI16(CH1_DMA_DST_BASE));
    CH1_DMA_TD[0] = CyDmaTdAllocate();
    CH1_DMA_TD[1] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(CH1_DMA_TD[0], 64, CH1_DMA_TD[1], CH1_DMA__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(CH1_DMA_TD[1], 64, CH1_DMA_TD[0], CH1_DMA__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(CH1_DMA_TD[0], LO16((uint32)CH1_ADC_SAR_WRK0_PTR), LO16((uint32)&ch1_buff[0]));
    CyDmaTdSetAddress(CH1_DMA_TD[1], LO16((uint32)CH1_ADC_SAR_WRK0_PTR), LO16((uint32)&ch1_buff[1]));
    CyDmaChSetInitialTd(CH1_DMA_Chan, CH1_DMA_TD[0]);
    CyDmaChEnable(CH1_DMA_Chan, 1);
}

/* Defines for CH2_DMA */
#define CH2_DMA_BYTES_PER_BURST 1
#define CH2_DMA_REQUEST_PER_BURST 1
#define CH2_DMA_SRC_BASE (CYDEV_PERIPH_BASE)
#define CH2_DMA_DST_BASE (CYDEV_SRAM_BASE)

void CH2_DMA_config() {
    /* Variable declarations for CH2_DMA */
    uint8 CH2_DMA_Chan;
    uint8 CH2_DMA_TD[2];

    /* DMA Configuration for CH2_DMA */
    CH2_DMA_Chan = CH2_DMA_DmaInitialize(CH2_DMA_BYTES_PER_BURST, CH2_DMA_REQUEST_PER_BURST, 
        HI16(CH2_DMA_SRC_BASE), HI16(CH2_DMA_DST_BASE));
    CH2_DMA_TD[0] = CyDmaTdAllocate();
    CH2_DMA_TD[1] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(CH2_DMA_TD[0], 64, CH2_DMA_TD[1], CH2_DMA__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(CH2_DMA_TD[1], 64, CH2_DMA_TD[0], CH2_DMA__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(CH2_DMA_TD[0], LO16((uint32)CH2_ADC_SAR_WRK0_PTR), LO16((uint32)&ch2_buff[0]));
    CyDmaTdSetAddress(CH2_DMA_TD[1], LO16((uint32)CH2_ADC_SAR_WRK0_PTR), LO16((uint32)&ch2_buff[1]));
    CyDmaChSetInitialTd(CH2_DMA_Chan, CH2_DMA_TD[0]);
    CyDmaChEnable(CH2_DMA_Chan, 1);
}

void HandleSampleRate(void) {
    // Read sample rate
    int sample_rate = write_buffer[0];
    // Clear status
    I2CS_SlaveClearWriteStatus();
    I2CS_SlaveClearWriteBuf();
        
    // Change sample rate
    int period = 100;
    switch(sample_rate) {
     case 100: period = 10;   break;
     case  50: period = 20;   break;
     case  20: period = 50;   break;
     case  10: period = 100;  break;
     case   1: period = 1000;
    }
    
    char str[10];
    sprintf(str, "SR: %d", sample_rate);
    LCD_ClearDisplay();
    LCD_Position(0,0);
    LCD_PrintString(str);
    
    sprintf(str, "P: %d", period);
    LCD_Position(1,0);
    LCD_PrintString(str);
    
    
    DMA_Timer_WritePeriod(period);
}

/* [] END OF FILE */
