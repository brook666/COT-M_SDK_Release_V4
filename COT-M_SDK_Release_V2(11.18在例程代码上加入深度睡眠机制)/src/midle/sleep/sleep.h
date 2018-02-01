

#ifndef SLEEP_H
#define SLEEP_H

#include <stdint.h>

enum SLEEPSTATAUS
{
	NoneSave        	= 0x00,
	SaveInstructionRam 	= 0x02,
	SaveDataBuffer  	= 0x04,
	SaveDataRam     	= 0x08
	
};

void hal_sleep( uint32_t osal_timeout );




#endif
