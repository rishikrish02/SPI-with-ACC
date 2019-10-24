/*
 * acc.c
 *
 *  Created on: Sep 11, 2019
 *      Author: rishi
 */


#include "acc.h"

#include "em_device.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "em_chip.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "core_cm4.h"


void spi_setup() 													//spi setup
{
	CMU_ClockEnable(cmuClock_HFPER,true);
	CMU_ClockEnable(cmuClock_GPIO,true);
	CMU_ClockEnable(cmuClock_USART1,true);


	GPIO_PinModeSet(gpioPortC,9,gpioModePushPull,1); 				//enabling he chip select
	GPIO_PinModeSet(gpioPortC,6,gpioModePushPull,1);				//MOSI
	GPIO_PinModeSet(gpioPortC,7,gpioModeInput,0);					//MISO
	GPIO_PinModeSet(gpioPortC,8,gpioModePushPull,1);			 	//CLK

	USART_InitSync_TypeDef initdata = USART_INITSYNC_DEFAULT;


	initdata.clockMode=usartClockMode3;								//sample on rising edge
	initdata.msbf=true;
	initdata.autoCsEnable=true;                            			//autochipselect enable is set
	initdata.master=true;
	initdata.baudrate=1000000;
	initdata.enable=usartDisable;

	USART_InitSync(USART1,&initdata);



	USART1->ROUTELOC0 = (USART_ROUTELOC0_CLKLOC_LOC11)| 		      				//CLK
						(USART_ROUTELOC0_CSLOC_LOC11)|			    				//CS
						(USART_ROUTELOC0_TXLOC_LOC11)| 			   					//mosi
						(USART_ROUTELOC0_RXLOC_LOC11);  		    				//miso

	USART1->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN; 		//enabling

	USART_Enable(USART1,usartEnable);

}


void spi_tx(uint8_t address, uint8_t data)
{
	//GPIO_PinOutClear(gpioPortC,9);                  		//pulling the chip select pin low

	while(!(USART1->STATUS & USART_STATUS_TXBL));   	    //while this is not true, transmit buffer not empty
	USART_TxDouble(USART1,address);
	while(!(USART1->STATUS & USART_STATUS_RXDATAV));	    //recieve buffer not empty, need to read, get out of while loop
	USART_RxDouble(USART1);
	while(!(USART1->STATUS & USART_STATUS_TXBL));
	USART_TxDouble(USART1,data);
	while(!(USART1->STATUS & USART_STATUS_RXDATAV));
	USART_RxDouble(USART1);

	//GPIO_PinOutSet(gpioPortC,9);						    //back to high

}
uint16_t spi_rx(uint8_t address)
{
	//done so that READ bit is set as 1
	address = 0x80 | address;
	while(!(USART1->STATUS & USART_STATUS_TXBL));
	USART_TxDouble(USART1,address);
	while(!(USART1->STATUS & USART_STATUS_RXDATAV));
	USART_RxDouble(USART1);
	while(!(USART1->STATUS & USART_STATUS_TXBL));
	USART_TxDouble(USART1,0x00);								    	//dummy
	while(!(USART1->STATUS & USART_STATUS_RXDATAV));


	return USART_RxDouble(USART1);
}
