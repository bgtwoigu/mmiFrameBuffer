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

/////////////////////////////////////////////////////////Global var/////////////////////////////////////////////////////////////

static U16  yxAppNeedRunFlag = 0;
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

void YxAppUploadDataFinishCb(void)
{
	char  kind = 0;
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
	}
	if(!(yxAppParam->uploadStart & YX_UPLOAD_KIND_GPS) && !(yxAppParam->uploadStart & YX_UPLOAD_KIND_HEART))
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
	}
	else
	{
		yxAppParam->heartTick = 0;
		if((yxAppParam->uploadStart & YX_UPLOAD_KIND_GPS)&&(!(yxAppNeedRunFlag & YX_RUNKIND_OWNER_GPS)))
		{
			yxAppParam->uploadStart = 0;
			yxAppParam->gpsTick = 200;
			YxAppUploadGpsProc();
		}
	}
#if(YX_IS_TEST_VERSION!=0)
	YxAppTestUartSendData((U8*)"upload end\r\n",12);
#endif
}

void YxAppStepRunMain(U16 runKind)
{
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
}

extern int ApolloWatchConfig(void);
extern WatchConf WatchInstance;
void YxAppIdleInition(void)
{
#if ENABLE_MMI_FRAMEBUFFER //Update By WangBoJing 20160327
	mmiFrameBufferInit();
#endif
#if ENABLE_MMI_KEYPAD
	mmiKeyPadInit();
#endif
	if((yxAppParam)&&(yxAppParam->allIsReady==1))
		return;
#if(YX_GSENSOR_SURPPORT!=0)
	YxAppGsensorProcInit();
#endif
	YxAppMMiRegisterMsg();
#if(YXAPP_UART_TASK_OWNER_WAY==1)
	YxAppRegisterUartMsgToMMiTask();
#endif
	if(yxAppParam)
		yxAppParam->allIsReady = 1;
	YxAppGetImeiNumber();//add by ldz
#if (YX_PROTOCOL_KIND==1)
	YxProtocolInition();
#endif
	ApolloWatchConfig();
	YxAppStartSecondTimer(1);
	yxAppExit = 0;
	yxAppParam->sleephandle = L1SM_GetHandle();
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
	YxAppUploadGpsProc();
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
}

void YxAppUploadHeartProc(void)
{
	YXSYSTEMPARAM     *yxSetting = YxAppGetSystemParam(0);
	if(yxAppParam==NULL || yxAppParam->allIsReady<3)
		return;
	if(yxAppExit==1)
		return;
	if(yxSetting->systemTick + 1 >= 0xFF00)
		yxSetting->systemTick = 0;
	else
		yxSetting->systemTick++;
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
		}
		return;
	}
#if(YX_IS_TEST_VERSION!=0)
	YxAppTestUartSendData((U8*)"10second\r\n",10);
#endif
	yxAppParam->heartTick++;
	if(yxAppParam->heartTick >= yxSetting->uploadTick)
	{
		yxAppParam->heartTick = 0;
		if(yxAppNeedRunFlag & YX_RUNKIND_OWNER_HEART)
			return;
		else
		{
			S8 res = YxAppConnectToMyServer();
			yxAppNeedRunFlag = YX_RUNKIND_OWNER_HEART;
			yxAppParam->uploadStart |= YX_UPLOAD_KIND_HEART;
			if(res==0)//传输失败
				YxAppUploadDataFinishCb();
			else if(res==3)//busing
				yxAppNeedRunFlag = 0;
			return;
		}
	}
	return;
}
#endif