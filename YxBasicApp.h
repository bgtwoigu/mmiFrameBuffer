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
/***************************************************����������ǵ���Ҫ�޸���غ�**************************************************/
//debug
#define  YX_IS_TEST_VERSION          0               //�Ƿ��ǲ��԰汾,0:�����汾,1:����1���Բ���ӡ���,2:USB���Բ���ӡ���,USB��ӡʱ
                                                     //���Զ˹���Ҫ��catcher,ͬʱ������custom\common\hal\nvram\nvram_data_items.c
                                                     //��static port_setting_struct const NVRAM_EF_PORT_SETTING_DEFAULT[]������,tst-ps uses uart_port_null(value is 99),99��Ϊ4����
                                                     //��catcher��FILLTER��ѡֻһ��MOD��MOD_BT�Ϳ��Կ������ǵĴ�ӡ�����Ϣ

//GPS��������
#define  YXAPP_USE_UART              uart_port2      //������Ҫ�õĴ��ں�,Ĭ��Ϊuart2
#define  YX_GPS_UART_BAUDRATE        UART_BAUD_115200  //GPSĬ�ϵĲ�����,֧��MT3337,UBOX-7020��U8,�Լ�����NMEA0183Э���GPSоƬ,UART_BAUD_9600,UART_BAUD_115200,(MT3337:115200,ublox:9600)
#define  YXAPP_UART_TASK_OWNER_WAY   0               //������Ĵ��������Ǹ�TASK,0:MOD_YXAPP, 1:MOD_MMI
#define  YXAPP_GPS_KIND              0               //���GPSоƬ�Ƿ�֧������ϵͳ,���ֻ֧��GPS����0,�����(GP,BD,GL)�ֶ�֧��,����3,3:GN��ģ��λ,2:GL,1:ֻ֧��BD,0:ֻ֧��GPS
//ע��GPS����һ����Դ���ƽ���������,��Ҫ����ԭ��ͼ�޸�һ��,��������.API����Ϊ:YXAppGpsPowerCtrl

//������������˿�,Ŀǰֻ֧��TCP��ʽ,HTTP����TCP��ʽ
#define  YX_NET_WAY                  MAPN_NET        //MAPN_NET,MAPN_WAP,������ʽ,NET��WAP����,����WAP��ʽ������ͨ���ƶ�����ͨ����
	                                                 //10.0.0.172����������,�˷�ʽֻ����HTTP������,���Ҷ˿�һ��ҪΪ80,�������ز���ȥ��
                                                     //NET��ʽ�Ͳ���ͨ��������,��ֱ�ӿ������ӷ�������,û�ж˿�����
#define  YX_HOST_NAME_MAX_LEN        40              //��������ַ���

#if 0//��Ҫ���ĳ����ǵķ�����IP��˿�
#define  YX_DOMAIN_NAME_DEFAULT      "192.168.1.1" //��������������ip��ַ,��:192.186.1.1
#define  YX_SERVER_PORT              5088            //������port,�����HTTP������,�˿�Ĭ��Ϊ80,�����ͨ��CMWAP��ʽ����,��ֻ����HTTP������,���Ҷ˿�һ��ҪΪ80
#else
#define  YX_DOMAIN_NAME_DEFAULT      "www.163.com" //��������������ip��ַ,��:192.186.1.1
#define  YX_SERVER_PORT              80            //������port,�����HTTP������,�˿�Ĭ��Ϊ80,�����ͨ��CMWAP��ʽ����,��ֻ����HTTP������,���Ҷ˿�һ��ҪΪ80
#endif
#define  YX_CONN_SERVER_TIME_OUT     60000           //SOCKET read write time out,second
#define  YX_MIN_UPLOAD_TIME          1               //������ʱ����,��Ϊ��λ,Ĭ��Ϊ15��,ϵͳ��ļ�ʱ����ÿ15��һ��TICK
#define  YX_GPS_UPLOAD_TIME          20              //��λ���ϴ����,����Ϊ��λ,Ĭ��Ϊ20����
#define  YX_GPRS_RECONNECT_COUNT     3               //���������ͨѶ�жϻ�û���ӵ�������ʱ������������Ӵ���
#define  YX_GPS_CONVERT_DM2DD        1               //��Ҫ����Ҫ��GPS������DDMMMMMMת��DDDDDD,1:��Ҫ,0:����Ҫ

#define  YX_GPS_DISPLAY_UI           0               //�Ƿ���ʾGPSͼ��,0:����ʾ
#define  YX_GPS_USE_AGPS             1               //�Ƿ�֧��AGPS,0:��֧��
#define  YX_GSENSOR_SURPPORT         0               //GSENSOR,1:�����жϷ�ʽ,2:TIMER��ʽ,0:��֧��
#define  YX_PROTOCOL_KIND            1               //ͨѶЭ������,
#define  YX_NO_SIMCARD_GET_DATA      1               //û�в���SIM��ʱ,ͬ���ɼ����ݲ���������,���п�ʱ,���ϴ���������,1:֧��,0:��֧��
#define  YX_SMS_CMD_DONT_USE         1               //����Ҫ����ָ��
#define  YX_SECOND_TICK_WAY          2               //��ļ�ʱ��ʽ,1:Ϊstack_timer,2:starttimer
#if(YX_GPS_USE_AGPS==1)
#define  YX_GPS_SAMPLE_DATA_TIME     20              //GPS�ɼ������ʱ��,20��
#else
#define  YX_GPS_SAMPLE_DATA_TIME     50              //GPS�ɼ������ʱ��,50��
#endif
#define  YX_HEART_TICK_UNIT          10000           /**������ÿ10���һ�ε�λ*/
/***************************************************���²��ֵĺ궨�岻��Ҫȥ�Ķ�**************************************************/

#define  YX_APP_MAX_CELL_INFOR_NUM   6               //����վ��,MTK���Ϊ6,��Ҫ����
#define  YX_APP_MAX_SMS_CHARS        70              //һ����������ַ���,��Ҫ����
#define  YX_SOCK_BUFFER_LEN          2048            //��SOCKET��һ�ζ�ȡ���͵�����ֽ���
#define  YX_SOCK_READ_BUFFER_LEN     2048            

#define  YX_FILE_FOLD_NAME           "yxdat"         //��Ŀ¼����
#define  YX_EFENCE_MAX_NUM           3              //������Χ������
#define  YX_EFENCE_NAME_LENGTH       10             //������Χ�����Ƴ���,(unicode)
#define  YX_MAX_MONITOR_NUM          4              //���໤������,00:dady,1:monmy
#define  YX_WHITE_NUM_LIST_MAX       10             //�������б�������
#define  YX_LOGS_MAX_NUM             (YX_EFENCE_MAX_NUM+YX_MAX_MONITOR_NUM+2+2)
#define  YX_BT_MAC_BUFFER_LEN        18             //BT mac�ַ�BUFFER����,�����˽�β0
#define  YX_MAX_ALARM_NUM            3              //���������
#define  YX_BIG_MEMERY_LEN           30*1024        //�����շ��������ݵ��ڴ�
#define  YX_UNSEND_PKG_HEADER_LEN    28             //��ȥ����ͷ�̶���С
#define  YX_UNSEND_PKG_FILE_LEN      (YX_BIG_MEMERY_LEN-YX_UNSEND_PKG_HEADER_LEN)      //���δ�ϴ����ļ���С
#if (YX_GPS_USE_AGPS==1)
#define  AGPS_MAX_RECEIVE_BUFFER_LEN   4*1204       //AGPS�ļ���С
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


//msg command kind    
#define  YX_MSG_BLUETOOTH_CTRL       1
#define  YX_MSG_WLAN_POWER_CTRL      2 //1:power on and scan,0:power off
#define  YX_MSG_LBS_CELL_CTRL        3 //1:get cell infors,0:cancel
#define  YX_MSG_CALL_CTRL            4 //1:make call,0 cancel
#define  YX_MSG_SMS_CTRL             5
#define  YX_MSG_GPRS_CTRL            6
#define  YX_MSG_MAINAPP_TICK_CTRL    7 //����TICK
#define  YX_MSG_MAINAPP_SECOND_CTRL  8 //����TICK
#define  YX_MSG_START_POS_CTRL       9 //������λ
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
#define  YX_RUNKIND_OWNER_HEART  0x4000  //������
//max 
#define APOLLO_RUNKIND_HEART_COMPLETE 	0x2000

#define APOLLO_RUNKIND_VOICE_UPLOAD		0x0020
#define APOLLO_RUNKIND_VOICE_DOWNLOAD	0x0040

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
    U16    mcc;             /*MCC ���Ҵ��� 460*/
    U16    mnc;             /*MNC ��Ӫ�̴���*/
    U16    lac;             /*LAC λ����*/
    U16    ci;              /*CI С����*/
}yxapp_cell_info_struct;
typedef struct{
	char   isRunning;
	U8     cellNum;
	yxapp_cell_info_struct cellInfor[YX_APP_MAX_CELL_INFOR_NUM+1];//0:�ǵ�ǰ����С������Ϣ,����6��Ϊ�ҵ���С��
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
    kal_int8   sock;                    /**< socket ���*/
	S8         apn;
	U8         appid;
    U16        port;                    /**< socket Զ�̶˿�*/
    U8         socket_state;            /**< socket ��·��״̬*/
    U32        account_id;
	S8         hostName[YX_HOST_NAME_MAX_LEN+1];               //�����������IP
	soc_dns_a_struct  dnsIplist;
}YXAPPSOCKCONTEXT;

typedef struct
{
	U8         sendCount;              /*����ط�����,ΪFFʱ,����Ҫ�ط�*/
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

//�豸������ر�״̬
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
	U8      gpsKind;         //��ѡ�õ�GPS����,0:BD,1GP,2:GN
	U8      updateKind;
	U8      gpstatus[4];    //����״̬,GSA���ֵ,��3λ�Ƕ�λ����0:�Զ�,1�ֶ�,��λ����,1δ��λ,2��ά��λ,3��ά ��5λ:�õ������������12��
#if (YX_GPS_USE_AGPS==1)
	U8      yxAgpsDataBuf[AGPS_MAX_RECEIVE_BUFFER_LEN];
	U16     yxAgpsDatLen;
#endif
	U8      gpsTimerVal;     //GPS���ɼ�����ʱ��,YX_GPS_SAMPLE_DATA_TIME
	U8      yxGpsStartTick;
	U16     gpsCircle;      //GPS��������
	double  longitudev[4];   //����,����Ϊ��,0:BD,1:GP,2:GN
	double  latitudev[4];   //γ��,����Ϊ��,0:BD,1:GP,2:GN
	double  hightv[4];       //�߶�,��λΪ��
	double  gpsangle[4];    //���淽λ���汱��ϵ,0:BD,1:GP,2:GN
	double  gpsrspeed[4];   //�ٶȹ���,0:BD,1:GP,2:GN
	double  gpsanglem[4];    //���淽λ�Ǵű���ϵ,0:BD,1:GP,2:GN
	double  gpsrspeedn[4];   //�ٶȺ���,0:BD,1:GP,2:GN
}YXGPSPARAMSTR;

typedef struct
{
	U8      lowBattery;    //�͵��������ż�,ֵֻȡ��7λ,��8λΪ�Ƿ����˶���
	U8      gpsSetKind;    //��ѡ��λϵͳ
	//U8      lastBt[15];    //Ҫ�ҵ�BT�豸��ַ,Ҫת��16����ASCII
	U8      imei[16];      //15 imei
	U8      deviceFlag;    //�豸������ر�״̬
	U8      allowOff;      //�Ƿ�����ػ�,1:ok,0:no
	U16     uploadTick;    //�ϴ����ݼ��ʱ��,����Ϊ��λ
	U16     gpsTimes;      //GPS��������
	U16     systemTick;    //ϵͳ��
	U16     minuteTick;    //ϵͳ��
}YXSYSTEMPARAM;

#define  YX_UPLOAD_KIND_HEART        0x01  //������
#define  YX_UPLOAD_KIND_GPS          0x02  //��λ��
#define YX_UPLOAD_KIND_VOICE		0x03
#define YX_DOWNLOAD_KIND_VOICE		0x04

typedef struct
{
	char    sleepdisable;
	char    uploadStart;
	U8      sleephandle;
	U8      allIsReady;
	U8      heartTick;     //��������ʱ
	U8      gpsTick;       //��λ����ʱ
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
	kal_uint8  isFirst; /**�Ƿ�Ϊ�����ĵ�һ�δ�,1:��*/
	kal_int32  sendLen;
	kal_int32  dataLen;
	kal_int32  totalLen;
}YXPROTOCOLPARAM;
#endif
//make call
typedef struct
{
	S8         number[YX_APP_CALL_NUMBER_MAX_LENGTH+1]; //���ڼ�����ز��绰
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
#define     LOG_EFENCE     '1'  //��LOG���ж��,action:'0':out,'1':in
#define     LOG_LOWBAT     '2'  //��LOGֻ��һ��,�����µ�һ��Ϊ׼
#define     LOG_PHONE      '3'  //���ػ�,action:'1':��,'0':��,��LOGֻ��һ��,�����µ�һ��Ϊ׼
#define     LOG_CALL       '4'  //�绰��¼,��LOGֻ��һ��,�����µ�һ��Ϊ׼,ACT:0 out call,1:in call
#define     LOG_OFFHANDS   '5'  //���ѱ���,action:'0'ȡ��,'1'����

typedef struct
{
	kal_int8     kind;//0:no,1:efence,2:lowbattery,3:listennig,4:mobile
	kal_uint16   date;//month|day
	kal_uint16   time;//hour|min
	kal_uint8    action;
	kal_uint16   value;//efence�����,or,�͵�����ֵor���������ĺ���ֵ,�̶�Ϊ4λ,���㲹0
}YXLOGPARAM;

#if ENABLE_MMI_WATCHCONF
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
#endif

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
extern void YxAppSendSms(S8* number, U16* content,char gsmCode);//����ΪASCII��,������ҪΪUNICODE��,���ֽ���ǰ,���ֽ��ں�,��󳤶�ΪYX_APP_MAX_SMS_CHARS,gsmCode:��Ϊ���ͱ����ʽΪGSM7,Ϊ0ʱ,��ΪSRV_SMS_DCS_UCS2
//gprs apis
extern U32 YxAppDtcntMakeDataAcctId(char simId,char *apnName,char apnType,U8 *appId);
extern void YxAppUploadDataFinishCb(void);
extern S32 YxappAddOneApn(char simId,const U8 *apnName,const U8 *accountName,char *userName,char *password);

//socket apis
extern U16 YxAppGetHostNameAndPort(S8 **hostName);
extern S8 YxAppTcpConnectToServer(const S8* host, U16 port, S8 apn);//apn:only:MAPN_WAP,MAPN_NET
extern S32 YxAppTcpSockRead(U8 *buf,S32 len);
extern S32 YxAppTcpSockWrite(U8 *buf, S32 len);
extern void YxAppCloseSocket(char reconnect);//reconnect,Ϊ1:��Ҫ��������,Ϊ0:����Ҫ,ֱ�ӹر�
extern S8 YxAppConnectToMyServer(void);
extern void YxAppRestartServerTimer(void);
extern void YxAppReconnectServer(void);
extern void YxAppCloseNetwork(void);
extern S8 *YxAppGetServerName(U16 *port);
//file apis
extern void YxAppSetServerParams(char *host,U16 port);//����������йز���
extern YXMONITORPARAM *YxAppGetMonitorList(U8 index,S8 loadFisrt);//��ȡ�໤����Ϣ
extern void YxAppSaveMonitorList(void);//����ȡ�໤����Ϣ
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
extern double ConvertJwDdmm2Dd(double ll); //��γ��ת�ɵ����������
extern double ConvertSpeedKnotToKm(double speed);//����ת���ɹ���
extern void GpsTaskDealWithMessage(void);//�����GPSоƬ����������
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
extern void YxAppEndAllAction(void);//�ػ�����
#if (YX_PROTOCOL_KIND==1)
extern void YxProtocolInition(void);
extern void YxProtocolSetSosStatus(void);
extern PU8 YxProtocolGetBigBuffer(void);
#endif
#ifdef __cplusplus
}
#endif
#endif