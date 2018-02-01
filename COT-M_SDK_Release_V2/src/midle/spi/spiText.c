/*----------------------------------------------------------------------
* $Id
*
* Copyright (C), 2015-2016, COT IOT Team Information Co.,Ltd,All Rights Reserved
*
* FileName: spiTest.c
*
* Author: hzx
*
* Version: 1.0
*
* Date: 2017-03-22
*
* Description: the functions of serial virtual file system
*
* Function List:
*
* History:
* 测试描述：修改spi_data的值如0x07
*--------------------------------------------------------------------------*/
#include "spi.h"
#include "vfs.h"

void spi_text()
{	
	uint32_t spi_fd;
	uint8_t spi_data = 0;
	uint8_t read_value = 0;
	spi_init_module();
	spi_fd = open(PATH_SPI,AM_MC3630_IOM);
	
	ioctl( spi_fd, SLAVE_CHIPSELECT, AM_MC3630_CS );  
    //ioctl( spi_fd, GPIO_INTERRUPT, AM_MC3630_INT1 );
	/*读取芯片是否挂载*/
	lseek( spi_fd, 0X18 , SPI_READ_ADDER);
	read(spi_fd,&spi_data,1);
	if(0x71 == spi_data)  //需要打断点 检测spi_data的值
	{
		lseek( spi_fd, 0X19 , SPI_READ_ADDER);
		read(spi_fd,&spi_data,1);
	}
	/*read FREG_1 reg and write*/
	#if 1
	lseek( spi_fd, FEATURE_C_1 , SPI_READ_ADDER);
	read(spi_fd,&read_value,1);
	spi_data |= 0x06;
	spi_data &= 0x3E;
	spi_data |= 0x80;
	
	lseek( spi_fd, FEATURE_C_1 , SPI_WRITE_ADDER);
	write(spi_fd,&spi_data,1);
	lseek( spi_fd, FEATURE_C_1 , SPI_READ_ADDER);
	read(spi_fd,&read_value,1);
	#endif
}//需要打断点 检测spi_data和read_value的值
