#ifdef __USE_YX_APP_SERVICE__
#include "kal_release.h"
#include "stack_common.h"  
#include "stack_msgs.h"
#include "app_ltlcom.h"      /* Task message communiction */
#include "task_config.h"     /* Task creation */
#include "stack_ltlcom.h"	/* msg_send_ext_queue.....definitions */
#include "stack_config.h"
#include "kal_trace.h"
#include "task_main_func.h"
#include "dcl.h"
#include "kal_general_types.h"
#include "kal_public_api.h"
#include "Eint.h"
#include "drv_comm.h"
#include "kal_public_defs.h"
#include "gpio_sw.h"
#include "us_timer.h"

/**********************************************************Gsensor gpio define***************************************************/

#define  GSENSOR_I2C_SCL  5
#define  GSENSOR_I2C_SDA  4
#define  GSENSOR_EINT_NO  1
#define  GSENSOR_I2C_DELAY_TIME   350

/**********************************************************Gsensor I2C***********************************************************/

#define   MS_I2C_ACK  1
#define   MS_I2C_NAK  0

#define GSENSOR_CLK_PIN_GPIO_MODE		GPIO_ModeSetup(GSENSOR_I2C_SCL,0)
#define	GSENSOR_DATA_PIN_GPIO_MODE		GPIO_ModeSetup(GSENSOR_I2C_SDA,0)
#define GSENSOR_I2C_CLK_OUTPUT			GPIO_InitIO(OUTPUT,GSENSOR_I2C_SCL)
#define GSENSOR_I2C_DATA_OUTPUT			GPIO_InitIO(OUTPUT,GSENSOR_I2C_SDA)
#define GSENSOR_I2C_DATA_INPUT		   	GPIO_InitIO(INPUT,GSENSOR_I2C_SDA)
#define GSENSOR_I2C_CLK_HIGH		    GPIO_WriteIO(1,GSENSOR_I2C_SCL)
#define GSENSOR_I2C_CLK_LOW				GPIO_WriteIO(0,GSENSOR_I2C_SCL)
#define GSENSOR_I2C_DATA_HIGH			GPIO_WriteIO(1,GSENSOR_I2C_SDA)
#define GSENSOR_I2C_DATA_LOW			GPIO_WriteIO(0,GSENSOR_I2C_SDA)
#define GSENSOR_I2C_GET_BIT				GPIO_ReadIO(GSENSOR_I2C_SDA)

#define GSENSOR_I2C_GPIO_MODE \
{ \
	GSENSOR_CLK_PIN_GPIO_MODE; \
	GSENSOR_DATA_PIN_GPIO_MODE; \
}

#define MS_I2C_DELAY ms_i2c_udelay(3);
void ms_i2c_udelay(kal_uint32 delay)
{
#ifndef WIN32
#if 0
	volatile int count=0;
	kal_uint16  timeT = GSENSOR_I2C_DELAY_TIME;
	for(;count<timeT;count++);
#else
	kal_uint32 ust = 0, ust1 = 0; //ust_get_current_time
	kal_uint32 count = 0;
	kal_uint32 break_count = 0;
	if(delay > 4615) //longer than 1 tick
	{
		kal_sleep_task(delay/4615);
		delay = delay%4615;
	}
	ust = ust_get_current_time();
	do{
		ust1 = ust_get_current_time();
		if(ust1 != ust)
			count++;
		else
			break_count++;
		ust = ust1;
	}while((count < delay) && (break_count < 0xFFFFFF));
#endif
#endif
}

static void GsesnorI2CStart(void)
{
	GSENSOR_I2C_GPIO_MODE;
	GSENSOR_I2C_DATA_OUTPUT;
	GSENSOR_I2C_CLK_OUTPUT;
	GSENSOR_I2C_DATA_HIGH;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_CLK_HIGH;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_DATA_LOW;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_CLK_LOW;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
}

static void GsesnorI2CStop(void)
{
	GSENSOR_I2C_GPIO_MODE;
	GSENSOR_I2C_DATA_OUTPUT;
	GSENSOR_I2C_CLK_OUTPUT;
	GSENSOR_I2C_DATA_LOW;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_CLK_HIGH;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_DATA_HIGH;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_CLK_HIGH;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
}

char GsensorI2CSendByte(kal_uint8 data)
{
	kal_uint8 _a = 0x80;
	GSENSOR_I2C_DATA_OUTPUT;
	GSENSOR_I2C_CLK_OUTPUT;
	while(_a != 0)
	{
		GSENSOR_I2C_CLK_LOW;
		MS_I2C_DELAY;
		MS_I2C_DELAY;
		if (data&_a)
		{
			GSENSOR_I2C_DATA_HIGH;
		}
		else
		{
			GSENSOR_I2C_DATA_LOW;
		}
		MS_I2C_DELAY;
		MS_I2C_DELAY;
		GSENSOR_I2C_CLK_HIGH;
		MS_I2C_DELAY;
		MS_I2C_DELAY;
		_a = _a >> 1;
	}
	GSENSOR_I2C_DATA_INPUT;
	GSENSOR_I2C_CLK_LOW;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_CLK_HIGH;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	_a = GSENSOR_I2C_GET_BIT;
	GSENSOR_I2C_CLK_LOW;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_DATA_OUTPUT;
	return _a;
}

kal_uint8 GsensorI2CReadByte(char bAck)
{
	kal_uint8  ucRet = 0,bit = 0;
	kal_int32  i = 0;
	GSENSOR_I2C_DATA_INPUT;
	GSENSOR_I2C_CLK_OUTPUT;
	for(i = 7; i >= 0; i--)
	{
		GSENSOR_I2C_CLK_LOW;
		MS_I2C_DELAY;
		MS_I2C_DELAY;
		GSENSOR_I2C_CLK_HIGH;
		MS_I2C_DELAY;
		MS_I2C_DELAY;
		bit = GSENSOR_I2C_GET_BIT;
		if(bit)
			ucRet |= bit << i;
		MS_I2C_DELAY;
		MS_I2C_DELAY;
	}
	GSENSOR_I2C_CLK_LOW;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_DATA_OUTPUT;
	if(bAck)
		GSENSOR_I2C_DATA_LOW;
	else
		GSENSOR_I2C_DATA_HIGH;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_CLK_HIGH;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	GSENSOR_I2C_CLK_LOW;
	MS_I2C_DELAY;
	MS_I2C_DELAY;
	return ucRet;
}

char GsensorI2CSend(kal_uint8 ucDeviceAddr, kal_uint8 reg, kal_uint8* pucData, kal_uint32 unDataLength)
{
	char  bRet = 1;
	GsesnorI2CStart();
	if(GsensorI2CSendByte(ucDeviceAddr & 0xFE) == 0)
	{
		if(GsensorI2CSendByte(reg) == 0)
		{
			kal_uint32  i = 0;
			for(i = 0; i < unDataLength; i++)
			{
				if(GsensorI2CSendByte(pucData[i]) == 1)
				{
					bRet = 0;
					break;
				}
			}
		}
		else
			bRet = 0;
	}
	else
		bRet = 0;
	GsesnorI2CStop();
	return bRet;
}

char GsensorI2CReceive(kal_uint8 ucDeviceAddr, kal_uint8 reg, kal_uint8* pucData, kal_uint32 unDataLength)
{
	char   bRet = 1;
	GsesnorI2CStart();
	if(GsensorI2CSendByte(ucDeviceAddr & 0xFE) == 0)
	{
		if(GsensorI2CSendByte(reg) == 0)
		{
			GsesnorI2CStart();
			if(GsensorI2CSendByte(ucDeviceAddr | 0x01) == 0)
			{
				kal_uint32  i = 0;
				for(i = 0; i < unDataLength - 1; i++)
				{
					pucData[i] = GsensorI2CReadByte(1);
				}
				pucData[unDataLength - 1] = GsensorI2CReadByte(0);
			}
			else
				bRet = 0;
		}
		else
			bRet = 0;
	}
	else
		bRet = 0;
	GsesnorI2CStop();
	return bRet;
}


void I2C_ini(void)
{
	GPIO_PullenSetup(GSENSOR_I2C_SDA, KAL_FALSE);
	GPIO_PullSelSetup(GSENSOR_I2C_SDA, KAL_FALSE);	
}

void I2C_close(void)
{
	GSENSOR_I2C_GPIO_MODE;
	
	GSENSOR_I2C_DATA_OUTPUT;
	GSENSOR_I2C_CLK_OUTPUT;
	
	GSENSOR_I2C_DATA_LOW;
	GSENSOR_I2C_CLK_LOW;
}

/**********************************************************HW I2C***********************************************************/

DCL_HANDLE ms_i2c_handle;

void ms_i2c_configure(kal_uint32 slave_addr, kal_uint32 speed)
{
	I2C_CONFIG_T cfg;
	
	//if(!ms_i2c_configure_done)
		ms_i2c_handle = DclSI2C_Open(DCL_I2C, DCL_I2C_OWNER_MS);
	cfg.eOwner = DCL_I2C_OWNER_MS;
	cfg.fgGetHandleWait = KAL_TRUE;
	cfg.u1SlaveAddress = slave_addr;
	cfg.u1DelayLen = 0;
	cfg.eTransactionMode = DCL_I2C_TRANSACTION_FAST_MODE;
	cfg.u4FastModeSpeed = speed;
	cfg.u4HSModeSpeed = 0;
	cfg.fgEnableDMA = KAL_FALSE;
	DclSI2C_Configure(ms_i2c_handle, (DCL_CONFIGURE_T *)&cfg);
	
	//ms_i2c_configure_done = KAL_TRUE;
}

void ms_i2c_close(void)
{
	DclSI2C_Close(ms_i2c_handle);
	//ms_i2c_configure_done = KAL_FALSE;
}

kal_bool ms_i2c_send(kal_uint8 ucDeviceAddr, kal_uint8 ucBufferIndex, kal_uint8* pucData, kal_uint32 unDataLength)
{
	kal_bool bRet = KAL_TRUE;

	kal_uint32 i;
	kal_uint8 write_buf[9];
	I2C_CTRL_CONT_WRITE_T write;
	DCL_STATUS status;
	//if(ms_i2c_configure_done)
	{
		write_buf[0] = ucBufferIndex;
		for(i=0;i<unDataLength;i++)
		{
			write_buf[i+1] = *(pucData+i);
		}
		write.pu1Data = write_buf;
		write.u4DataLen = unDataLength+1;
		write.u4TransferNum = 1;
		status = DclSI2C_Control(ms_i2c_handle, I2C_CMD_CONT_WRITE, (DCL_CTRL_DATA_T *)&write);
		if(status != STATUS_OK)
			return KAL_FALSE;
	}

	return bRet;
}

kal_bool ms_i2c_receive(kal_uint8 ucDeviceAddr, kal_uint8 ucBufferIndex, kal_uint8* pucData, kal_uint32 unDataLength)
{
	kal_bool bRet = KAL_TRUE;

	kal_uint32 i;
	I2C_CTRL_WRITE_AND_READE_T write_read;
	//if(ms_i2c_configure_done)
	{
		write_read.pu1InData = pucData;
		write_read.u4InDataLen = unDataLength;
		write_read.pu1OutData = &ucBufferIndex;
		write_read.u4OutDataLen = 1;
		DclSI2C_Control(ms_i2c_handle, I2C_CMD_WRITE_AND_READ, (DCL_CTRL_DATA_T *)&write_read);
	}

	return bRet;
}

#define MS_BMA250_SLAVE_ADDR	0x30

kal_bool MS_I2C_write_byte(kal_uint8 ucBufferIndex, kal_uint8 pucData)
{
    return ms_i2c_send(MS_BMA250_SLAVE_ADDR,ucBufferIndex,&pucData,1);
}

kal_bool MS_I2C_read(kal_uint8 ucBufferIndex, kal_uint8 *pucData, kal_uint32 unDataLength)
{
	return ms_i2c_receive(MS_BMA250_SLAVE_ADDR,ucBufferIndex,pucData,unDataLength);
}


void SW_i2c_udelay(kal_uint32 delay)
{
    kal_uint32 ust = 0; //ust_get_current_time
    kal_uint32 count = 0;
    kal_uint32 break_count = 0;

    ust = ust_get_current_time();
    do{
        if(ust_get_current_time() != ust)
            count++;
        else
            break_count++;
    }while((count < delay) && (break_count < 0xFFFFFF));
}

void BMA250_delay_ms(kal_uint16 delay)
{
    kal_uint16 i=0;

    for(i=0; i<delay; i++)
    {
        SW_i2c_udelay(1000);
    }
}

void BMA250E_ini(void)
{
	kal_bool result;

	ms_i2c_configure(0x30, 300);//初始化 硬件 I2C   0x30 器件地址  300 速度
	
	result = MS_I2C_write_byte(0x14, 0XB6);  //软复位
	BMA250_delay_ms(1);
	
	result = MS_I2C_write_byte(0x11, 0x0); //设置为 正常工作模式  不进入待机模式
	BMA250_delay_ms(10);
	
	result = MS_I2C_write_byte(0x0F, 0X0c);//设置量程 +-8G    +-2G 0x03   4G: 0x05     8G:0x08   16G:0x0c
	
	result = MS_I2C_write_byte(0x10, 0x09);  //频率  15.63---09

	//中断配置
	MS_I2C_write_byte(0x13  , 0x0);//输出格式

	MS_I2C_write_byte(0x19  , 0x80);//配置INT1  抬手显示
	
	MS_I2C_write_byte(0x1b  , 0x08);//配置INT2  摇一摇接听----------------------
	
	MS_I2C_write_byte(0x1E  , 0x00);//中断用滤波后数据

	MS_I2C_write_byte(0x20  , 0x00);//低电平有效  开漏输出

	MS_I2C_write_byte(0x21  , 0x81);// clear any latched interrupts  temporary, 250 ms   250ms 0X81
	
	MS_I2C_write_byte(0x27  , 0x0c);//摇一摇 监测到几次才触发
	MS_I2C_write_byte(0x29  , 30);//摇一摇 伐值
	
	MS_I2C_write_byte(0x2E  , 40);// 出发的度数  抬手显示
	MS_I2C_write_byte(0x2F  , 0x10);//   

	MS_I2C_write_byte(0x16  , 0x80);//使能中断	
	
	MS_I2C_write_byte(0x18  , 0x01);// 运动 静止  中断---------------------------
	//运动静止  涉及到的寄存器 slo_no_mot_sel  slo_no_mot_dur 
	//slo_no_mot_en_x slo_no_mot_th
}

void BMA250_read_xyz(kal_int16 *X, kal_int16 *Y, kal_int16 *Z)
{
	char MS_sensor_buffer[6];
	
    MS_I2C_read(0x02, MS_sensor_buffer, 6);

    *X = (kal_int16)((((kal_int16)MS_sensor_buffer[1]) << 8) | (kal_int16)MS_sensor_buffer[0]);
    *X = *X >> 6;
    *Y = (kal_int16)((((kal_int16)MS_sensor_buffer[3]) << 8) | (kal_int16)MS_sensor_buffer[2]);
    *Y = *Y >> 6;
    *Z = (kal_int16)((((kal_int16)MS_sensor_buffer[5]) << 8) | (kal_int16)MS_sensor_buffer[4]);
    *Z = *Z >> 6;
	
	// guoshaofeng BMA250是10位数据，需要转换为12位
    *X = *X << 2; /* Acceleration along x-axis*/
    *Y = *Y << 2; /* Acceleration along y-axis*/
    *Z = *Z << 2; /* Acceleration along z-axis*/

    
}

void BMA250_read_flat(unsigned char *p)
{
	MS_I2C_read(0x0C, p, 1);
}

/**********************************************************HW I2C***********************************************************/

		
//-------------------------------EINT----------------------------------

#include "Eint.h"
unsigned char EINT5_flag=0,EINT6_flag=0;
void EINT5_interrupt(void);
void EINT6_interrupt(void);

void EINT_ini(void)
{
	//----------------摇一摇 中断-----------------------------
/*	
		GPIO_InitIO(INPUT,10); \
		GPIO_ModeSetup(10,3); \
		GPIO_PullenSetup(10, KAL_TRUE);
		GPIO_PullSelSetup(10, KAL_TRUE);
	
		EINT5_flag=0;
		
		EINT_Mask(9);
		EINT_SW_Debounce_Modify(9, 5);//5毫秒 消抖
		EINT_Registration(9, KAL_TRUE,KAL_FALSE,EINT5_interrupt, KAL_FALSE);
		EINT_UnMask(9);
	
*/
	//----------------摇一摇 中断-----------------------------
		
	
	
	//----------------抬手显示 中断---------------------------
#if 1
		GPIO_InitIO(INPUT,11); \
		GPIO_ModeSetup(11,3); \
		GPIO_PullenSetup(11, KAL_TRUE);
		GPIO_PullSelSetup(11, KAL_TRUE);
	
		EINT6_flag=0;
	
		EINT_Mask(10);
		EINT_SW_Debounce_Modify(10, 5);
		EINT_Registration(10, KAL_TRUE,KAL_FALSE,EINT6_interrupt, KAL_FALSE);
		EINT_UnMask(10);
#endif
	//----------------抬手显示 中断---------------------------



}

//----------------摇一摇 中断-----------------------------
extern void EINT5_handle(void);
void EINT5_interrupt(void)
{
	EINT_Mask(9);
	if(EINT5_flag==1)
	{

		EINT_SW_Debounce_Modify(9,5);
		EINT_Set_Polarity(9,KAL_FALSE);


		EINT5_flag=0;
		EINT_UnMask(9);
		return;
	}
	else
	{
		static unsigned char flag_diedao=0;
		ilm_struct *fmr_ilm=NULL;
		EINT_SW_Debounce_Modify(9,5);
		EINT_Set_Polarity(9,KAL_TRUE);
		//step++;
		//EINT5_handle();
		if(flag_diedao == 0) flag_diedao = 1;
		else
		{
			DRV_SendPrimitive(fmr_ilm,
							MOD_EINT_HISR,
							MOD_YXAPP,//MOD_MMI,
							MSG_ID_USB_TEST_START_IND,
							NULL,
							MMI_MMI_SAP);
			msg_send_ext_queue(fmr_ilm);	
		}
		
		EINT5_flag=1;
		EINT_UnMask(9);
		return;
	}



}


//----------------抬手显示 中断---------------------------
#if 1
extern void EINT6_handle(void);

void EINT6_interrupt(void)
{

	EINT_Mask(10);
	if(EINT6_flag==1)
	{
		ilm_struct *fmr_ilm=NULL;

		EINT_SW_Debounce_Modify(10,5);
		EINT_Set_Polarity(10,KAL_FALSE);
		


		EINT6_flag=0;
		EINT_UnMask(10);

		//Uart1_sends("uart2_int2\r\n",strlen("uart2_int2\r\n"));
		
		return;
	}
	else
	{
		ilm_struct *fmr_ilm=NULL;

		EINT_SW_Debounce_Modify(10,5);
		EINT_Set_Polarity(10,KAL_TRUE);

		//EINT6_handle();
		DRV_SendPrimitive(fmr_ilm,
						MOD_EINT_HISR,
						MOD_YXAPP,//MOD_MMI,
						MSG_ID_USB_TEST_START_IND,
						NULL,
						MMI_MMI_SAP);
		msg_send_ext_queue(fmr_ilm);
		
		EINT6_flag=1;
		EINT_UnMask(10);
		return;
	}

}

#endif
//-------------------------------EINT----------------------------------

#endif
