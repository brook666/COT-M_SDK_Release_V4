#include "cmfr.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "usb_desc.h"
#include "usb_prop.h"

#if (defined(ENABLE_USB))
/* file interface: open
   when args is not 0, it is baudrate config. Default baudrate is setting when args is 0. */
static int32_t USB_Open( struct File* file, uint32_t args )
{
    file->count++;/* use count + 1 */   
	
	return 0;
}

/* file ops: release */
static int32_t USB_Release( struct File* file )
{
	file->count--;/* use count - 1 */
    
	return 0;
}

/* file ops: read */
uint32_t USB_Read( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
	uint32_t len,ReceiveLen=0;
	uint8_t* data = (uint8_t*)buf;	

	if (bDeviceState != CONFIGURED)	
	{	
		return 0;
	}

	while(len < count)
	{
		ReceiveLen = USB_SIL_Read(UsbReadEndp, data);

		data += ReceiveLen;
		len += ReceiveLen;
	}
	
	return count;
}

/* file ops: write */
uint32_t USB_Write( struct File* file, void* buf, uint32_t count, uint32_t* pos )
{
	uint32_t len;	
    uint8_t* data = (uint8_t*)buf;
	uint32_t templen = count;
	uint8_t  temp[64];	
	uint8_t  type = CONF_GetUsbCommType();

	/* data which is send to usb must be split to several pieces 
	   according to CUSTOMHID_SIZ_END1_IN_SIZE */

	if (bDeviceState != CONFIGURED)	
	{	
		return 0;
	}
		
	while(templen)
	{		
		if(GetEPTxStatus(UsbWriteEndp) == EP_TX_NAK)
		{			
			memset(temp, 0, sizeof(temp));
			len = (templen<UsbWriteDataSize)?templen:UsbWriteDataSize;
			memcpy(temp, data, len);

			data += len;
			templen -= len;
			
			if((type == CONF_USBTYPE_HID)
#if (defined(ENABLE_USB_FIVE_HIDKB))
			  ||(type == CONF_USBTYPE_FIVE)
#endif
			  )
			{
				len = sizeof(temp);
			}

			UserToPMABufferCopy(temp, UsbWriteEndpTxaddr, len); 
			SetEPTxCount(UsbWriteEndp, len);     		
	
			SetEPTxValid(UsbWriteEndp);
			
		}
	}
	
	return count;
}

/* file ops: ioctl */
int32_t  USB_Ioctl( struct File* file, int32_t request, uint32_t args )
{
	int8_t ret = 0;											/* ret value */
   	
	switch( request )
	{
		case REQ_SET_BUADRATE:/* set baudrate */
			
			break;/* end of setting baudrate */

        case REQ_IT_RXNE:/* rxnx interrupt */            
            
            break;
		default:
			ret = -1;
			break;
	}  	

	return ret;
}

/* file operation */
struct FileOps usb_ops = 
{
   USB_Open,		    /* open */
   USB_Release,	 	/* release */
   USB_Read,		    /* read */
   USB_Write,	   		/* write */
   NULL,				/* lseek */
   USB_Ioctl,			/* ioctl */
};

/* usart1 file descriptor */
struct File file_usb =
{
	PATH_USB,  	    /* path */
    0,				    /* pos */
    &usb_ops,	   	    /* ops */
    0             ,  /* private */
    0,	  			    /* count */
};

//#endif


//extern uint8_t CustomHID_DeviceDescriptor[CUSTOMHID_SIZ_DEVICE_DESC];

uint16_t USB_InitPid(uint8_t type, uint16_t vid, uint16_t pid)
{
	if(vid != 0x0403)
	{
		return pid;
	}

	if((type<=0)||(type>USBTYPE_NUM))	
	{
		return pid;
	}

	return (USB_IDPRODUCT_BASE-((type-1)<<4));
}

int32_t USB_InitModule(void)
{
	uint8_t type = CONF_GetUsbCommType();
	
	if((CONF_GetUsbCommFunc()&CONF_USBFUNC_MCUUSB) == 0)
	{
		return ERR_UNDEFINE;
	}

	if((type<=0)||(type>USBTYPE_NUM))//now not support ccid
	{
		return ERR_UNDEFINE;
	}

	USB_DescriptorInit(type,s_conf.vid,s_conf.pid);	

	if((type == CONF_USBTYPE_HID)||(type == CONF_USBTYPE_VSC)
        ||(type == CONF_USBTYPE_CCID)
#if ((defined(ENABLE_USB_FIVE_HIDKB)) || (defined(ENABLE_USB_FIVE_VSCKB)))
	  ||(type == CONF_USBTYPE_FIVE)
#endif
	  )
	{
		register_dev( &file_usb );
	}		

	Set_System();
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	
	return ERR_SUCCESS;		
}
#endif

