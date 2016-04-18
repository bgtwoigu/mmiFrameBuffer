#ifndef __YX_BASIC_APP__
#define __YX_BASIC_APP__

#ifdef __MMI_BT_SUPPORT__
#include "BtcmSrvGprot.h"
#endif
#include "SmsSrvGprot.h"
#include "Unicodexdcl.h"
#include "OslMemory_Int.h"
#include "app2soc_struct.h"

#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------------------

#define UART_LOG   0  //0:关闭串口1打印信息  1打开串口1打印信息

//-------------------------------------------------

//-----------------------timer---------------------

#define LCD_TIMER     JMMS_ASYNC_SAVE_TIMER
#define WIFI_TIMER    JMMS_ASYNC_PARSE_TIMER
#define CALL_TIMER    JMMS_VIEW_PREVIEW_TIMER
#define I2C_TIMER     JMMS_NO_COMM_TIMER_CHECK
#define HAND_TIMER    JDD_TIMER_00

//-----------------------timer---------------------

//-----------------------uart----------------------

typedef enum
{
	UART_FUNCTION_NONE = 0,
	UART_FUNCTION_GPS,
	UART_FUNCTION_WIFI,
	UART_FUNCTION_BLE
} UART_FUNCTION;

typedef enum
{
	UART_STATUS_OFF = 0,
	UART_STATUS_ON
} UART_STATUS;

extern void Uart1_ini(void);
extern void Uart1_sends(U8 *buff,U16 len);
extern void Uart2_ini(unsigned long n);
extern void Uart2_open(unsigned long n);
extern void Uart2_close(void);
extern void Uart2_sends(U8 *buff,U16 len);

//-----------------------uart----------------------

//-----------------------display-------------------

typedef enum
{
	LCD_SWITCH_ONOFF_STATUS_OFF = 0,
	LCD_SWITCH_ONOFF_STATUS_ON
} LCD_SWITCH_ONOFF_STATUS;

//extern void lcd_dis_on(void);
//extern void lcd_dis_off(void);
//extern void dis_ini(void);

//-----------------------display-------------------

//-----------------------key-----------------------

//extern void key_ini(void);
//extern void key_re_ini(void);

//-----------------------key-----------------------

//-----------------------gps-----------------------

typedef enum
{
	GPS_SATELLITE_STATUS_NONE = 0,
	GPS_SATELLITE_STATUS_EXIST
} GPS_SATELLITE_STATUS;

typedef enum
{
	GPS_DATA_STATUS_NONE = 0,
	GPS_DATA_STATUS_OK
} GPS_DATA_STATUS;

extern unsigned char JW_value[10];

extern void gps_start(void);
extern void gps_end(void);
extern void gps_ready(void);
extern void gps_data_ready(void);
extern GPS_SATELLITE_STATUS gps_get_satellite_status_flag(void);

//-----------------------gps-----------------------

typedef struct _LatLng {
	double lat;
	double lng;
	double high;
} LatLng;

//-----------------------wifi----------------------

typedef enum
{
	WIFI_DATA_STATUS_NONE = 0,
	WIFI_DATA_STATUS_OK
} WIFI_DATA_STATUS;

extern unsigned char WIFI_mac_status ;
extern unsigned int WIFI_mac_all_num ;
extern unsigned char WIFI_mac1[7];//WIFI_mac1[0] : 信号强度，正数，大于零 数值越小，信号越强。
extern unsigned char WIFI_mac2[7];
extern unsigned char WIFI_mac3[7];
extern unsigned char WIFI_mac4[7];

extern void WIFI_start(void);
extern void WIFI_end(void);
extern WIFI_DATA_STATUS WIFI_get_status(void);
extern void WIFI_data_ready(void);

//-----------------------wifi----------------------

//-----------------------ble-----------------------

extern void ble_power_on(void);
extern void ble_power_off(void);

//-----------------------ble-----------------------

//-----------------------lbs-----------------------

typedef enum
{
	LBS_DATA_STATUS_NONE = 0,
	LBS_DATA_STATUS_OK
} LBS_DATA_STATUS;

extern void lbs_start(void);
extern void lbs_get_value(void);
LBS_DATA_STATUS lbs_get_data_status_flag(void);
	
//-----------------------lbs-----------------------

//-----------------------POWER---------------------

extern void power_manage_ini(void);
extern void VSIM2_ON(void);
extern void VSIM2_OFF(void);
extern void VCAMA_ON(void);
extern void VCAMA_OFF(void);
extern void VUSB_ON(void);
extern void VUSB_OFF(void);
extern void VMC_ON(void);
extern void VMC_OFF(void);
extern void VIBR_ON(void);
extern void VIBR_OFF(void);

//-----------------------POWER---------------------

//-----------------------CALL----------------------

typedef enum
{
	CALL_IN_STATUS_END = 0,//没有接入电话
	CALL_IN_STATUS_INCOMING,//电话接入  铃声响  未接通
	CALL_IN_STATUS_IN// 正在通话中
} CALL_IN_STATUS;

typedef enum
{
	CALL_OUT_STATUS_END = 0,
	CALL_OUT_STATUS_START
} CALL_OUT_STATUS;

typedef enum
{
	CALL_IN_ALLOW_CONNECT_ALLOWED = 0,
	CALL_IN_ALLOW_CONNECT_UNALLOWED
} CALL_IN_ALLOW_CONNECT;

typedef enum
{
	CALL_OUT_ALLOW_DISCONNECT_ALLOWED = 0,
	CALL_OUT_ALLOW_DISCONNECT_UNALLOWED
} CALL_OUT_ALLOW_DISCONNECT;

typedef enum
{
	CALL_LISTEN_STATUS_OFF = 0,
	CALL_LISTEN_STATUS_ON
} CALL_LISTEN_STATUS;

extern CALL_LISTEN_STATUS call_listen_status_flag;

extern void CALL_incoming(void);
extern void CALL_in(void);
extern void CALL_out(void);
extern void CALL_end(void);
extern void CALL_incoming_connect(void);
extern void CALL_incoming_disconnect(void);
extern void CALL_in_disconnect(void);
extern void CALL_outgoing_disconnect(void);
extern CALL_IN_STATUS CALL_get_call_in_status_flag(void);
extern CALL_OUT_STATUS CALL_get_call_out_status_flag(void);
extern CALL_IN_ALLOW_CONNECT CALL_get_call_in_allow_connect_flag(void);
extern CALL_OUT_ALLOW_DISCONNECT CALL_get_call_out_allow_disconnect(void);
extern void CALL_listen_on(void);
extern void CALL_listen_off(void);
extern CALL_LISTEN_STATUS CALL_check_listen(void);
extern void CALL_listen_start(void);

//-----------------------CALL----------------------

//---------------------------远程关机------------------------------

extern void Shutdown_cmd(void);

//---------------------------远程关机------------------------------

//---------------------------找手表--------------------------------

typedef enum
{
	SEARCH_WATCH_STATUS_START = 0,
	SEARCH_WATCH_STATUS_END
} SEARCH_WATCH_STATUS;

extern SEARCH_WATCH_STATUS search_watch_status_flag;
extern void start_search_watch(void);
extern void stop_search_watch(void);

//---------------------------找手表--------------------------------

//---------------------------查话费--------------------------------

extern unsigned char iphone_number_for_sms[20];
extern void SmsSend_to_phone(U16 *p);
extern void Telephone_Bill_Enquiry(unsigned char *phone_num);

//---------------------------查话费--------------------------------

//---------------------------拒绝陌生人来电------------------------

typedef enum
{
	REFUSED_TO_STRANGER_CALLS_STATUS_OFF = 0,
	REFUSED_TO_STRANGER_CALLS_STATUS_ON 
} REFUSED_TO_STRANGER_CALLS_STATUS;

extern REFUSED_TO_STRANGER_CALLS_STATUS refused_to_stranger_calls_status_flag;

//拒绝陌生人来电
extern void CALL_refused_to_stranger_calls_on(void);
//允许陌生人来电
extern void CALL_refused_to_stranger_calls_off(void);

//---------------------------拒绝陌生人来电------------------------

//---------------------------来电自动接听--------------------------

typedef enum
{
	CALL_AUTOMATIC_ANSWERING_STATUS_OFF = 0,
	CALL_AUTOMATIC_ANSWERING_STATUS_ON
} CALL_AUTOMATIC_ANSWERING_STATUS;

extern CALL_AUTOMATIC_ANSWERING_STATUS call_automatic_answering_status_flag;

extern void CALL_automatic_answering_status_on(void);
extern void CALL_automatic_answering_status_off(void);

//---------------------------来电自动接听--------------------------

//---------------------------静音模式------------------------------

typedef enum
{
	MUTE_MODE_STATUS_OFF = 0,
	MUTE_MODE_STATUS_ON
} MUTE_MODE_STATUS;

extern void mute_mode_open(void);
extern void mute_mode_close(void);
extern MUTE_MODE_STATUS mute_mode_get_status(void);

//---------------------------静音模式------------------------------

//---------------------------I2C-----------------------------------

typedef enum
{
	APDS9901_STATUS_NOT_LOOSE = 0,
	APDS9901_STATUS_LOOSE,
	APDS9901_STATUS_ERROR
} APDS9901_STATUS;

//extern unsigned int Step_num;

extern void I2C_initialization(void);
extern void I2C_APDS9901_ini(unsigned char step);
extern void I2C_APDS9901_read_data(void);
extern void  I2C_APDS9901_START(void);
extern void I2C_APDS9901_TASK1(void);
extern void I2C_APDS9901_TASK2(void);
extern void I2C_APDS9901_TASK3(void);
extern APDS9901_STATUS I2C_APDS9901_GET_STATUS(void);	

extern void BMA250E_initialization(void);
extern void BMA250E_READ_DATA(kal_int16 *X, kal_int16 *Y, kal_int16 *Z, unsigned char *p);
extern void BMA250E_task_ini(void);

//---------------------------I2C-----------------------------------

//---------------------------摇一摇接通----------------------------

extern void yaoyiyao_ini(void);
extern void yaoyiyao_open(void);
extern void yaoyiyao_close(void);

//---------------------------摇一摇接通----------------------------

//---------------------------抬手显示------------------------------

typedef enum
{
	FLAT_STATUS_OFF =0,
	FLAT_STATUS_ON
} FLAT_STATUS;

extern FLAT_STATUS flat_status_new;
extern FLAT_STATUS flat_status_old;

typedef enum
{
	HAND_STATUS_NO_CHANGE = 0,
	HAND_STATUS_CHANGE
} HAND_STATUS;

typedef enum
{
	HAND_SCREEN_STATUS_OFF = 0,
	HAND_SCREEN_STATUS_ON
} HAND_SCREEN_STATUS;

extern HAND_STATUS hand_status_flag;
extern HAND_SCREEN_STATUS hand_screen_status_flag;
extern FLAT_STATUS flat_status_tep;
extern void hand_mode_ini(void);
extern void hand_mode_open(void);
extern void hand_mode_close(void);

extern unsigned char flag_hand;
//---------------------------抬手显示------------------------------

//---------------------------motor---------------------------------

extern void motor_en(void);
extern void motor_disen(void);

//---------------------------motor---------------------------------



/***************************************************具体根据你们的需要修改相关宏**************************************************/
//debug
#define  YX_IS_TEST_VERSION          0               //是否是测试版本,0:量产版本,1:串口1调试并打印输出,2:USB调试并打印输出,USB打印时
                                                     //电脑端工具要用catcher,同时代码里custom\common\hal\nvram\nvram_data_items.c
                                                     //中static port_setting_struct const NVRAM_EF_PORT_SETTING_DEFAULT[]数组里,tst-ps uses uart_port_null(value is 99),99改为4即可
                                                     //在catcher的FILLTER里选只一个MOD即MOD_BT就可以看到我们的打印输出信息

//GPS串口配置
#define  YXAPP_USE_UART              uart_port2      //配置你要用的串口号,默认为uart2
#define  YX_GPS_UART_BAUDRATE        UART_BAUD_115200  //GPS默认的波特率,支持MT3337,UBOX-7020与U8,以及采用NMEA0183协议的GPS芯片,UART_BAUD_9600,UART_BAUD_115200,(MT3337:115200,ublox:9600)
#define  YXAPP_UART_TASK_OWNER_WAY   0               //配置你的串口属于那个TASK,0:MOD_YXAPP, 1:MOD_MMI
#define  YXAPP_GPS_KIND              0               //你的GPS芯片是否支持三种系统,如果只支持GPS请用0,如果三(GP,BD,GL)种都支持,就用3,3:GN多模定位,2:GL,1:只支持BD,0:只支持GPS
//注意GPS还有一个电源控制脚在驱动中,需要根据原理图修改一下,给它供电.API名称为:YXAppGpsPowerCtrl

//服务器主机与端口,目前只支持TCP方式,HTTP即是TCP方式
#define  YX_NET_WAY                  MAPN_NET        //MAPN_NET,MAPN_WAP,上网方式,NET与WAP两种,区别WAP方式上网是通过移动或联通网关
	                                                 //10.0.0.172代理上网的,此方式只能连HTTP服务器,并且端口一定要为80,否则网关不会去联
                                                     //NET方式就不是通过代理上,是直接可以连接服务器的,没有端口限制
#define  YX_HOST_NAME_MAX_LEN        40              //域名最大字符数

#if 1//需要更改成你们的服务器IP与端口
#define  YX_DOMAIN_NAME_DEFAULT      "112.93.116.189" //服务器域名或是ip地址,如:192.186.1.1
#define  YX_SERVER_PORT              8880            //服务器port,如果是HTTP服务器,端口默认为80,如果是通过CMWAP方式上网,则只能用HTTP服务器,并且端口一定要为80
#define  APOLLO_BURNKEY_SERVER_IP		"120.76.25.198"
#define  APOLLO_BURNKEY_SERVER_PORT	 5555
#else
#define  YX_DOMAIN_NAME_DEFAULT      "www.163.com" //服务器域名或是ip地址,如:192.186.1.1
#define  YX_SERVER_PORT              80            //服务器port,如果是HTTP服务器,端口默认为80,如果是通过CMWAP方式上网,则只能用HTTP服务器,并且端口一定要为80
#endif
#define  YX_CONN_SERVER_TIME_OUT     50000           //SOCKET read write time out,second
#define  YX_MIN_UPLOAD_TIME          1               //心跳包时间间隔,秒为单位,默认为15秒,系统秒的计时是以每15秒一个TICK
#define  YX_HEAT_TIME_MIN            1               //最小心跳包间隔
#define  YX_HEAT_TIME_MAX            10              //最大心跳包间隔
#define  YX_GPS_UPLOAD_TIME          20              //定位包上传间隔,分钟为单位,默认为20分钟
#define  YX_GPS_TIME_MIN             5               //定位包最小时间间隔
#define  YX_GPS_TIME_MAX             60              //定位包最大时间间隔
#define  YX_GPRS_RECONNECT_COUNT     3               //当与服务器通讯中断或没连接到服务器时，最大重新连接次数
#define  YX_GPS_CONVERT_DM2DD        1               //需要不需要把GPS的坐标DDMMMMMM转成DDDDDD,1:需要,0:不需要

#define  YX_GPS_DISPLAY_UI           0               //是否显示GPS图标,0:不显示
#define  YX_GPS_USE_AGPS             1               //是否支持AGPS,0:不支持
#define  YX_GSENSOR_SURPPORT         2               //GSENSOR,1:采用中断方式,2:TIMER方式,0:不支持
#define  YX_PROTOCOL_KIND            1               //通讯协议种类,
#define  YX_NO_SIMCARD_GET_DATA      1               //没有插入SIM卡时,同样采集数据并保存起来,等有卡时,再上传到服务器,1:支持,0:不支持
#define  YX_SMS_CMD_DONT_USE         1               //不需要短信指令
#define  YX_SECOND_TICK_WAY          2               //秒的计时方式,1:为stack_timer,2:starttimer
#if(YX_GPS_USE_AGPS==1)
#define  YX_GPS_SAMPLE_DATA_TIME     30              //GPS采集坐标最长时间,30秒
#else
#define  YX_GPS_SAMPLE_DATA_TIME     50              //GPS采集坐标最长时间,50秒
#endif
#define  YX_HEART_TICK_UNIT          5000           /**心跳包每10秒计一次单位*/
/***************************************************以下部分的宏定义不需要去改动**************************************************/

#define  YX_APP_MAX_CELL_INFOR_NUM   6               //最大基站数,MTK最大为6,不要更改
#define  YX_APP_MAX_SMS_CHARS        70              //一条短信最大字符数,不要更改
#define  YX_SOCK_BUFFER_LEN          2048            //从SOCKET中一次读取或发送的最大字节数
#define  YX_SOCK_READ_BUFFER_LEN     2048            

#define  YX_FILE_FOLD_NAME           "yxdat"         //根目录名称
#define  YX_EFENCE_MAX_NUM           3              //最大电子围栏个数
#define  YX_EFENCE_NAME_LENGTH       10             //最大电子围栏名称长度,(unicode)
#define  YX_MAX_MONITOR_NUM          4              //最大监护人名单,00:dady,1:monmy
#define  YX_WHITE_NUM_LIST_MAX       10             //白名单列表最大个数
#define  YX_LOGS_MAX_NUM             (YX_EFENCE_MAX_NUM+YX_MAX_MONITOR_NUM+2+2)
#define  YX_BT_MAC_BUFFER_LEN        18             //BT mac字符BUFFER长度,包括了结尾0
#define  YX_MAX_ALARM_NUM            3              //最大闹钟数
#define  YX_BIG_MEMERY_LEN           30*1024        //最大可收发网络数据的内存
#define  YX_UNSEND_PKG_HEADER_LEN    28             //过去包包头固定大小
#define  YX_UNSEND_PKG_FILE_LEN      (YX_BIG_MEMERY_LEN-YX_UNSEND_PKG_HEADER_LEN)      //最大未上传包文件大小
#if (YX_GPS_USE_AGPS==1)
#define  AGPS_MAX_RECEIVE_BUFFER_LEN   4*1204       //AGPS文件大小
#endif

#define  YX_APN_USE_NEW_ONE
#define  YX_APP_DNS_REQUEST_ID		0x055A5121

#define  YX_APP_SIM1          1
#define  YX_APP_SIM2          2

#define  MAPN_WAP             0
#define  MAPN_NET             1
#define  MAPN_WIFI            2

#define MSIM_OPR_NONE         0
#define MSIM_OPR_CMCC         1
#define MSIM_OPR_UNICOM       2
#define MSIM_OPR_TELCOM       3
#define MSIM_OPR_WIFI         4
#define MSIM_OPR_UNKOWN       5

#define PF_INET                      SOC_PF_INET 
#define SOCK_STREAM                  SOC_SOCK_STREAM

#define ENABLE			1
#define DISABLE			0
#define ENABLE_MMI_FRAMEBUFFER		ENABLE
#define ENABLE_MMI_KEYPAD			ENABLE
#define ENABLE_MMI_WATCHCONF		ENABLE
#define ENABLE_MMI_INITTIME			ENABLE
#define ENABLE_MMI_APOLLOCLOCK		ENABLE

#define APOLLO_ODM_AOXIN_OLDMAN_WATCH			0x00
#define APOLLO_ODM_AOXIN_SMART_WATCH			0x01
#define APOLLO_ODM_LAOYING_POSITION				0x02
#define APOLLO_SMART_WATCH						0xA0
#define APOLLO_PROTOCOL_VERSION		APOLLO_ODM_AOXIN_OLDMAN_WATCH

//msg command kind    
#define  YX_MSG_BLUETOOTH_CTRL       0xB1
#define  YX_MSG_WLAN_POWER_CTRL      0xB2 //1:power on and scan,0:power off
#define  YX_MSG_LBS_CELL_CTRL        0xB3 //1:get cell infors,0:cancel
#define  YX_MSG_CALL_CTRL            0xB4 //1:make call,0 cancel
#define  YX_MSG_SMS_CTRL             0xB5
#define  YX_MSG_GPRS_CTRL            0xB6
#define  YX_MSG_MAINAPP_TICK_CTRL    0xB7 //分钟TICK
#define  YX_MSG_MAINAPP_SECOND_CTRL  0xB8 //秒钟TICK
#define  YX_MSG_START_POS_CTRL       0xB9 //立即定位

#ifndef APOLLO_KEY //Update By WangBoJing 20150915
#define APOLLO_KEY



#define APOLLO_UPKEY_EVENT_SINGLE_UP			0x0A
#define APOLLO_UPKEY_EVENT_SINGLE_DOWN		0x0B
#define APOLLO_UPKEY_EVENT_REPEAT_UP		0x0C
#define APOLLO_UPKEY_EVENT_REPEAT_DOWN		0x0D
#define APOLLO_PLAY_RECV_VOICE_DATA			0x0E

#define APOLLO_UART1							0x1A
#define APOLLO_UART2							0x1B
#define APOLLO_UART3							0x1C

#define APOLLO_MSG_BLUETOOTH_POWERDOWN 		0x01
#define APOLLO_MSG_BLUETOOTH_SETUP  			0x02
#define APOLLO_MSG_WATCH_POWERDOWN 			0x03
#define APOLLO_MSG_TELEPHONEFARE_SELECT  	0x04
#define APOLLO_MSG_REALTIME_GPS_LOCATION 	0x05

#define APOLLO_MSG_SEATCH_WATCH 				0x06
#define APOLLO_MSG_ELECTIRC_FENCE			0x07
#define APOLLO_MSG_REFUSE_CALL_SETON			0x08
#define APOLLO_MSG_REFUSE_CALL_SETOFF  		0x09


#define APOLLO_MSG_STOP_RECORDER			APOLLO_UPKEY_EVENT_REPEAT_UP
#define APOLLO_MSG_START_RECORDER			APOLLO_UPKEY_EVENT_REPEAT_DOWN

#define APOLLO_MSG_AUTO_CONNECT_SETON 		0x10
#define APOLLO_MSG_AUTO_CONNECT_SETOFF 		0x11
#define APOLLO_MSG_SHAKE_CONNECT_SETON 		0x12
#define APOLLO_MSG_SHAKE_CONNECT_SETOFF	 	0x13
#define APOLLO_MSG_HANDON_RECOGNITION_SETON  0x14
#define APOLLO_MSG_HANDON_RECOGNITION_SETOFF 0x15

#define APOLLO_MSG_MUTE_SETON				0x16
#define APOLLO_MSG_MUTE_SETOFF				0x17

#define APOLLO_MSG_LISTEN_MODE				0x18



	
#define APOLLO_MSG_LANGUAGE_CHINESE 	 		0x20
#define APOLLO_MSG_LANGUAGE_ENGLISH 			0x21
#define APOLLO_MSG_LANGUAGE_RESSION 			0x22

#define APOLLO_MSG_TIMEZONE_1 				0x41
#define APOLLO_MSG_TIMEZONE_2	 			0x42

#define APOLLO_MSG_LED_ON						0x58

/* ** **** ********  ****************  MESSAGE  ****************  ******** **** ** */

#define APOLLO_MSG_REALTIME_POS				0x01
#define APOLLO_MSG_VOICE_RECV				0x02
#define APOLLO_MSG_COUNT_STEP_SETON			0x03
#define APOLLO_MSG_COUNT_STEP_SETOFF		0x04
#define APOLLO_MSG_SLEEP_QUALITY_SETON		0x05
#define APOLLO_MSG_SLEEP_QUALITY_SETOFF		0x06
#define APOLLO_MSG_SET_PHONENUMBER			0x07
#define APOLLO_MSG_CLOCKALARM				0x08
#define APOLLO_MSG_SAFEZONE_ALARM			0x09
#define APOLLO_MSG_LONGSAT_ALARM			0x0A
#define APOLLO_MSG_LOWPOWER_ALARM			0x0B

#define APOLLO_MSG_FAMILYNUMBER_SET			0x41
#define APOLLO_MSG_CONTRACT_SET				0x42
#define APOLLO_MSG_PHONEBOOK_SET			0x43
#define APOLLO_MSG_FREQ_SET					0x44

#define APOLLO_MSG_SOS_MSGREMAINER_OPEN		0x51
#define APOLLO_MSG_SOS_MSGREMAINER_CLOSE	0x52
#define APOLLO_MSG_POWER_MSGREMAINDER_OPEN	0x53
#define APOLLO_MSG_POWER_MSGREMAINDER_CLOSE 0x54

#define APOLLO_MSG_ALARMMUSIC_OPEN			0x61
#define APOLLO_MSG_ALARMMUSIC_CLOSE			0x62
#define APOLLO_MSG_SAFEZONEMUSIC_OPEN		0x63
#define APOLLO_MSG_SAFEZONEMUSIC_CLOSE		0x64

#define APOLLO_MSG_SOS_CLEAR				0x21
#define APOLLO_MSG_PHONEFARE_SELECT			0x2A


#define APOLLO_MSG_BLUETOOTH_UPLOAD			0x91
#define APOLLO_MSG_BLUETOOTH_DOWNLOAD 		0x92
#define APOLLO_MSG_SERIALPORT_SEND			0x93
#define APOLLO_MSG_SERIALPORT_RECV			0x94

#define APOLLO_BURN_KEY_MSG					0xA0
#define APOLLO_MSG_FALLDOWN					0xA1
#define APOLLO_MSG_LED_REFLASH				0xA2
#define APOLLO_MSG_BLUETOOTH_MATCH			0xA3

#define APOLLO_MSG_BUZZER_DEVICE_OPEN		0x81
#define APOLLO_MSG_BUZZER_DEVICE_CLOSE		0x82
#define APOLLO_MSG_LIGHT1_DEVICE_OPEN		0x83
#define APOLLO_MSG_LIGHT1_DEVICE_CLOSE		0x84
#define APOLLO_MSG_LIGHT2_DEVICE_OPEN		0x85
#define APOLLO_MSG_LIGHT2_DEVICE_CLOSE		0x86

/* ** **** ********  ****************  INDEX  ****************  ******** **** ** */

#define IDX_PHONE_INDEX						4
#define IDX_PHONE_NUMBER					5


#define ITEM_SIZE		64

#define FAMILY_NUMBER_SIZE		2
#define CONTACT_NUMBER_SIZE		10
#define PHONE_BOOK_SIZE			10
#define PHONE_NUMBER_LENGTH		20

#define  APOLLO_WATCH_CONF_SIZE		2*1024
#define  APOLLO_WATCH_CONF_FILE		L"\\watch.conf"
#define  APOLLO_SERVER_IP_FILE		L"\\server.conf"



#endif

//msg user data kind
//bt operation
#define  YX_DATA_BTSETNAME            1
#define  YX_DATA_BTSEARCH             2
#define  YX_DATA_BT_ABORT_SEARCH      3
#define  YX_DATA_BT_POWER_CTRL        4
#define  YX_DATA_BT_SET_VISIBILITY    5
//lbs operation
#define  YX_DATA_LBS_START            1
#define  YX_DATA_LBS_CANECL           0
//call operation
#define  YX_DATA_MAKE_CALL            1
#define  YX_DATA_REFUSE_INCALL        0
//sms operation
#define  YX_DATA_SEND_SMS             1
//gprs operation
#define  YX_DATA_GPRS_RECONNECT       1
//call type
#define  YX_CALL_NORMAL               0
#define  YX_CALL_LISTEN               1
#define  YX_CALL_CIRCYLE              2
#define  YX_CALL_SEND1                3
#define  YX_CALL_SEND2                4
//run kind
#define  YX_RUNKIND_WIFI         0x0001
#define  YX_RUNKIND_LBS          0x0002
#define  YX_RUNKIND_GPS          0x0004
#define  YX_RUNKIND_GSENSOR      0x0008
#define  YX_RUNKIND_BT_SEARCH    0x0010
#define  YX_RUNKIND_OWNER_GPS    0x8000  //period upload data by timer
#define  YX_RUNKIND_OWNER_HEART  0x4000  //心跳包
//max 
#define APOLLO_RUNKIND_HEART_COMPLETE 	0x2000

#define APOLLO_RUNKIND_VOICE_UPLOAD		0x0020
#define APOLLO_RUNKIND_VOICE_DOWNLOAD	0x0040
#define APOLLO_RUNKIND_INIT_WATCHDATA	0x0080
#define APOLLO_RUNKIND_AGPS_DOWNLOAD	0x0100


#define  YX_APP_CALL_NUMBER_MAX_LENGTH   20
#define  YX_APP_CALL_NAMES_MAX_LENGTH    10

#define  YX_CHECK_MONITOR_INFOR(a,n) (!a || !a||(strlen((S8*)a)<5)||(strlen((S8*)a)>YX_APP_CALL_NUMBER_MAX_LENGTH)||(n[0]==0&&n[1]==0))

#ifdef __MMI_WLAN_FEATURES__
typedef struct
{
	kal_uint8               bssid[WNDRV_MAC_ADDRESS_LEN]; /* MAC address */
    kal_uint8               ssid[WNDRV_SSID_MAX_LEN+1]; 
	kal_int32               rssi;
}WLANMACPARAM;
#endif
#ifndef SRV_BT_CM_BD_FNAME_LEN
#define SRV_BT_CM_BD_FNAME_LEN 100
#endif
#ifdef __MMI_BT_SUPPORT__
#define  YXAPP_BT_MAX_SEARCH_DEVICES  8

typedef struct
{
	U8     name[SRV_BT_CM_BD_FNAME_LEN+1];
    U32    lap;    /* Lower Address Part 00..23 */
    U8     uap;    /* upper Address Part 24..31 */
    U16    nap;    /* Non-significant    32..47 */
}YXBTPARAM;
#endif

#ifdef __NBR_CELL_INFO__
typedef struct{
  //  U16    arfcn;           /*ARFCN*/
 //   U8     bsic;            /*BSIC*/
    U8     rxlev;           /*Received signal level*/
    U16    mcc;             /*MCC 国家代码 460*/
    U16    mnc;             /*MNC 运营商代码*/
    U16    lac;             /*LAC 位置码*/
    U16    ci;              /*CI 小区号*/
}yxapp_cell_info_struct;
typedef struct{
	char   isRunning;
	U8     cellNum;
	yxapp_cell_info_struct cellInfor[YX_APP_MAX_CELL_INFOR_NUM+1];//0:是当前所在小区的信息,后面6个为找到的小区
}YXAPPCELLPARAM;
#endif
typedef struct{
	S8     number[SRV_SMS_MAX_ADDR_LEN+1];
	S8     storeNumber[SRV_SMS_MAX_ADDR_LEN+1];
	U16    content[YX_APP_MAX_SMS_CHARS+1];
}YXAPPSMSPARAM;

#define    YX_NET_STATUS_CONN_HOST_BY_NAME        1
#define    YX_NET_STATUS_CONN_CONNECTTING         2
#define    YX_NET_STATUS_CONN_CONNECTED           3
#define    YX_NET_STATUS_CONN_STOP                4
#define    YX_NET_STATUS_CONN_TIME_OUT            5
typedef struct
{
    kal_uint8 address[4];   /* resolved IP address for queried domain name */
}YXSOCDNSASTRUCT;

typedef struct 
{
    kal_int8   sock;                    /**< socket 句柄*/
	S8         apn;
	U8         appid;
    U16        port;                    /**< socket 远程端口*/
    U8         socket_state;            /**< socket 链路的状态*/
    U32        account_id;
	S8         hostName[YX_HOST_NAME_MAX_LEN+1];               //保存的域名或IP
	soc_dns_a_struct  dnsIplist;
}YXAPPSOCKCONTEXT;

typedef struct
{
	U8         sendCount;              /*最大重发次数,为FF时,不需要重发*/
	U8         sendBuf[YX_SOCK_BUFFER_LEN];
	U8         readBuffer[YX_SOCK_READ_BUFFER_LEN];
	S32 		readLen;
	S32 		readIndex;
	S32        sendLen;
	S32        curSendLen;
}YXAPPSOCKDATAPARAM;

typedef struct
{
	S8         number[YX_APP_CALL_NUMBER_MAX_LENGTH+1];//ascii
	U8         names[(YX_APP_CALL_NAMES_MAX_LENGTH+1)<<1];//unicode
}YXMONITORPARAM;

#define     YX_GPS_KIND_GP   0
#define     YX_GPS_KIND_BD   1
#define     YX_GPS_KIND_GL   2
#define     YX_GPS_KIND_GN   3

#define     YX_GPS_FLAG_LATITUDE     0x01
#define     YX_GPS_FLAG_LONGITITUDE  0x02
#define     YX_GPS_FLAG_SPEED        0x04
#define     YX_GPS_FLAG_DATE         0x08
#define     YX_GPS_FLAG_TIME         0x10
#define     YX_GPS_FLAG_INVALID_DATE 0x20

#define     YX_DATA_UPDATE_BD        0x01
#define     YX_DATA_UPDATE_GP        0x02
#define     YX_DATA_UPDATE_GL        0x04
#define     YX_DATA_UPDATE_GN        0x08

#define     YX_CAN_READ_DATA         0x02
#define     YX_GETTING_SAMPLE_DATA   0x04

#define     YX_GPS_GSA_POS_FLAG      0x06

//设备开启与关闭状态
#define     YX_DEVICE_BT_ON_FLAG     0x01
#define     YX_DEVICE_WIFI_ON_FLAG   0x02

typedef struct
{
	char    isopen;
	U8      gpshour[4];
	U8      gpsminute[4];
	U8      gpssecond[4];
	U8      gpsyear[4];
	U8      gpsmonth[4];
	U8      gpsday[4];
	U8      gpsKind;         //所选用的GPS种类,0:BD,1GP,2:GN
	U8      updateKind;
	U8      gpstatus[4];    //卫星状态,GSA相关值,低3位是定位种类0:自动,1手动,定位类型,1未定位,2二维定位,3三维 高5位:用到的卫星数最大12颗
#if (YX_GPS_USE_AGPS==1)
	U8      yxAgpsDataBuf[AGPS_MAX_RECEIVE_BUFFER_LEN];
	U16     yxAgpsDatLen;
#endif
	U8      gpsTimerVal;     //GPS最大采集数据时间,YX_GPS_SAMPLE_DATA_TIME
	U8      yxGpsStartTick;
	U16     gpsCircle;      //GPS工作周期
	double  longitudev[4];   //经度,负数为西,0:BD,1:GP,2:GN
	double  latitudev[4];   //纬度,负数为南,0:BD,1:GP,2:GN
	double  hightv[4];       //高度,单位为米
	double  gpsangle[4];    //地面方位角真北参系,0:BD,1:GP,2:GN
	double  gpsrspeed[4];   //速度公里,0:BD,1:GP,2:GN
	double  gpsanglem[4];    //地面方位角磁北参系,0:BD,1:GP,2:GN
	double  gpsrspeedn[4];   //速度海里,0:BD,1:GP,2:GN
}YXGPSPARAMSTR;

typedef struct
{
	U8      lowBattery;    //低电量报警门槛,值只取低7位,第8位为是否发送了短信
	U8      gpsSetKind;    //所选定位系统
	//U8      lastBt[15];    //要找的BT设备地址,要转成16进制ASCII
	U8      imei[16];      //15 imei
	U8      deviceFlag;    //设备开启与关闭状态
	U8      allowOff;      //是否允许关机,1:ok,0:no
	U16     uploadTick;    //上传数据间隔时间,分钟为单位
	U16     gpsTimes;      //GPS工作周期
	U16     systemTick;    //系统秒
	U16     minuteTick;    //系统分
}YXSYSTEMPARAM;

#define  YX_UPLOAD_KIND_HEART        0x01  //心跳包
#define  YX_UPLOAD_KIND_GPS          0x02  //定位包
#define YX_UPLOAD_KIND_VOICE		0x03
#define YX_DOWNLOAD_KIND_VOICE	0x04

typedef struct
{
	char    sleepdisable;
	char    uploadStart;
	U8      sleephandle;
	U8      allIsReady;
	U8      heartTick;     //心跳包计时
	U8      gpsTick;       //定位包计时
}YXAPPPARAM;

#if(YX_GSENSOR_SURPPORT!=0)
typedef struct
{
#if(YX_GSENSOR_SURPPORT==2)
	kal_timerid timerId;
	kal_uint8   sample_period;
#else
	kal_uint8   isReady;
#endif
}YXGSENSORPARAM;
#endif
#if (YX_PROTOCOL_KIND==1)//IK protocol
typedef struct
{
	kal_uint8  sendOk;//0:no,1:ok
	kal_uint8  *bigBuffer;
	kal_uint8  isSos;
	kal_uint8  isFirst; /**是否为开机的第一次传,1:是*/
	kal_int32  sendLen;
	kal_int32  dataLen;
	kal_int32  totalLen;
}YXPROTOCOLPARAM;
#endif
//make call
typedef struct
{
	S8         number[YX_APP_CALL_NUMBER_MAX_LENGTH+1]; //用于监听与回拨电话
	kal_uint8  callKind;//0:normal call,1:listen call,2:circle call
	kal_uint8  callIndex;
	YXMONITORPARAM yxMonitors[YX_MAX_MONITOR_NUM];
}YXCALLPARAM;
//firewall
typedef struct
{
	kal_uint8  status;//on/off,0:off,1:on
	kal_uint16 startTime[3];//hour|min
	kal_uint16 endTime[3];//hour|min
	YXMONITORPARAM yxWhiteList[YX_WHITE_NUM_LIST_MAX];
}YXFIREWALLPARAM;
//efence
typedef struct
{
	kal_uint8  names[(YX_EFENCE_NAME_LENGTH+1)<<1];
	kal_uint8  kind;//0:out,1:in,2:both
	kal_uint8  lastKind;
	double     log;
	double     lat;
	double     radius;//meter
}YXEFENCEPARAM;
//logs
#define     LOG_EFENCE     '1'  //此LOG会有多个,action:'0':out,'1':in
#define     LOG_LOWBAT     '2'  //此LOG只有一个,以最新的一个为准
#define     LOG_PHONE      '3'  //开关机,action:'1':开,'0':关,此LOG只有一个,以最新的一个为准
#define     LOG_CALL       '4'  //电话记录,此LOG只有一个,以最新的一个为准,ACT:0 out call,1:in call
#define     LOG_OFFHANDS   '5'  //防脱报警,action:'0'取下,'1'戴上

typedef struct
{
	kal_int8     kind;//0:no,1:efence,2:lowbattery,3:listennig,4:mobile
	kal_uint16   date;//month|day
	kal_uint16   time;//hour|min
	kal_uint8    action;
	kal_uint16   value;//efence的序号,or,低电量的值or语音监听的号码值,固定为4位,不足补0
}YXLOGPARAM;


typedef struct _WATCHCONF {
	char u8StepFlag;
	char u8SleepFlag;
	char u8SOSMsgFlag;
	char u8PowerMsgFlag;
	char u8AlarmMusicFlag;
	char u8SafeZoneMusic;
	char u8Freq;
	char u8FamilyNumber[FAMILY_NUMBER_SIZE][PHONE_NUMBER_LENGTH];
	char u8ContactNumber[CONTACT_NUMBER_SIZE][PHONE_NUMBER_LENGTH];
	char u8PhoneBook[PHONE_BOOK_SIZE][PHONE_NUMBER_LENGTH];
} WatchConf;



#define isPrime(year) ((year%4==0&&year%100!=0)||(year%400==0))  
struct SampleDate  {  
    int year;  
    int month;  
    int day;  
}; 



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//basic api
extern void *YxAppMemMalloc(kal_int32 size);
extern void YxAppMemFree(void *pptr);
extern void YxAppSendMsgToMMIMod(U8 command,U8 param1,U8 param2);
extern void YxAppDisableSleep(void);
extern void YxAppEnableSleep(void);
extern char YxAppGetSimOperator(char simId);
extern void YxAppGetImeiNumber(void);
extern U8 YxAppGetSignalLevelInPercent(void);
extern U8 YxAppBatteryLevel(void);
extern void YxAppGetSystemDateTimeStr(char *strBuf);//buf len need 20
extern S32 YxAppUCSStrlen(U16 *string);
extern void YxAppMMiRegisterMsg(void);

//gps uart api
extern U16 YxAppUartReadData(U8 *buff,U16 Maxlen);
extern U16 YxAppUartSendData(U8 *buff,U16 len);
extern void YxAppUartTxFlush(void);
extern void YxAppUartRxFlush(void);
#if(YX_IS_TEST_VERSION!=0)
extern void YxAppTestUartSendData(U8 *dataBuf,U16 dataLen);
#endif
extern void YxAppGpsUartOpen(void);
extern void YxAppGpsUartClose(void);
extern void YXAppGpsPowerCtrl(char on);//driver power ctrl,1,power on,0,power off
#if(YXAPP_UART_TASK_OWNER_WAY==1)
extern void YxAppRegisterUartMsgToMMiTask(void);//if gps uart is uart1,need use this
#endif

#ifdef __MMI_WLAN_FEATURES__
extern void YxAppWlanPowerScan(char scan);
extern WLANMACPARAM *YxAppGetWlanScanInfor(U8 index);
extern U8 *YxAppSetAndGetLocalMac(U8 *macBuf);
#endif

//gsensor
#if(YX_GSENSOR_SURPPORT!=0)
extern char YxAppGsensorSampleData(kal_int16 *x_adc, kal_int16 *y_adc, kal_int16 *z_adc);//in driver
extern void YxAppGsensorInition(void);//in driver
//mmi
extern void YxAppGsensorProcInit(void);
#if(YX_GSENSOR_SURPPORT==2)
extern void YxAppGsensorSample(kal_bool enable);
#endif
#endif

#ifdef __MMI_BT_SUPPORT__
extern void YxAppBTPowerControl(char on);
extern char YxAppGetBtStatus(void);
extern char YxAppGetLocalBtMac(U8 *nameBuf,U8 *addressBuf);
extern void YxAppSetBtDeviceName(char *nameBuf);
extern void YxAppGetLocalBtNameMac(PU8 *name,PU8 *mac);
extern void YxAppBtSearchDevices(void);
extern void YxAppBtAbortSearch(void);
extern void YxAppBtSetNotify(U32 event_mask);
extern void YxAppBtSetVisibility(char on);
extern char YxAppBtGetDevicesAddress(U8 index,char *addressStr);//buffer is 15
#endif
#ifdef __NBR_CELL_INFO__
extern void YxAppCellRegReq(void);
extern void YxAppCellDeregReq(void);
extern yxapp_cell_info_struct *YxAppLbsGetData(char idx);
extern U8 YxAppGetLBSInfor(char *lineBuffer,char idx);//lineBuffer:min length is 25
#endif

//call apis
extern void YxAppCircyleCallInit(void);
extern void YxAppCallStopCircyle(void);
extern S8 YxAppMakeWithCallType(S8 callType,S8 *number);
//extern void YxAppEndIncommingCall(void);
extern void YxAppEndCallCb(void);
extern void YxAppCallEndProc(void);
extern void YxAppCallSetAudioMute(char onoff);
//sms apis
extern void YxAppSmsCheckDateTime(MYTIME *timeStamp);
extern void YxAppSetSmsNumber(S8 *number,U8 length);
extern void YxAppSendSms(S8* number, U16* content,char gsmCode);//号码为ASCII码,内容则要为UNICODE码,低字节在前,高字节在后,最大长度为YX_APP_MAX_SMS_CHARS,gsmCode:此为发送编码格式为GSM7,为0时,则为SRV_SMS_DCS_UCS2
//gprs apis
extern U32 YxAppDtcntMakeDataAcctId(char simId,char *apnName,char apnType,U8 *appId);
extern void YxAppUploadDataFinishCb(void);
extern S32 YxappAddOneApn(char simId,const U8 *apnName,const U8 *accountName,char *userName,char *password);

//socket apis
extern U16 YxAppGetHostNameAndPort(S8 **hostName);
extern S8 YxAppTcpConnectToServer(const S8* host, U16 port, S8 apn);//apn:only:MAPN_WAP,MAPN_NET
extern S32 YxAppTcpSockRead(U8 *buf,S32 len);
extern S32 YxAppTcpSockWrite(U8 *buf, S32 len);
extern void YxAppCloseSocket(char reconnect);//reconnect,为1:需要重新连接,为0:不需要,直接关闭
extern S8 YxAppConnectToMyServer(void);
extern void YxAppRestartServerTimer(void);
extern void YxAppReconnectServer(void);
extern void YxAppCloseNetwork(void);
extern S8 *YxAppGetServerName(U16 *port);
//file apis
extern void YxAppSetServerParams(char *host,U16 port);//保存服务器有关参数
extern YXMONITORPARAM *YxAppGetMonitorList(U8 index,S8 loadFisrt);//获取监护者信息
extern void YxAppSaveMonitorList(void);//保存取监护者信息
extern char YxAppSetMonitorInfor(U8 idx,S8 *number,U8 *names);
extern char YxAppGetMonitorNameByCallNumber(S8 *number,U8 *nameBuf);
extern void YxAppPrintToFile(U8 *buffer,U16 length);
extern S8 *YxAppGetCallNumberBuffer(void);
//white name list
extern YXMONITORPARAM *YxAppGetFirewallItemBy(U8 idx);
extern char YxAppGetFirewallNameByCallNumber(S8 *number,U8 *nameBuf);
extern char YxAppSetFirewallInfor(U8 idx,S8 *number,U8 *names);
extern void YxAppSaveFirewallList(void);
extern U16 YxAppSearchNumberFromList(S8 *number);
extern U8 YxAppGetNumberItem(U8 index,PU8 name,PU8 usc2_number);
//firewall
extern U8 YxAppFirewallReadWriteStatus(U8 status);
extern char YxAppFirewallReadWriteTime(char read,U16 *startTime,U16 *endTime);
extern YXSYSTEMPARAM *YxAppGetSystemParam(S8 loadFile);
extern void YxAppSaveSystemSettings(void);
extern char YxAppCheckTimeIsHiddenTime(void);
//gps apis
extern void YxAppGpsControl(char operation);
extern void YxAppSetGpsKind(U8 mode);
extern void YxGpsCheckFiveMinuteTimeOut(void);
extern void YxAppGpsInition(U8 defaultGps,U16 gpsTimes);
extern void GpsGetTimeValue(U8 *hour,U8 *minute,U8 *second);
extern char GpsGetDateValue(U16 *year,U8 *month,U8 *day);
extern double ConvertJwDdmm2Dd(double ll); //经纬度转成地球常用坐标点
extern double ConvertSpeedKnotToKm(double speed);//海里转换成公里
extern void GpsTaskDealWithMessage(void);//处理从GPS芯片读到的数据
extern double GpsGetLongitudeV(void);
extern double GpsGetLatitudeV(void);
extern double GpsGetHightV(void);
extern double GpsGetSpeedV(void);
extern double GpsGetAngleV(void);
#if (YX_GPS_USE_AGPS==1)
extern void YxAgpsUpdateData(U8 *buffer,U16 length);
extern char YxAgpsDataWriteToIc(U8 *buffer,U16 DataLen);
extern char YxAgpsGetDataTime(U8 *buffer);
//files
extern U16 YxAppGetAgpsData(kal_uint8 *data,U16 maxLen);
extern void YxAppSaveAgpsData(kal_uint8 *data,U16 dataLen);
#endif
extern double YxAppGetGpsTwoPointDistance(double lat1, double long1, double lat2, double long2);
//profile
extern char YxAppGetCurrentProfile(void);
extern void YxAppSetCurrentProfile(char value);
//battery low shreshold
extern char YxAppSystemReadWriteLowBattery(char read,char value);
extern void YxAppCheckLowBatteryShreshold(U8 percent);
//efence
extern U8 YxAppGetEfenceNumber(void);
extern void YxAppCheckEfence(double log,double lat);
extern char YxAppEfenceUpdate(kal_uint8 i,kal_uint8 *eNames,kal_uint8 kind,double lon,double lat,double radius);
extern void YxAppEfenceSaveData(void);
//logs
extern void YxAppLogAdd(U8 kind,U8 action,U16 value);
extern YXLOGPARAM *YxAppGetLogByIndex(char index);
extern void YxAppClearAllLogs(void);
extern void YxAppSaveLogData(void);
//alarms
extern U8 YxAppGetActiveAlarmNumber(void);
extern void YxAppSetAlarm(U8 index,U8 hour,U8 min,S8 status,S8 freq,U8 days);
//date time
extern char YxAppSystemDtIsDefault(void);
extern void YxAppSetTimeFormat(char kind);
//shutdown
extern void YxAppSetAllowShutdown(void);
extern void YxAppRunShutdownCmd(void);
extern void YxAppAutoShutdown(void);
//find watch
extern void YxAppSearchWtPlayTone(void);
//backlight
extern void YxAppSetBacklightTime(char kind);
//app
extern U16 YxAppUTF8ToUincode(U8 *inbuf,U16 lenin,U8 *outbuf,U16 lenout);
extern U16 YxAppUincodeToUTF8(U8 *inbuf,U16 lenin,U8 *outbuf,U16 lenout);
extern U16 YxAppGetRunFlag(void);
extern void YxAppStepRunMain(U16 runKind);
extern void YxAppInitionParam(void);

//data proc callback
extern U16 YxAppBuildTxtCmdGpsUploadPackage(char savepkg,U8 *sendBuf,U16 maxLen);
extern U16 YxAppBuildTxtCmdHeartUploadPackage(U8 *sendBuf,U16 maxLen);
extern S32 YxAppSockConnectedCallback(U8 *sendBuf,U16 maxLen);
extern S32 YxAppSockReadDataCallback(U8 *readBuf,U16 readLen,U8 *sendBuffer);
extern void YxAppSaveUnsendTxtcmdPackage(void);
extern void YxAppDeleteUnsendPkgFile(void);
extern S32 YxAppGetUnsendDataLength(void);
extern S32 YxAppGetUnsendDataFromFile(U8 *buffer,S32 maxLen,S32 offset);

//timer tick
extern void YxAppUploadDataByTickProc(void);
extern void YxAppStartPosAtOnceProc(char kind);
extern void YxAppSecondTickProc(void);
extern void YxAppStartSecondTimer(char start);
extern void YxAppHeartTickProc(void);
//app main
extern void YxAppUploadHeartProc(void);
extern void YxAppUploadGpsProc(void);
extern void YxAppSetOntimeCallGpsRunFlag(void);
extern U16 YxAppGetSystemTick(char second);
extern void YxAppEndAllAction(void);//关机调用
#if (YX_PROTOCOL_KIND==1)
extern void YxProtocolInition(void);
extern void YxProtocolSetSosStatus(void);
extern PU8 YxProtocolGetBigBuffer(void);
#endif
#ifdef __cplusplus
}
#endif
#endif
