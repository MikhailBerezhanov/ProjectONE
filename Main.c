/*==============================================================================
>Project name:  Project_One
>Brief:         Firmware for getting data from gyro+accel bmi160 BMI160 and
                magnetometr , filter that values with MadgwickAHRS and send via 
                Bluetooth HC-05 and USART1 STM32F103C8 to COM port (Serial_)
>Author:        Berezhanov M.
>Date:          29.01.2018
>Version:       0.1
==============================================================================*/

//----------------------------= #[I]ncludes =-----------------------------------
#include "Main.h"					// all project defines there
#include "Menu.h"					// lib for MainMenu display via USART Terminal 
#include "stm32f10x_Inits.h"		// periph devices libs
#include "BMI160_func.h"			// lib for BMI160 and BMM150
#include "IRQ_Handlers.h"
//-----------------------------= #[D]efines =-----------------------------------

				/*watch Main.h. All project settings defines there*/

//---------------------------= 101[V]ariables =---------------------------------
extern struct bmi160_dev bmi160;
extern struct bmi160_int_settg int_config;
extern struct bmi160_pmu_status pmu_status;	
extern struct bmi160_sensor_data accel;
extern struct bmi160_sensor_data gyro;	
extern struct bmi160_common_status sensor_status;
extern struct bmm150_dev bmm150;

volatile char currTime[9] = __TIME__;
volatile char currDate[11] =  __DATE__ ;

extern volatile char USART1_TxBuf[TX_BUF_SIZE];
extern USART_InitTypeDef USART2_InitStructure;
extern int8_t rsltBMI160;
extern int8_t rsltBMM150;
struct MainMenu_Struct MainMenu;

volatile bool SENSOR_FLAG_DATA_RECIEVED = false;
//****DEBUG****
_Bool state = 0;
int spi_TxData = 11;

//----------------------------= [P]rototypes ; =--------------------------------
void MainMenu_init(void);

//------------------------------= functions =-----------------------------------
int main (void)                 // >>>_MAIN_<<<
{
//SystemInit();    
SetSysClockTo_72();
	
USART1_init();	
USART2_init();

MainMenu_init();

SPI2_MASTER_init();

rsltBMI160 = BMI160_init();		
rsltBMM150 = BMM150_init();
	
GPIO_init();
	
MainMenu_startMess(&MainMenu);
	
		uint8_t chip_id = 0;
		bmm150_get_regs(BMM150_CHIP_ID_ADDR, &chip_id, 1, &bmm150);
		sprintf((char*)USART1_TxBuf, "\r\nBMM150 CHIP ID: %X", chip_id );
		USART1_SEND(USART1_TxBuf);	
	
	while(1)
	{
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)) GPIO_ResetBits(GPIOC,GPIO_Pin_13);		//turn led ON	
		
		
	MainMenu_checkInput(&MainMenu);
	MainMenu_checkAnswer(&MainMenu);	
     /*  
    sprintf(usart_TxBuf,"\r\n MAG DATA X : %d \r\n", bmm150.data.x);
    USART1_SEND(usart_TxBuf);
    sprintf(usart_TxBuf,"\r\n MAG DATA Y : %d \r\n", bmm150.data.y);
    USART1_SEND(usart_TxBuf);
    sprintf(usart_TxBuf,"\r\n MAG DATA Z : %d \r\n", bmm150.data.z);
    USART1_SEND(usart_TxBuf);
    _delay_ms(100);
    */    
		
		//**-------DEBUGGING-------**
	//SPI2_Read_BMI160(1,(BMI160_CHIP_ID_ADDR | BMI160_SPI_RD_MASK),&id,1);	
	//SPI2_Read_BMI160(1,(BMI160_ACCEL_DATA_ADDR | BMI160_SPI_RD_MASK),&id,3);	
	//SPI2_Write_BMI160(1,(BMI160_CHIP_ID_ADDR & BMI160_SPI_WR_MASK),&id,1);
	//SPI_Send(spi_TxData);
		
		if (SENSOR_FLAG_DATA_RECIEVED)
		{
		/*
		SENSOR_FLAG_DATA_RECIEVED = false;
		USART1_SEND("Orientation: ");
		sprintf(usart_TxBuf, "%f  ", fdata.heading );
		USART1_SEND(usart_TxBuf);
		sprintf(usart_TxBuf, "%f  ", fdata.pitch );
		USART1_SEND(usart_TxBuf);
		sprintf(usart_TxBuf, "%f\r\n", fdata.roll );
		USART1_SEND(usart_TxBuf);
		
		BMI160_display_data();	
		_delay_ms(100);
		*/
		
		//_delay_ms(100);
		}
		
		/*
		if (RX1_FLAG_END_LINE) 
		{           
		RX1_FLAG_END_LINE = false;		// Reset END_LINE Flag
		USART1_SEND("\r\nI have received a line:\r\n");
		USART1_SEND(USART1_RxBuf);
		USART1_SEND("\r\n");			//new line
						
			sscanf((char*)USART1_RxBuf,"%s%x",buffer,&spi_TxData);
			if (strncmp((char*)buffer,"spisend", 7) == 0)
			{
			SPI_Send(spi_TxData);
			sprintf(usart_TxBuf, "\r\n\tyou have send: %X", spi_TxData );
			USART1_SEND(usart_TxBuf);			
			USART1_SEND("\r\n");
			}
			
		Clear_Buffer(USART1_RxBuf);				//clear RX buffer
		}
		*/
	} //end of while

}//end of main


/**
  * @brief   Fills MainMenu structure with necessary values and functions
  * @note   
  * @param  
  * @retval 
  */
void MainMenu_init(void)
{
MainMenu.terminalSend = USART1_SEND;
MainMenu.hc05Send = USART2_SEND;
MainMenu.usart2Struct = &USART2_InitStructure;
}

