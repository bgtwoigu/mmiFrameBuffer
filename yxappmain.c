#ifdef __USE_YX_APP_SERVICE__
#include "MMI_features.h"
#include "MMIDataType.h"
#include "kal_release.h"
#include "Gdi_layer.h"
#include "NVRAMEnum.h"
#include "NVRAMProt.h"
#include "NVRAMType.h"
#include "custom_nvram_editor_data_item.h"
#include "l1sm_public.h"
#ifdef IK158_BB
#include "gui_typedef.h"
#include "wgui_categories_util.h"
#endif
#include "YxBasicApp.h"

#include "TimerEvents.h"

/////////////////////////////////////////////////////////Global var/////////////////////////////////////////////////////////////

static U8  u8HeartPacketTimes = 0;
static U16  yxAppNeedRunFlag = APOLLO_RUNKIND_HEART_COMPLETE;
static U8   yxAppExit = 0;
static YXAPPPARAM *yxAppParam = NULL;

/////////////////////////////////////////////////////////Global api/////////////////////////////////////////////////////////////

#ifdef IK158_BB
static void YxAppSetAppIconDisplay(void)
{
	static U8  yxiconisset = 0;
	if(yxiconisset==0)
	{
		wgui_status_icon_bar_set_icon_display(STATUS_ICON_CIRCLE);
		wgui_status_icon_bar_update();
		yxiconisset = 1;
	}
}
#endif

U16 YxAppGetRunFlag(void)
{
	return yxAppNeedRunFlag;
}

void ApolloAppSetRunFlag(U16 flag) {
	yxAppNeedRunFlag = flag;
}

void ApolloAppSetPacketTimes(U8 u8Times) {
	u8HeartPacketTimes = u8Times;
}

void YxAppDisableSleep(void)
{
	if(yxAppParam->sleepdisable==0)
	{
		if(yxAppParam->sleephandle!=0 && yxAppParam->sleephandle!=0xFF)
			L1SM_SleepDisable(yxAppParam->sleephandle);
		yxAppParam->sleepdisable = 1;
	}
}

void YxAppEnableSleep(void)
{
	if(yxAppParam->sleepdisable==1)
	{
		if(yxAppParam->sleephandle!=0 && yxAppParam->sleephandle!=0xFF)
			L1SM_SleepEnable(yxAppParam->sleephandle);
		yxAppParam->sleepdisable = 0;
	}
}

extern U16 apollo_flag;
extern U16 save_flag;

/*
 * #define  YX_RUNKIND_OWNER_GPS    0x8000  //period upload data by timer
#define  YX_RUNKIND_OWNER_HEART  0x4000  //心跳包
//max 
#define APOLLO_RUNKIND_HEART_COMPLETE 	0x2000

#define APOLLO_RUNKIND_VOICE_UPLOAD		0x0020
#define APOLLO_RUNKIND_VOICE_DOWNLOAD	0x0040
#define APOLLO_RUNKIND_INIT_WATCHDATA	0x0080
 */

void YxAppUploadDataFinishCb(void)
{
	char  kind = 0;
	extern void ApolloConnectToServer(void) ;
	extern U8 u8Time;
	u8Time = 0;

	kal_prompt_trace(MOD_YXAPP," YxAppUploadDataFinishCb :%x\n", yxAppNeedRunFlag);
	apollo_flag = YxAppGetRunFlag();

	if (apollo_flag & APOLLO_RUNKIND_VOICE_UPLOAD) {
		//ApolloAppSetRunFlag(APOLLO_RUNKIND_VOICE_UPLOAD);
		//ApolloConnectToServer();
	} else if (apollo_flag & YX_RUNKIND_OWNER_GPS) {
		//ApolloAppSetRunFlag(YX_RUNKIND_OWNER_GPS);
		//ApolloConnectToServer();
	} else if (apollo_flag & APOLLO_RUNKIND_VOICE_DOWNLOAD) {
		//ApolloAppSetRunFlag(APOLLO_RUNKIND_VOICE_DOWNLOAD);
		//ApolloConnectToServer();
	} else if (apollo_flag & APOLLO_RUNKIND_AGPS_DOWNLOAD) {
		//ApolloAppSetRunFlag(APOLLO_RUNKIND_VOICE_DOWNLOAD);
		//ApolloConnectToServer();
	} else if (apollo_flag & APOLLO_RUNKIND_INIT_WATCHDATA) {
		
	}
#if 0
	else if (apollo_flag & APOLLO_RUNKIND_VOICE_DOWNLOAD) {
		//YxAppConnectToMyServer();
		ApolloAppSetRunFlag(APOLLO_RUNKIND_VOICE_DOWNLOAD);
		ApolloConnectToServer() ;
	}
#endif	
	apollo_flag = 0x0;
	return ;
#if 0	
	if(yxAppNeedRunFlag & YX_RUNKIND_OWNER_GPS)
	{
		yxAppNeedRunFlag &= ~YX_RUNKIND_OWNER_GPS;
		yxAppParam->uploadStart &= ~YX_UPLOAD_KIND_GPS;
		kind = 1;
	}
	else if(yxAppNeedRunFlag & YX_RUNKIND_OWNER_HEART)
	{
		yxAppNeedRunFlag &= ~YX_RUNKIND_OWNER_HEART;
		yxAppParam->uploadStart &= ~YX_UPLOAD_KIND_HEART;
	} else if (yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_UPLOAD) {
		yxAppNeedRunFlag &= APOLLO_RUNKIND_VOICE_UPLOAD;
		yxAppParam->uploadStart &= ~YX_UPLOAD_KIND_VOICE;
		kind = 2;
	} else if (yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_DOWNLOAD) {
		yxAppNeedRunFlag &= APOLLO_RUNKIND_VOICE_DOWNLOAD;
		yxAppParam->uploadStart &= ~YX_DOWNLOAD_KIND_VOICE;
		kind = 3;
	} 

	kal_prompt_trace(MOD_YXAPP, "YxAppUploadDataFinishCb :%x; kind:%d\n", yxAppNeedRunFlag, kind);
	if(!(yxAppParam->uploadStart & YX_UPLOAD_KIND_GPS) && !(yxAppParam->uploadStart & YX_UPLOAD_KIND_HEART) &&
		 !(yxAppParam->uploadStart & YX_UPLOAD_KIND_VOICE) &&  !(yxAppParam->uploadStart & YX_DOWNLOAD_KIND_VOICE))
	{
		yxAppParam->uploadStart = 0;
		yxAppNeedRunFlag = 0;
	}
	if(kind==1)
	{
		YxAppEnableSleep();
		YxAppSaveUnsendTxtcmdPackage();
		yxAppParam->gpsTick = 0;
		if((yxAppParam->uploadStart & YX_UPLOAD_KIND_HEART)&&(!(yxAppNeedRunFlag & YX_RUNKIND_OWNER_HEART)))
		{
			yxAppParam->uploadStart = 0;
			yxAppParam->heartTick = 120;
			YxAppUploadHeartProc();
		}
	} else if (kind == 2) {
		
	} else if (kind == 3) {
		
	} else {
		yxAppParam->heartTick = 0;
		if((yxAppParam->uploadStart & YX_UPLOAD_KIND_GPS)&&(!(yxAppNeedRunFlag & YX_RUNKIND_OWNER_GPS))
			&& (!(yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_UPLOAD)) && (!(yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_DOWNLOAD)))
		{
			yxAppParam->uploadStart = 0;
			yxAppParam->gpsTick = 200;
			YxAppUploadGpsProc();
		}
	}
#if(YX_IS_TEST_VERSION!=0)
	YxAppTestUartSendData((U8*)"upload end\r\n",12);
#endif
#endif
}

void YxAppStepRunMain(U16 runKind)
{
#if 0
	U16   oldFlag = yxAppNeedRunFlag&0x3FFF;
	if(yxAppParam->allIsReady!=3)
	{
		if(runKind==YX_RUNKIND_WIFI)
		{
			yxAppParam->allIsReady = 3;
#ifdef IK158_BB
			YxAppSetAppIconDisplay();
#endif
		}
		return;
	}
	yxAppNeedRunFlag &= (~runKind);
	if((yxAppNeedRunFlag&0x3FFF)==0)
	{
		if(oldFlag)//connect to sever
		{
			S8 res = YxAppConnectToMyServer();
			if(res==0)//传输失败
				YxAppUploadDataFinishCb();
			else if(res==3)//busing
				YxAppSaveUnsendTxtcmdPackage();
		}
		return;
	}
	//if(runKind==YX_RUNKIND_LBS)
	//	YxAppDisableSleep();
	if(yxAppNeedRunFlag & YX_RUNKIND_LBS)
		YxAppCellRegReq();
#ifdef __MMI_WLAN_FEATURES__
	else if(yxAppNeedRunFlag & YX_RUNKIND_WIFI)
		YxAppWlanPowerScan(1);
#endif
#ifdef __MMI_BT_SUPPORT__
	else if(yxAppNeedRunFlag & YX_RUNKIND_BT_SEARCH)
		YxAppBtSearchDevices();
#endif
	else if(yxAppNeedRunFlag & YX_RUNKIND_GPS)
	{
		YxAppDisableSleep();
		YxAppGpsControl(1);
	}
#else
	
#endif
}

void test(void)
{
	srv_gpio_setting_set_bl_time(10);
}

extern void ApolloMinuteTimerCallback(void) ;
extern void ApolloStartGPS(void) ;
extern int ApolloWatchConfig(void);
extern void ApolloStartLocationTimer(int timeType);
extern void ApolloInitTime(void);
extern void ApolloStartLab(void);
extern void ApolloStartGPS(void);

extern WatchConf WatchInstance;
#if ENABLE_MMI_INITTIME
extern U8 u8NeedInitWatchTimeFlag;
#endif
#if (YX_GPS_USE_AGPS == 1)
extern U8 u8NeedInitAgpsDataFlag;
extern YXGPSPARAMSTR  yxGpsParam;
#endif

void YxAppIdleInition(void)
{
#if ENABLE_MMI_INITTIME
	u8NeedInitWatchTimeFlag = 0x01;
	ApolloInitTime();
#endif
#if ENABLE_MMI_FRAMEBUFFER //Update By WangBoJing 20160327
	mmiFrameBufferInit();
#endif
#if ENABLE_MMI_KEYPAD
	mmiKeyPadInit();
#endif
	if((yxAppParam)&&(yxAppParam->allIsReady==1))
		return;

	YxAppMMiRegisterMsg();

	if(yxAppParam)
		yxAppParam->allIsReady = 1;
	
	YxAppGetImeiNumber();//add by ldz
	
#if (YX_PROTOCOL_KIND==1)
	YxProtocolInition();
#endif

	ApolloWatchConfig();

	YxAppStartSecondTimer(1);
	//ApolloMinuteTimerCallback();
	//StartTimer(APOLLO_MINUTE_TIMER, YX_HEART_TICK_UNIT * 72, ApolloMinuteTimerCallback);
	//StartTimer(APOLLO_GPS_ONOFF_TIMER, YX_HEART_TICK_UNIT * 36, ApolloStartGPS);
	ApolloStartLocationTimer(4);
	//yxAppExit = 0;
	
	yxAppParam->sleephandle = L1SM_GetHandle();

#ifndef  WIN32
 	power_manage_ini(); 
	VCAMA_ON();
#endif

	Uart2_ini(9600);

//#if UART_LOG	
	Uart1_ini();
//#endif

	//dis_ini();
	//key_ini();


	//I2C_APDS9901_START();
	BMA250E_task_ini();
	
	//yaoyiyao_ini();//摇一摇初始化
	
	hand_mode_ini();// 抬手显示 初始化

	srv_gpio_setting_set_bl_time(10);
	
	//StartTimer(I2C_TIMER,2000,yaoyiyao_ini);//继续刷
#if (YX_GPS_USE_AGPS == 1)
	yxGpsParam.yxAgpsDatLen = YxAppGetAgpsData(yxGpsParam.yxAgpsDataBuf,AGPS_MAX_RECEIVE_BUFFER_LEN);
	u8NeedInitAgpsDataFlag = 0x01;
	//ApolloStartLab();
	//ApolloStartGPS();
		//StartTimer( YX_HEART_TICK_UNIT*2,ApolloStartLab);
#endif
	
}

void YxAppInitionParam(void)
{
	yxAppParam = (YXAPPPARAM*)OslMalloc(sizeof(YXAPPPARAM));
	memset((void*)yxAppParam,0,sizeof(YXAPPPARAM));
}

void YxAppUploadDataByTickProc(void)
{
	if(yxAppExit==1 || yxAppParam==NULL || yxAppParam->allIsReady==0)
		return;
//	YxAppUploadGpsProc();
//	YxAppSendMsgToMMIMod(YX_MSG_MAINAPP_TICK_CTRL,0,0);
}

void YxAppSecondTickProc(void)
{
	if(yxAppExit==1 || yxAppParam==NULL || yxAppParam->allIsReady<3)
		return;
	YxAppSendMsgToMMIMod(YX_MSG_MAINAPP_SECOND_CTRL,0,0);
}

void YxAppStartPosAtOnceProc(char kind)//kind:1:is sos call,0:other call
{
	if(yxAppParam==NULL || yxAppParam->allIsReady<3)
		return;
	if(kind==1)
		YxProtocolSetSosStatus();
	if(yxAppParam->uploadStart==0)
		yxAppParam->gpsTick = 200;
	YxAppSendMsgToMMIMod(YX_MSG_START_POS_CTRL,0,0);
}

void YxAppSetOntimeCallGpsRunFlag(void)
{
	yxAppParam->uploadStart |= YX_UPLOAD_KIND_GPS;
}

U16 YxAppGetSystemTick(char second)
{
	YXSYSTEMPARAM     *yxSetting = YxAppGetSystemParam(0);
	if(second)
		return yxSetting->systemTick;
	return yxSetting->minuteTick;
}

void YxAppEndAllAction(void)
{
	yxAppExit = 1;
	YxAppStartSecondTimer(0);
	YxAppCloseNetwork();
}


void YxAppUploadGpsProc(void)
{
#if 0
	YXSYSTEMPARAM     *yxSetting = YxAppGetSystemParam(0);
	if(yxAppParam->allIsReady==1)//开机前1分钟,获取本机的BTMAC,name,wlan mac
	{
#ifdef __MMI_BT_SUPPORT__
		YxAppGetLocalBtNameMac(NULL,NULL);
#endif
#ifdef __MMI_WLAN_FEATURES__
		YxAppWlanPowerScan(1);
#else
		yxAppParam->allIsReady = 3;
#ifdef IK158_BB
		YxAppSetAppIconDisplay();
#endif
#endif
#if(YX_IS_TEST_VERSION!=0)
		YxAppTestUartSendData((U8*)"getmac\r\n",9);
#endif
		return;
	}
	if(yxAppParam==NULL || yxAppParam->allIsReady<3)
		return;
	if(yxAppExit==1)
		return;
	if(yxSetting->minuteTick + 1 >= 0xFF00)
		yxSetting->minuteTick = 0;
	else
		yxSetting->minuteTick++;
	YxGpsCheckFiveMinuteTimeOut();
#if 1
	if(yxAppParam->uploadStart)
	{
		if((yxAppParam->uploadStart & YX_UPLOAD_KIND_GPS) == 0)
		{
			yxAppParam->gpsTick++;
#if(YX_IS_TEST_VERSION!=0)
			if(yxAppParam->gpsTick >= 2)
#else
			if(yxAppParam->gpsTick >= yxSetting->gpsTimes)
#endif
			{
				yxAppParam->gpsTick = 0;
				yxAppParam->uploadStart |= YX_UPLOAD_KIND_GPS;
			}
		}
		return;
	}
#else
#if(YX_NO_SIMCARD_GET_DATA==0)
	char   res = YxAppGetSimOperator(YX_APP_SIM1);
	if((yxAppParam->uploadStart==1)||(res==MSIM_OPR_NONE)||(res==MSIM_OPR_UNKOWN))
		return;
#else
	if(yxAppParam->uploadStart==1)
		return;
#endif
#endif
	yxAppParam->gpsTick++;
#if(YX_IS_TEST_VERSION!=0)
	YxAppTestUartSendData((U8*)"mintick\r\n",9);
#endif
#if(YX_IS_TEST_VERSION!=0)
	if(yxAppParam->gpsTick >= 2)
#else
	if(yxAppParam->gpsTick >= yxSetting->gpsTimes)
#endif
	{
		yxAppParam->gpsTick = 0;
		if(yxAppNeedRunFlag & YX_RUNKIND_OWNER_GPS)//正在运行
			return;
		yxAppNeedRunFlag = YX_RUNKIND_OWNER_GPS|YX_RUNKIND_LBS|YX_RUNKIND_GPS;
#ifndef WIN32
#ifdef __MMI_WLAN_FEATURES__
		yxAppNeedRunFlag |= YX_RUNKIND_WIFI;
#endif
#ifdef __MMI_BT_SUPPORT__
		//if(strlen((char*)(yxSetting->lastBt))==14)
			yxAppNeedRunFlag |= YX_RUNKIND_BT_SEARCH;
#endif
#endif
		YxAppStepRunMain(0);
		yxAppParam->uploadStart |= YX_UPLOAD_KIND_GPS;
		return;
	}
#endif
}


void ApolloUploadGpsProc(void) {
#if 0
	kal_prompt_trace(MOD_YXAPP," ApolloUploadGpsProc :%x, ApolloFlag:%x\n", yxAppNeedRunFlag, apollo_flag);
	if (yxAppNeedRunFlag & APOLLO_RUNKIND_HEART_COMPLETE || yxAppNeedRunFlag & YX_RUNKIND_OWNER_HEART) {
		extern void ApolloConnectToServer(void) ;
		yxAppNeedRunFlag |= YX_RUNKIND_OWNER_GPS;
		ApolloConnectToServer();
	} else {
		save_flag = YX_RUNKIND_OWNER_GPS;
	}
#else
	if ((yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_UPLOAD) 
		|| (yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_DOWNLOAD)
		|| (yxAppNeedRunFlag & APOLLO_RUNKIND_AGPS_DOWNLOAD)) {
		;
	} else {
		extern void ApolloConnectToServer(void) ;
		yxAppNeedRunFlag |= YX_RUNKIND_OWNER_GPS;
		ApolloConnectToServer();
	}
#endif
	
}

char checkApolloFlagStatus(void) {
	if ((yxAppNeedRunFlag & APOLLO_RUNKIND_HEART_COMPLETE) || (yxAppNeedRunFlag & YX_RUNKIND_OWNER_HEART)
		|| (yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_UPLOAD) || (yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_DOWNLOAD)
		|| (yxAppNeedRunFlag & YX_RUNKIND_OWNER_GPS) || (yxAppNeedRunFlag & APOLLO_RUNKIND_AGPS_DOWNLOAD )) {
		return 1;
	} else {
		return 0;
	}
}

void YxAppUploadHeartProc(void)
{
	//YXSYSTEMPARAM     *yxSetting = YxAppGetSystemParam(0);	
	extern void ApolloConnectToServer(void) ;
	
	if (u8HeartPacketTimes++ > 4) {
		yxAppParam->uploadStart = 0;
		yxAppNeedRunFlag = APOLLO_RUNKIND_HEART_COMPLETE;
		u8HeartPacketTimes = 0;
	}
	kal_prompt_trace(MOD_YXAPP," YxAppUploadHeartProc :%x\n", yxAppNeedRunFlag);
	if (!checkApolloFlagStatus()) {
		yxAppNeedRunFlag = APOLLO_RUNKIND_HEART_COMPLETE;
	}
	
#if 0
	if ((yxAppNeedRunFlag & APOLLO_RUNKIND_HEART_COMPLETE) || (yxAppNeedRunFlag & YX_RUNKIND_OWNER_HEART)) {		
		extern U8 u8Time;

		if (save_flag & YX_RUNKIND_OWNER_GPS) {
			yxAppNeedRunFlag = YX_RUNKIND_OWNER_GPS;
			save_flag = 0x0;
			ApolloConnectToServer();

			return ;
		}
		if (save_flag & APOLLO_RUNKIND_VOICE_UPLOAD) {
			save_flag = 0x0;
			ApolloAppSetRunFlag(APOLLO_RUNKIND_VOICE_UPLOAD);
			ApolloConnectToServer();

			return ;
		}
		
		yxAppNeedRunFlag |= YX_RUNKIND_OWNER_HEART;
		u8Time = 0;
		ApolloConnectToServer();
	}
#elif 1
	if ((yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_UPLOAD)
		|| (yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_DOWNLOAD)
		|| (yxAppNeedRunFlag & APOLLO_RUNKIND_AGPS_DOWNLOAD)
		|| (yxAppNeedRunFlag & YX_RUNKIND_OWNER_GPS)) {
		;
	} else {
		yxAppNeedRunFlag |= YX_RUNKIND_OWNER_HEART;
		u8Time = 0;
		ApolloConnectToServer();
	}
#else	
	
	if(yxAppParam==NULL || yxAppParam->allIsReady<3)
		return;
	if(yxAppExit==1)
		return;
	if(yxSetting->systemTick + 1 >= 0xFF00)
		yxSetting->systemTick = 0;
	else
		yxSetting->systemTick++;
	kal_prompt_trace(MOD_YXAPP," YxAppUploadHeartProc :%x, %d\n", yxAppNeedRunFlag, yxAppParam->uploadStart);
	if(yxAppParam->uploadStart)
	{
		if((yxAppParam->uploadStart & YX_UPLOAD_KIND_HEART) == 0)
		{
			yxAppParam->heartTick++;
			if(yxAppParam->heartTick >= yxSetting->uploadTick)
			{
				yxAppParam->heartTick = 0;
				yxAppParam->uploadStart |= YX_UPLOAD_KIND_HEART;
			}
			return;
		} else if((yxAppParam->uploadStart & YX_UPLOAD_KIND_VOICE) == 0) {

		} else if((yxAppParam->uploadStart & YX_UPLOAD_KIND_GPS) == 0) {

		} else if((yxAppParam->uploadStart & YX_DOWNLOAD_KIND_VOICE) == 0) {

		}
	}
#if(YX_IS_TEST_VERSION!=0)
	YxAppTestUartSendData((U8*)"10second\r\n",10);
#endif
	yxAppParam->heartTick++;
	if(yxAppParam->heartTick >= yxSetting->uploadTick)
	{
		yxAppParam->heartTick = 0;
		if(yxAppNeedRunFlag & YX_RUNKIND_OWNER_HEART || yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_DOWNLOAD || 
			yxAppNeedRunFlag & APOLLO_RUNKIND_VOICE_UPLOAD)
			return;
		else
		{
			extern void ApolloConnectToServer(void) ;
			extern U8 u8Time;
			u8Time = 0;
			ApolloConnectToServer();
			//StartTimer(APOLLO_RESEND_MESSAGE_TIMER, 2000, ApolloConnectToServer);
			//S8 res = YxAppConnectToMyServer();
			yxAppNeedRunFlag = YX_RUNKIND_OWNER_HEART;
			yxAppParam->uploadStart |= YX_UPLOAD_KIND_HEART;
			#if 0
			if(res==0)
				YxAppUploadDataFinishCb();
			else if(res==3)//busing
				yxAppNeedRunFlag = 0;
			#endif
			return;
		}
	}
	return;
#endif
}
#endif
