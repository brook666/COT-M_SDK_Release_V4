/*----------------------------------------------------------------------
* $Id
*
* Copyright (C), 2015-2016, COT IOT Team Information Co.,Ltd,All Rights Reserved
*
* FileName: flash.c
*
* Author: wuwd
*
* Version: 1.0
*
* Date: 2017-03-17
*
* Description: the functions of serial virtual file system
*
* Function List:
*
* History:
*--------------------------------------------------------------------------*/

/*********************************************************************
 * INCLUDES
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "error.h"
#include "vfs.h"
#include "flash.h"


#include "am_hal_flash.h"


/* macro definition */
#define FLASH_LOCKED        (0)     /* flash is locked */
#define FLASH_UNLOCKED      (1)     /* flash is unlocked */
#define FLASH_BASE          (0)     /* flash is BASE */
#define FLASH_COMPLETE      (0)     /* SUCCESS */



/*********************************************************************
 * LOCAL VARIABLES
 */


/* file interface: open */
static int32_t FLASH_Open( struct File* file, uint32_t args )
{
    if( 0 == file->count && strncmp(file->path,PATH_FLASH,4) == 0 )
    {
        file->count++;/* use count + 1 */
        return ERR_SUCCESS ;
    }

	return ERR_PARAMETER;
}

/* file ops: release */
static int32_t FLASH_Release( struct File* file )
{
    uint32_t unlock =  (uint32_t)file->private_pramaer;   /* write lock status */

    if( strncmp(file->path,PATH_FLASH,4) == 0 )
    {
        if( file->count > 0 )
            file->count--;/* use count - 1 */

        /* lock bank */
        if( unlock == FLASH_UNLOCKED )
        {
            //FLASH_lock();
            file->private_pramaer = (uint32_t*)FLASH_LOCKED;
        }

        return ERR_SUCCESS ;
    }

	return ERR_FAILUER;
}

/* file ops: read */
int32_t FLASH_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
    uint32_t i;
    uint32_t dwNewPos = FLASH_BASE + *pos;        /* new position */
    uint8_t* data = (uint8_t*)buf;

    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_FLASH,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

	for( i = 0; i < count; i++, dwNewPos++ )
	{
		data[i] = (uint8_t) *((uint8_t*)dwNewPos);
	}
    *pos += count;

    return count;
}

/* file ops: write */
int32_t FLASH_Write( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{

    uint32_t dwaddr;                              /* new position */
	uint32_t i =  (uint32_t)file->private_pramaer;          /* write lock status and loop var */
    volatile int32_t  i32ReturnCode = 0;            /* flash status */
    uint32_t  datacount = 0;
    uint32_t* data = (uint32_t*)buf;

    if( NULL == file ||
        NULL == buf  ||
        0 == count   ||
        NULL == pos  ||
        strncmp(file->path,PATH_FLASH,4) != 0 )
    {
        return ERR_PARAMETER ;
    }

    /* if flash is locked, unlock it first */
    if( i == FLASH_LOCKED )
    {
        //FLASH_Unlock();
        file->private_pramaer = (uint32_t*)FLASH_UNLOCKED;
    }

    dwaddr = FLASH_BASE + *pos;
    if( dwaddr & 0x03 ) /* flash must be writen by  word. */
        return 0;
    if( count & 0x03 )/* flash must be writen by  word. */
    {
        count -= (count & 0x03);
    }

    if(count > 0)
    {
        /* erase the page */
        i32ReturnCode = am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
                                       (uint32_t)AM_HAL_FLASH_ADDR2INST(dwaddr),
                                       (uint32_t)AM_HAL_FLASH_ADDR2PAGE(dwaddr));
    }
    if( i32ReturnCode == FLASH_COMPLETE )
    {
        /* write word by word */
        for( i = 0; i < count; i += datacount, dwaddr += datacount )
        {
            datacount = FLASH_PAGE_SIZE - (dwaddr & FLASH_PAGE_SIZE_MASK);
            if(datacount > count) datacount = count;
            /* if pos is align to page size, erase the page */
            if((i != 0)&&((dwaddr & FLASH_PAGE_SIZE_MASK) == 0 ))
            {
                i32ReturnCode = am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
                                       (uint32_t)AM_HAL_FLASH_ADDR2INST(dwaddr),
                                       (uint32_t)AM_HAL_FLASH_ADDR2PAGE(dwaddr));
                if( i32ReturnCode != FLASH_COMPLETE )
                {
                    break;
                }
            }

            // Write the psImage structure directly to the flag page.
            i32ReturnCode = am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                                  (uint32_t *) data,
                                  (uint32_t *)dwaddr,
                                  (datacount>>2));
            if( i32ReturnCode != FLASH_COMPLETE )
            {
                break;
            }
        }
    }

    *pos =  dwaddr - FLASH_BASE;

    file->private_pramaer = (uint32_t*)FLASH_LOCKED;

	return i;
}

/* file ops: lseek */
int32_t  FLASH_Lseek( struct File* file, int32_t offset, uint8_t whence )
{
    switch( whence )
    {
        case SEEK_SET:
            file->pos = (uint32_t)offset;
            break;
        case SEEK_CUR:
            file->pos += offset;
            break;
        case SEEK_END:
            //file->pos = FLASH_SIZE + offset;
            break;
    }
    return file->pos;
}

/* file operation */
struct FileOps flash_ops =
{
   FLASH_Open,		    /* open */
   FLASH_Release,	 	/* release */
   FLASH_Read,		    /* read */
   FLASH_Write,	   		/* write */
   FLASH_Lseek,			/* lseek */
   NULL,			    /* ioctl */
   NULL,                /* irq handler regiser */
};

/* usart1 file descriptor */
struct File file_flash =
{
	PATH_FLASH,  	/* path */
    0,				/* pos */
    &flash_ops,	   	/* ops */
    (uint32_t*)FLASH_LOCKED,   /* private */
    0,	  			/* count */
};


/* init flash character driver */
int32_t FLASH_InitModule(void)
{
    register_dev( &file_flash );
	return 0;
}


