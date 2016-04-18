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
#include "DateTimeGprot.h"
#include "gui.h"
#include "TimerEvents.h"
#include "app_datetime.h"
#include "YxBasicApp.h"

#if 1 //Update By WangBoJing 20150914
#include "ApolloProtocol.h"
#endif


////////////////////////////////////////////////////////////Global define///////////////////////////////////////////////////////////////////
#define GetDateTime(t) applib_dt_get_date_time((applib_time_struct *)t)
#define   YXAPP_SMS_COMMAND_NUM        3
#define   PROTOCOL_VER      '1'
#define   YXAPP_WATCH_VERSION  "1.0"      //soft version number
#define   CUSTOMER_ID       0x0001        //user id,server give you

//cmwap http协议样本
//a是要打印数据的BUFFER,b:服务器文件路径,c:数据包nContent-Length:长度,d:数据包内容,端口只能访问80,h:主机域名
#define MAKE_HTTP_WAP_HEAD_POST(a,b,h,c,d)  sprintf(a, "POST %s HTTP/1.1\r\nHost: 10.0.0.172\r\nUser-Agent: MAUI_WAP_Browser\r\nX-Online-Host:%s\r\nAccept: */*\r\nAccept-Charset: utf-8, utf-16, iso-8859-1, iso-10646-ucs-2, GB2312, windows-1252, us-ascii\r\nAccept-Language: zh-tw, zh-cn, en\r\nContent-Type:plication/x-www-form-urlencoded; charset=utf-8\r\nContent-Length: %d\r\n\%s\r\n\r\n",b,h,c,d);

//此为获取YX_DOMAIN_NAME_DEFAULT网页内容,a为BUFFER,B为访问主机的目录文件,h:主机域名
#define MAKE_HTTP_WAP_HEAD_GET(a,b,h)  sprintf(a, "GET %s HTTP/1.1\r\nHost: 10.0.0.172\r\nUser-Agent: MAUI_WAP_Browser\r\nX-Online-Host: %s\r\nAccept: */*\r\nAccept-Charset: utf-8, utf-16, iso-8859-1, iso-10646-ucs-2, GB2312, windows-1252, us-ascii\r\nAccept-Language: zh-tw, zh-cn, en\r\nConnection: Keep-Alive\r\n\r\n\r\n",b,h);

//cmnet http协议样本,YX_DOMAIN_NAME_DEFAULT网页内容,a为BUFFER,B为访问主机的目录文件,h:主机域名
#define MAKE_HTTP_NET_HEAD_GET(a,b,h)  sprintf(a, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: MAUI_WAP_Browser\r\nAccept: */*\r\nAccept-Charset: utf-8, utf-16, iso-8859-1, iso-10646-ucs-2, GB2312, windows-1252, us-ascii\r\nAccept-Language: zh-tw, zh-cn, en\r\nConnection: Keep-Alive\r\n\r\n\r\n",b,h);

//a是要打印数据的BUFFER,b:服务器文件路径,c:数据包nContent-Length:长度,d:数据包内容,端口只能访问80,h:主机域名
#define MAKE_HTTP_NET_HEAD_POST(a,b,h,c,d)  sprintf(a, "POST %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: MAUI_WAP_Browser\r\nAccept: */*\r\nAccept-Charset: utf-8, utf-16, iso-8859-1, iso-10646-ucs-2, GB2312, windows-1252, us-ascii\r\nAccept-Language: zh-tw, zh-cn, en\r\nContent-Type:plication/x-www-form-urlencoded; charset=utf-8\r\nContent-Length: %d\r\n\%s\r\n\r\n",b,h,c,d);

#define MG_ADD_KEYDATA_STRING(a,k,d) sprintf(a,"%s+%s=%s\r\n\r\n",a,k,d);
#define MG_ADD_KEYDATA_VALUE(a,k,d) sprintf(a,"%s+%s=%d\r\n\r\n",a,k,d);

#define MG_ADD_KEYDATA_HEAD(a,k)   sprintf(a,"%s+%s=",a,k);
#define MG_ADD_KEYDATA_DATA(a,d)   strcat(a,d);
#define MG_ADD_KEYDATA_TAIL(a)     strcat(a,"\r\n\r\n");



////////////////////////////////////////////////////////////Global vars///////////////////////////////////////////////////////////////////
//采用IK通讯协议
static YXPROTOCOLPARAM yxProtocolPkg;
//本协议总体格式:长度(4B)+版本(1B)+数据种类(1B,'1'为文本,'2'为语音,'3'为图片,'4'为AGPS星历数据,'5'为过去包数据)+ID(5B)+IMEI(15B)+DATA(文本指令或语音数据或图片数据)+CRC(2B)
//文本指令采用"+CMD=\r\n"格式,文字编码采用UTF8格式,结束符为:&#,
////////////////////////////////////////////////////////////SMS data proc///////////////////////////////////////////////////////////////////

//返回值说明:0:不拦截此短信,1:拦截此短信,本样列以儿童机为例,所有短信都拦截,都认为是命令,即都返回1
char YxAppCustomerSmsContentFilter(U8* number,U8 num_len,U8* content,U16 content_len,char encoding,MYTIME *timeStamp)
{
extern void YxAppSendSmsIntenal(S8* number,char gsmCode,U16* content);//ucs2
	unsigned char sms_temp[140]={0};

#if(YX_IS_TEST_VERSION!=0)
	U8     bufft[40];
#endif

#if(YX_IS_TEST_VERSION!=0)
	sprintf((char*)bufft,"New sms:%d,%d,%d,%d:%d:%d:NUM:%d\r\n",content[0],content[1],content[2],content[3],content_len,encoding,num_len);
	YxAppTestUartSendData(bufft,strlen((char*)bufft));
#endif



	YxAppSmsCheckDateTime(timeStamp);
	if((encoding!=0 && encoding!=8)||(num_len==0)||(number==NULL))//only proc english,gsm ascii
		return 1;
	else if(encoding==8)//SRV_SMS_DCS_UCS2
	{
		if((strstr((char*)number,"10086")!=NULL)||(strstr((char*)number,"10010")!=NULL))
		{

			U16  i = 0;
			char isYeSms = 0;
			while(i<content_len)
			{
				if((i+4<content_len)&&(content[i]==0x59&&content[i+1]==0x4F&&content[i+2]==0x9D&&content[i+3]==0x98))//余额
				{
					isYeSms = 1;
					break;
				}
				i += 2;
			}
			if(isYeSms)
			{
				//YxAppSmsSetYeBufferContent(content,content_len);
				//得到了话费查询的返回短信
				unsigned char i=0;
				while(1)
				{
					sms_temp[i] = content[i];
					i++;
					if(i>135)break;
					if(i>=content_len)break;
				}
				{
					U16 *qp;
					qp = (U16 *)sms_temp;
					SmsSend_to_phone(qp);
				}
			}
		}
		return 1;
	}
	else
	{

	}
	return 1;//is command sms,don't display
}


////////////////////////////////////////////////////////////Call call cb///////////////////////////////////////////////////////////////////
extern void CALL_incoming(void);
extern U8 mmiKeyPadIsExistPhoneNumber(char *u8PhoneNumber);
extern void mmi_ucm_incoming_call_sendkey(void);


kal_bool YxAppCallNumberIsAllowed(char callType,S8 *number)//callType:1:来电.号码为ASCII码,返回TRUE:允许,FASE:禁止
{
	YXMONITORPARAM    *numList = NULL;
	U8                index = 0,i = 0;
	
#if(YX_IS_TEST_VERSION!=0)
	char              logBuf[51];
	sprintf(logBuf,"calls:%s,t:%d\r\n",number,callType);
	YxAppTestUartSendData((U8*)logBuf,strlen(logBuf));
#endif

#if 0
	if( refused_to_stranger_calls_status_flag == REFUSED_TO_STRANGER_CALLS_STATUS_ON )
	{
		unsigned char i=0;
		
		if( strstr(number,"13530514107") == NULL );
		else i=1;

		if(i)
		{
			CALL_incoming();

			if( call_automatic_answering_status_flag == CALL_AUTOMATIC_ANSWERING_STATUS_ON )
			{
				//自动接听
				StartTimer(CALL_TIMER,5000,CALL_incoming_connect);
			}
			
			return KAL_TRUE;
		}
		else
		{
			//陌生人
			return KAL_FALSE;
		}
	}
	else
	{
		//来电直接接通
		CALL_incoming();
		
		if( call_automatic_answering_status_flag == CALL_AUTOMATIC_ANSWERING_STATUS_ON )
		{
			//自动接听
			StartTimer(CALL_TIMER,5000,CALL_incoming_connect);
		}
		return KAL_TRUE;
	}
#else
	kal_prompt_trace(MOD_YXAPP, "YxAppCallNumberIsAllowed : %s\n", number);

	if (strlen(number) <= 1) { //来电显示,没有开通
		StartTimer(CALL_TIMER, 10000, mmi_ucm_incoming_call_sendkey);
		return KAL_TRUE;
	} else if (mmiKeyPadIsExistPhoneNumber(number)) {
		StartTimer(CALL_TIMER, 10000, mmi_ucm_incoming_call_sendkey);
		return KAL_TRUE;
	} else {
		return KAL_FALSE;
	}
#endif
}

void YxAppEndCallCb(void)//when call is released,it calls this cb
{
	kal_prompt_trace(MOD_YXAPP, "YxAppEndCallCb\n");
	YxAppCallEndProc();
}

////////////////////////////////////////////////////////////gprs data proc///////////////////////////////////////////////////////////////////

static void YxProtocolFreeRes(void)
{
	yxProtocolPkg.totalLen = 0;
	yxProtocolPkg.sendOk = 0;
	yxProtocolPkg.dataLen = 0;
}

static kal_uint16 YxProtocolCheckSum(kal_uint8 *buf,kal_uint16 len)
{
#define   MOD_ADLER   55171
	kal_uint32     a = 7,b = 0;
	kal_uint16     i = 0,checksum = 0;
	len -= 2;
	while(i<len)
	{
		a = (a + buf[i]+13) % MOD_ADLER;
		b = (b + a + 3) % MOD_ADLER;
		i++;
	}
	a = (b<<16) | a;
	checksum = (kal_uint16)(a & 0x0000FFFF);
	if(checksum==0xFFFF)
		checksum = (kal_uint16)(a >> 16);
	return checksum;
}

#if 0
U16 YxAppBuildTxtCmdGpsUploadPackage(char savepkg,U8 *sendBuf,U16 maxLen) //定位包
{
	YXSYSTEMPARAM     *setParam = YxAppGetSystemParam(0);
	YXLOGPARAM        *logParam = NULL;
	MYTIME            oldtime;
	S8   *tempStr = (S8*)(sendBuf+4);
#ifdef __MMI_WLAN_FEATURES__
	WLANMACPARAM      *wlanP = NULL;
#endif
	yxapp_cell_info_struct *cellIfr = NULL;
	U16  i = 0,checkSum = 0,j = 0;
	GetDateTime(&oldtime);
	if(savepkg==1)
	{
		if(yxProtocolPkg.sendOk==1)
			return  0;
	}
	oldtime.nYear -= 2000;
	sprintf(tempStr,"%c1%05d%s+IKPS=DT:%02d%02d%02d%02d%02d,SOS:%d\r\n",PROTOCOL_VER,CUSTOMER_ID,setParam->imei,oldtime.nYear,oldtime.nMonth,oldtime.nDay,oldtime.nHour,oldtime.nMin,yxProtocolPkg.isSos);
	j = (U16)strlen(tempStr);
	tempStr += j;
	//lbs
	strcpy(tempStr,"+IKLBS=");
	j = (U16)strlen(tempStr);
	tempStr += j;
	i = 0;
#if 0//def WIN32
	sprintf(tempStr,"MCC:%d,MNC:%d,LAC:%d,CELLID:%d,RF:%d|",460,1,27374,78998,-87);
	j = (U16)strlen(tempStr);
	tempStr += j;
	sprintf(tempStr,"MCC:%d,MNC:%d,LAC:%d,CELLID:%d,RF:%d\r\n",460,0,27374,78998,-87);
	j = (U16)strlen(tempStr);
	tempStr += j;
#else
	while(i<YX_APP_MAX_CELL_INFOR_NUM)
	{
		cellIfr = YxAppLbsGetData((char)i);
		if(cellIfr)
		{
			if(YxAppLbsGetData((char)(i+1)))
				sprintf(tempStr,"MCC:%d,MNC:%d,LAC:%d,CELLID:%d,RF:%d|",cellIfr->mcc,cellIfr->mnc,cellIfr->lac,cellIfr->ci,-1*cellIfr->rxlev);
			else
				sprintf(tempStr,"MCC:%d,MNC:%d,LAC:%d,CELLID:%d,RF:%d",cellIfr->mcc,cellIfr->mnc,cellIfr->lac,cellIfr->ci,-1*cellIfr->rxlev);
			j = (U16)strlen(tempStr);
			tempStr += j;
		}
		else
			break;
		i++;
	}
#endif
	strcpy(tempStr,"\r\n");
	tempStr += 2;
	//gps
	if(GpsGetLongitudeV()==0.0||GpsGetLatitudeV()==0.0)
		strcpy(tempStr,"+IKGPS=\r\n");
	else
		sprintf(tempStr,"+IKGPS=LON:%.6f,LAT:%.6f,SPD:%.6f,ANG:%.4f\r\n",GpsGetLongitudeV(),GpsGetLatitudeV(),GpsGetSpeedV(),GpsGetAngleV());
	i = (U16)strlen(tempStr);
	tempStr += i;
	//wifi
#ifdef __MMI_WLAN_FEATURES__
	i = 0;
	strcpy(tempStr,"+IKWIFI=");
	j = (U16)strlen(tempStr);
	tempStr += j;
#if 0//def WIN32
	sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,TP-LINK|",0xeF,0xAB,0xAC,0x12,0x87,0x98,-90);
	i = (U16)strlen(tempStr);
	tempStr += i;
	sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,TP-LINK|",0xeF,0xAB,0xAC,0x12,0x87,0x98,-90);
	i = (U16)strlen(tempStr);
	tempStr += i;
	sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,TP-LINK|",0xeF,0xAB,0xAC,0x12,0x87,0x98,-90);
	i = (U16)strlen(tempStr);
	tempStr += i;
	sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,TP-LINK|",0xeF,0xAB,0xAC,0x12,0x87,0x98,-90);
	i = (U16)strlen(tempStr);
	tempStr += i;
	sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,TP-LINK|",0xeF,0xAB,0xAC,0x12,0x87,0x98,-90);
	i = (U16)strlen(tempStr);
	tempStr += i;
	sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,TP-LINK|",0xeF,0xAB,0xAC,0x12,0x87,0x98,-90);
	i = (U16)strlen(tempStr);
	tempStr += i;
	sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,TP-LINK",0xeF,0xAB,0xAC,0x12,0x87,0x98,-90);
	i = (U16)strlen(tempStr);
	tempStr += i;
#else
	while(i<8)
	{
		wlanP = YxAppGetWlanScanInfor((U8)(i++));
		if(wlanP)
		{
			if(YxAppGetWlanScanInfor((U8)i)==NULL)
				sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,TP-LINK",wlanP->bssid[0],wlanP->bssid[1],wlanP->bssid[2],wlanP->bssid[3],wlanP->bssid[4],wlanP->bssid[5],wlanP->rssi);
			else
				sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,TP-LINK|",wlanP->bssid[0],wlanP->bssid[1],wlanP->bssid[2],wlanP->bssid[3],wlanP->bssid[4],wlanP->bssid[5],wlanP->rssi);
			j = (U16)strlen(tempStr);
			tempStr += j;
		}
		else
			break;
	}
#endif
	strcpy(tempStr,"\r\n");
	tempStr += 2;
#endif
	//bt
#ifdef __MMI_BT_SUPPORT__
	i = 0;
	strcpy(tempStr,"+IKBT=");
	j = (U16)strlen(tempStr);
	tempStr += j;
#if 0//def WIN32
	sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,BT|",0xeF,0xAB,0xAC,0x12,0x87,0x98,-10);
	i = (U16)strlen(tempStr);
	tempStr += i;
	sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,BT|",0xeC,0xAB,0xAC,0x12,0x87,0x98,-10);
	i = (U16)strlen(tempStr);
	tempStr += i;
	sprintf(tempStr,"%02x:%02x:%02x:%02x:%02x:%02x,%d,BT",0xeD,0xAB,0xAC,0x12,0x87,0x98,-10);
	i = (U16)strlen(tempStr);
	tempStr += i;
#else
	while(i<YXAPP_BT_MAX_SEARCH_DEVICES)
	{
		if(YxAppBtGetDevicesAddress((U8)(i++),tempStr)==1)
		{
			if(YxAppBtGetDevicesAddress((U8)i,NULL)==1)
				strcat(tempStr,"|");
			j = (U16)strlen(tempStr);
			tempStr += j;
		}
		else
			break;
	}
#endif
	strcpy(tempStr,"\r\n");
	tempStr += 2;
#endif
	i = 0;
	strcpy(tempStr,"+IKLOG=");//log
	j = (U16)strlen(tempStr);
	tempStr += j;
	while(i<YX_LOGS_MAX_NUM)
	{
		logParam = YxAppGetLogByIndex((char)i);
		if((logParam)&&(logParam->kind==LOG_EFENCE))
		{
			if(YxAppGetLogByIndex((char)(i+1))==NULL)
				sprintf(tempStr,"K:%c,DT:%02d%02d%02d%02d,ACT:%c,V:%04x",logParam->kind,(logParam->date>>8),logParam->date&0x00FF,logParam->time>>8,logParam->time&0x00FF,logParam->action,logParam->value);
			else
				sprintf(tempStr,"K:%c,DT:%02d%02d%02d%02d,ACT:%c,V:%04x|",logParam->kind,(logParam->date>>8),logParam->date&0x00FF,logParam->time>>8,logParam->time&0x00FF,logParam->action,logParam->value);
			j = (U16)strlen(tempStr);
			tempStr += j;
		}
		i++;
	}
	strcpy(tempStr,"\r\n");
	tempStr += 2;
	//end
	i = (U16)(strlen((S8*)(sendBuf+4))+6);
	sendBuf[0] = (U8)(i&0x00FF);
	sendBuf[1] = (U8)(i>>8);
	sendBuf[2] = 0;
	sendBuf[3] = 0;
	checkSum = YxProtocolCheckSum(sendBuf,i);
	sendBuf[i-2] = (U8)(checkSum&0x00FF);
	sendBuf[i-1] = (U8)(checkSum>>8);
	return i;
}
#endif

U16 YxAppBuildTxtCmdGpsUploadPackage(char savepkg,U8 *sendBuf,U16 maxLen) { //定位包
	return 0;
}
#if 0

U16 YxAppBuildTxtCmdHeartUploadPackage(U8 *sendBuf,U16 maxLen)//心跳包,准备要上传的数据
{
	YXSYSTEMPARAM     *setParam = YxAppGetSystemParam(0);
	YXMONITORPARAM    *moniInfor = NULL;
	YXLOGPARAM        *logParam = NULL;
	MYTIME            oldtime;
#if (YX_GPS_USE_AGPS==1)
	U8                agpsTimeBuf[9];
#endif
	S8   *tempStr = (S8*)(sendBuf+4);
	U16  i = 0,checkSum = 0,j = 0;
	GetDateTime(&oldtime);
	//信号,电量百分比(0-100),情景模式('1':一般,'2':静音,'3':会议,'4':户外)
	oldtime.nYear -= 2000;
	i = YxAppGetSignalLevelInPercent();
	sprintf(tempStr,"%c1%05d%s+IKSYS=RF:%02d,BAT:%02d,PRO:%c,GPRS:%05d,GPS:%05d,DT:%02d%02d%02d%02d%02d\r\n",PROTOCOL_VER,CUSTOMER_ID,setParam->imei,i,YxAppBatteryLevel(),YxAppGetCurrentProfile(),setParam->uploadTick,setParam->gpsTimes,oldtime.nYear,oldtime.nMonth,oldtime.nDay,oldtime.nHour,oldtime.nMin);
	j = (U16)strlen(tempStr);
	tempStr += j;
	if(yxProtocolPkg.isFirst)//上传自检包
	{
		S8   *serverName = NULL;
#ifdef __MMI_BT_SUPPORT__
		U8   *btMac = NULL,*btName = NULL;
#endif
#ifdef __MMI_WLAN_FEATURES__
		U8   *wMac = NULL;
#endif
#ifdef __MMI_BT_SUPPORT__
		YxAppGetLocalBtNameMac(&btName,&btMac);
#endif
		serverName = YxAppGetServerName(&i);
#ifdef __MMI_WLAN_FEATURES__
		wMac = YxAppSetAndGetLocalMac(NULL);
#endif
#if defined(__MMI_BT_SUPPORT__)&&defined(__MMI_WLAN_FEATURES__)
		sprintf(tempStr,"+IKVER=V:%s,SRV:%s,PORT:%d,BMAC:%s,BNAME:%s,WMAC:%02x:%02x:%02x:%02x:%02x:%02x\r\n",YXAPP_WATCH_VERSION,serverName,i,(S8*)btMac,(S8*)btName,wMac[0],wMac[1],wMac[2],wMac[3],wMac[4],wMac[5]);
#elif defined(__MMI_BT_SUPPORT__)
		sprintf(tempStr,"+IKVER=V:%s,SRV:%s,PORT:%d,BMAC:%s,BNAME:%s,WMAC:%02x:%02x:%02x:%02x:%02x:%02x\r\n",YXAPP_WATCH_VERSION,serverName,i,(S8*)btMac,(S8*)btName,0,0,0,0,0,0);
#elif defined(__MMI_WLAN_FEATURES__)
		sprintf(tempStr,"+IKVER=V:%s,SRV:%s,PORT:%d,BMAC:00:00:00:00:00:00,BNAME:BT,WMAC:%02x:%02x:%02x:%02x:%02x:%02x\r\n",YXAPP_WATCH_VERSION,serverName,i,wMac[0],wMac[1],wMac[2],wMac[3],wMac[4],wMac[5]);
#else
		sprintf(tempStr,"+IKVER=V:%s,SRV:%s,PORT:%d,BMAC:00:00:00:00:00:00,BNAME:BT,WMAC:%02x:%02x:%02x:%02x:%02x:%02x\r\n",YXAPP_WATCH_VERSION,serverName,i,0,0,0,0,0,0);
#endif
		yxProtocolPkg.isFirst = 2;
		j = (U16)strlen(tempStr);
		tempStr += j;
	}
	i = 0;
	strcpy(tempStr,"+IKLOG=");//log
	j = (U16)strlen(tempStr);
	tempStr += j;
	while(i<YX_LOGS_MAX_NUM)
	{
		logParam = YxAppGetLogByIndex((char)i);
		if(logParam&&(logParam->kind!=LOG_EFENCE))
		{
			if(YxAppGetLogByIndex((char)(i+1))==NULL)
				sprintf(tempStr,"K:%c,DT:%02d%02d%02d%02d,ACT:%c,V:%04x",logParam->kind,(logParam->date>>8),logParam->date&0x00FF,logParam->time>>8,logParam->time&0x00FF,logParam->action,logParam->value);
			else
				sprintf(tempStr,"K:%c,DT:%02d%02d%02d%02d,ACT:%c,V:%04x|",logParam->kind,(logParam->date>>8),logParam->date&0x00FF,logParam->time>>8,logParam->time&0x00FF,logParam->action,logParam->value);
			j = (U16)strlen(tempStr);
			tempStr += j;
		}
		i++;
	}
	strcpy(tempStr,"\r\n");
	tempStr += 2;
	moniInfor = YxAppGetMonitorList(0,0);
	if(moniInfor->number[0]==0)//获取监护号码列表
	{
		strcpy(tempStr,"+IKQXHM=GET\r\n");
		j = (U16)strlen(tempStr);
		tempStr += j;
	}
	moniInfor = YxAppGetFirewallItemBy(0);
	if(moniInfor->number[0]==0)//获取白名单号码列表
	{
		strcpy(tempStr,"+IKBMD=GET\r\n");
		j = (U16)strlen(tempStr);
		tempStr += j;
	}
	if(YxAppGetEfenceNumber()==0)//获取电子围栏列表
	{
		strcpy(tempStr,"+IKDZWL=GET\r\n");
		j = (U16)strlen(tempStr);
		tempStr += j;
	}
	if(YxAppFirewallReadWriteStatus(3)==0)//获取防火状态
	{
		strcpy(tempStr,"+IKFHQ=GET\r\n");
		j = (U16)strlen(tempStr);
		tempStr += j;
	}
	if(YxAppFirewallReadWriteTime(1,NULL,NULL)==0)
	{
		strcpy(tempStr,"+IKMDR=GET\r\n");//获取免打扰时间
		j = (U16)strlen(tempStr);
		tempStr += j;
	}
	if(YxAppSystemDtIsDefault()==1)
	{
		strcpy(tempStr,"+IKDT=GET\r\n");//服务器授时
		j = (U16)strlen(tempStr);
		tempStr += j;
	}
	if(YxAppGetActiveAlarmNumber()==0)//get alarm
	{
		strcpy(tempStr,"+IKALM=GET\r\n");
		j = (U16)strlen(tempStr);
		tempStr += j;
	}
#if (YX_GPS_USE_AGPS==1)
	//agps
	strcpy(tempStr,"+IKAGPS=GET:");
	j = (U16)strlen(tempStr);
	tempStr += j;
	if(YxAgpsGetDataTime(agpsTimeBuf)==1)
	{
		agpsTimeBuf[8] = 0;
		strcpy(tempStr,(S8*)agpsTimeBuf);
		j = (U16)strlen(tempStr);
		tempStr += j;
	}
	strcpy(tempStr,"\r\n");
#endif
	//pedo
	//sleep
	//end
	i = (U16)(strlen((S8*)(sendBuf+4))+6);
	sendBuf[0] = (U8)(i&0x00FF);
	sendBuf[1] = (U8)(i>>8);
	sendBuf[2] = 0;
	sendBuf[3] = 0;
	checkSum = YxProtocolCheckSum(sendBuf,i);
	sendBuf[i-2] = (U8)(checkSum&0x00FF);
	sendBuf[i-1] = (U8)(checkSum>>8);
	return i;
}
#else

#if 1

U8 get_heart_packet_response_header(U8 *response) {
	return response[0];
}

U8 get_heart_packet_response_cmd(U8 *response) {
	return response[HEART_CMD_INDEX];
}

extern U16 apollo_flag;
extern U16 u16WatchStatusFlag;
extern YXAPPSOCKCONTEXT yxNetContext_ptr;

extern kal_int8 soc_close(kal_int8 s);
#endif


extern int apollo_init_aes_ctx(void) ;
extern void apollo_encrypt(unsigned char input[], unsigned char output[]) ;
extern void apollo_decrypt(unsigned char input[], unsigned char output[]);
extern APDS9901_STATUS flag_Prox_data;

U16 YxAppBuildTxtCmdHeartUploadPackage(U8 *sendBuf,U16 maxLen) {
	YXSYSTEMPARAM     *setParam = YxAppGetSystemParam(0);
//	YXMONITORPARAM    *moniInfor = NULL;
//	YXLOGPARAM        *logParam = NULL;

	char i = 0;
	unsigned char buf[32] = { //standard heart package
		'H', 0x00, 0x86, 0x18, 0x87, 0x41, 0x87, 0x42,
		0x90, 0x15, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, '%', '%', '%', '%', '%', '%', 
	};

	buf[16] = srv_nw_info_get_signal_strength_in_percentage(MMI_SIM1);
	buf[17] = flag_Prox_data<<4 & 0xF0 | 0x0F&srv_charbat_get_battery_level();
	buf[18] = u16WatchStatusFlag % 0xFF;
	buf[19] = u16WatchStatusFlag / 0xFF;
	
	//(*(unsigned int*) (buf+HEART_APOLOO_ID_START)) = (unsigned int)APOLLO_ID; //get apollo_id
#if 0
	*(buf+HEART_RESPONSE_CMD_INDEX) = heart_info.cmd;

	if (heart_info.cmd  != COMMAND_NULL) {
		if (heart_info.cmd == TELEPHONEFARE_SELECT) {
			*(buf+HEART_RESPONSE_RES_START) = *tele_fare;
			*(buf+HEART_RESPONSE_RES_START+1) = *(tele_fare+1);
			*(buf+HEART_RESPONSE_RES_START+2) = *(tele_fare+2);
		}

		for (i = 0;i < TELE_NUMBER_LENGTH;i += 2) {
			*(buf+HEART_RESPONSE_TEL_START+ i / 2) = STRINGTOHEX(heart_info.tele_number[i], heart_info.tele_number[i+1]);
		}
	}
	
#endif
	//apollo_init_aes_ctx();
	//apollo_encrypt(buf, buf);
	//apollo_encrypt(buf+16, buf+16);
	

	//S8   *tempStr = (S8*)(sendBuf+1);
	//sendBuf[0] = 'H';

	for (i = 0;i < 32;i ++) {
		sendBuf[i] = buf[i];
	}
	return i;
}

#endif

extern U8 apollo_key_buf[12];
extern U8 recv_apollo_key[20];
extern U8 apollo_key_ready; //0 download key, 1 key ready
extern U8 sos_key; 
extern U8 diedao_key;
extern U32 step_num;
extern U32 sleep_quality;
extern U8 blockDataFlag; //BLE data recv flag


int dateDiff(struct SampleDate mindate,struct SampleDate maxdate)  
{  
    int days=0,j,flag;  
    const int primeMonth[][12]={{31,28,31,30,31,30,31,31,30,31,30,31},{31,29,31,30,31,30,31,31,30,31,30,31}};     
    /************************************************************************/  
    /*        交换两个日期函数,将小的日期给mindate,将大的日期给maxdate     */  
    /************************************************************************/  
    struct SampleDate tmp;  
    if ((mindate.year>maxdate.year)|| (mindate.year==maxdate.year&&mindate.month>maxdate.month)||(mindate.year==maxdate.year&&mindate.month==maxdate.month&&mindate.day>maxdate.day))  
    {  
        tmp=mindate;  
        mindate=maxdate;  
        maxdate=tmp;  
    }   
    /************************************************************************/  
    /*  从mindate.year开始累加到maxdate.year                                */  
    /************************************************************************/  
    for(j=mindate.year;j<maxdate.year;++j)  
        days+=isPrime(j)?366:365;  
      
    //如果maxdate.year是闰年,则flag=1,后面调用primeMonth[1][12]  
    flag=isPrime(maxdate.year);  
    //加上maxdate.month到1月的天数  
    for (j=1;j<maxdate.month;j++)  
        days+=primeMonth[flag][j-1];  
  
    //减去mindate.month到1月的天数  
  
    flag=isPrime(maxdate.year);  
    for (j=1;j<mindate.month;j++)  
      days-=primeMonth[flag][j-1];  
    days=days+maxdate.day-mindate.day;  
    return days;  
}   

//kal_uint8 oldMin = 0;
//kal_uint8 u8Sec = 0;



U32 getSecondOff(void) {
	struct SampleDate beginTime = {2010, 1, 1};
	struct SampleDate nowTime = {0};
	applib_time_struct   dt;
	rtc_format_struct	Rtc;
	int diff = 0;

	mmiClockGetDateTime(&Rtc);
	mmiClockRtcToApplib(&Rtc, &dt);
	//applib_dt_get_date_time(&dt);

	dt.nYear = (dt.nYear > 2000 ? dt.nYear : (2000 + dt.nYear % 100) );

	nowTime.day = dt.nDay;
	nowTime.month = dt.nMonth;
	nowTime.year = dt.nYear;
#if 0
	u8Sec += 5;
	if (oldMin != dt.nMin) {
		oldMin = dt.nMin;
		u8Sec = 0;
	}
#endif
	diff = dateDiff(beginTime, nowTime);
	return (diff*24*60*60 + dt.nHour*60*60 + dt.nMin*60 + dt.nSec);
}

void Apollo_U32_datacpy(U8 *send, U32 num) {
	U8* pNum = (U8*)(&num);
	int i = 0;

	for (i = 0;i < 4;i ++) {
		send[i] = (U8)*(pNum+i);
	}
}

void Apollo_set_burnkey(U8 *sendBuf, U8 *key) {
	int i = 0;
	for (i = 0;i < 8;i ++) {
		sendBuf[i+1] = key[i];
	}
}

S32 Apollo_send_burnkey_package(U8 *sendBuf, U16 maxLen) {
	int i = 0;
	U8 key_request[8] = { 
		'k', 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	};

	for (i = 0;i < 8;i ++) {
		sendBuf[i] = key_request[i];
	}
	return 8;
}

extern U8 YxAppGetSignalLevelInPercent(void);
extern U16 u16DiedaoDeviceWorkPattern; //Diedao Device
extern U8 u8DiedaoDeviceBattery;		//Diedao Device


S32 Apollo_send_heart_package(U8 *sendBuf, U16 maxLen) {
	//Apollo_set_burnkey();
	int i = 0;
	U32 timeDiff = getSecondOff();
	Apollo_set_burnkey(heartHeader, apollo_key_buf); //set key

	heartHeader[9] = srv_charbat_get_battery_level(); //set battery

	heartHeader[10] = (diedao_key & 0x0F) | (sos_key & 0x0F << 4); //
	heartHeader[11] = u16WatchStatusFlag & 0x00FF;
	heartHeader[12] = (u16WatchStatusFlag & 0xFF00) >> 8;

	Apollo_U32_datacpy(heartHeader+13, step_num); //set step num
	//Apollo_U32_datacpy(heartHeader+17, sleep_quality); //set sleep quality
	heartHeader[17] = u16DiedaoDeviceWorkPattern & 0x00FF;
	heartHeader[18] = (u16DiedaoDeviceWorkPattern & 0xFF00) >> 8;
	heartHeader[19] = u8DiedaoDeviceBattery;
	heartHeader[20] = srv_nw_info_get_signal_strength_in_percentage(MMI_SIM1);
	Apollo_U32_datacpy(heartHeader+24, timeDiff);

	heartHeader[28] = blockDataFlag;
	//heartHeader[28] = 0x01; //init time
	blockDataFlag = 0x0;

	for (i = 0;i < APOLLO_UPLOAD_HEART_LENGTH; i ++) {
		sendBuf[i] = heartHeader[i];
	}

	return APOLLO_UPLOAD_HEART_LENGTH;
	//memccpy(void *, const void *, int, size_t)
}

S32 Apollo_send_record_data(U8 *sendBuf, U16 maxLen) {
#ifdef ENABLE_FUNC_RECORDER
	return Apollo_read_recorder_file(sendBuf);
#else
	return ApolloBlueToothRecv(sendBuf);
#endif
}

S32 Apollo_send_request_data(U8 *sendBuf, U16 maxLen) {
	int i = 0;
	U16  flag = YxAppGetRunFlag();

	for (i = 0;i < APOLLO_HEART_PACKAGE_LENGTH;i ++) {
		sendBuf[i] = requestHeader[i];
	}

	if (flag & APOLLO_RUNKIND_VOICE_DOWNLOAD) {
		sendBuf[13] = 0x60;
	} else if (flag & APOLLO_RUNKIND_AGPS_DOWNLOAD) {
		sendBuf[13] = 0x61;
	}

	Apollo_set_burnkey(sendBuf, apollo_key_buf);

	return 16;
}

U8 LatLongValue[10] = {0};
U8 LbsWifiValue[64] = {0};
U8 PosType = 0x00;
extern int JW_height;
#if (YX_GPS_USE_AGPS == 1)
extern U8 u8NeedInitAgpsDataFlag;
extern YXGPSPARAMSTR  yxGpsParam;
#endif

#define POS_TYPE 9


S32 Apollo_send_gps_data(U8 *sendBuf, U16 maxLen) {
	int i = 0;
	U16 flag = YxAppGetRunFlag();
	U32 timeDiff = getSecondOff();

	
	kal_prompt_trace(MOD_YXAPP,"PosType : %x\n", PosType);
	Apollo_set_burnkey(gpsHeader, apollo_key_buf);
	
	if (u8NeedInitAgpsDataFlag) { //Download AGPS Data
		PosType |= 0x10;
	}
	
	gpsHeader[POS_TYPE] = 0x0;
	if (PosType & 0x01) {
		gpsHeader[POS_TYPE] = PosType;		
		PosType = 0x0;
		for (i = 0;i < 10;i ++) {
			gpsHeader[POS_TYPE+i +1] = LatLongValue[i];
		}
		kal_prompt_trace(MOD_YXAPP," ffff JW_height:%d\n", JW_height);
		Apollo_U32_datacpy(gpsHeader+24, JW_height);
		Apollo_U32_datacpy(gpsHeader+20, timeDiff);
		//kal_prompt_trace(MOD_YXAPP," %x %x %x %x\n", gpsHeader[24], gpsHeader[25], gpsHeader[26], sendBuf[27]);
		for (i = 0;i < 32;i ++) {
			sendBuf[i] = gpsHeader[i];
		}

		flag &= (~YX_RUNKIND_OWNER_GPS);
		ApolloAppSetRunFlag(flag);
		
		return 32;
	} 
	if (PosType == 0x0) return 0;
	gpsHeader[POS_TYPE] = PosType;
#if 0
	if (PosType & 0x02) {
		gpsHeader[POS_TYPE] |= 0x02;
	} 
	if (PosType & 0x04) {
		gpsHeader[POS_TYPE] |= 0x04;
	}
#endif
	for (i = 0;i < 34;i ++) {
		gpsHeader[POS_TYPE+i +1] = LbsWifiValue[i];
	}
	Apollo_U32_datacpy(gpsHeader+44, timeDiff);
	for(i = 0;i < APOLLO_UPLOAD_GPS_LENGTH;i ++) {
		sendBuf[i] = gpsHeader[i];
	}
	kal_prompt_trace(MOD_YXAPP," id1:%x, id2:%x, PosType : %x\n",sendBuf[9], sendBuf[10], PosType);
	//PosType = 0x0;
	flag &= (~YX_RUNKIND_OWNER_GPS);
	ApolloAppSetRunFlag(flag);
	
	return APOLLO_UPLOAD_GPS_LENGTH;
}

#if 0
static S32 YxAppSendUnloadData(U8 *sendBuf,U16 maxLen)
{
	S32   datLength = YxAppGetUnsendDataLength(),i = 0,j = 0,bufLen = 0;
	U8    *tempBuf = sendBuf;
	U16   checkSum = 0;
	YXSYSTEMPARAM     *setParam = YxAppGetSystemParam(0);
	if((datLength<4)||(datLength > YX_UNSEND_PKG_FILE_LEN))
		return 0;
	if(datLength<=maxLen-YX_UNSEND_PKG_HEADER_LEN)  /**对于文件大小<=maxLen-YX_UNSEND_PKG_HEADER_LEN*/
		bufLen = maxLen - YX_UNSEND_PKG_HEADER_LEN;
	else
	{
		tempBuf = yxProtocolPkg.bigBuffer;
		bufLen = YX_UNSEND_PKG_FILE_LEN;
	}
	i = 4;
	sprintf((char*)(tempBuf+4),"%c%c%05d%s",'5',PROTOCOL_VER,CUSTOMER_ID,setParam->imei);
	j = strlen((S8*)(tempBuf+i));
	i += j;
	tempBuf[i++] = (U8)datLength;
	tempBuf[i++] = (U8)(datLength>>8);
	tempBuf[i++] = (U8)(datLength>>16);
	tempBuf[i++] = (U8)(datLength>>24);
	YxAppGetUnsendDataFromFile(tempBuf+i,bufLen,0);
	i += datLength;
	i += 2;//crc
	tempBuf[0] = (U8)(i&0x00FF);
	tempBuf[1] = (U8)(i>>8);
	tempBuf[2] = (U8)(i>>16);
	tempBuf[3] = (U8)(i>>24);
	checkSum = YxProtocolCheckSum(tempBuf,i);
	tempBuf[i-2] = (U8)(checkSum&0x00FF);
	tempBuf[i-1] = (U8)(checkSum>>8);
	return i;
}
#endif
#ifndef APOLLO_VOICE_FILENAME
#define APOLLO_VOICE_FILENAME		L"\\voice.amr"
#endif

UINT Apollo_write_voice_data(PU8 data,UINT size) {
	FS_HANDLE fh = 0;
	U8     path_ascii[64];
	U8     path[64 * 2];
	UINT   wrsize = 0;
	
	//ilm_struct *apollo_ilm = NULL;
	//unsigned char *p = (unsigned char*)construct_local_para(3,TD_CTRL);
	
	memset(path_ascii,0x00,64);
	sprintf((char*)path_ascii,"%c:\\%s",(S8)SRV_FMGR_PUBLIC_DRV,YX_FILE_FOLD_NAME);
	mmi_asc_to_ucs2((CHAR*)path,(CHAR*)path_ascii);
	mmi_ucs2cat((CHAR*)path,(CHAR*)APOLLO_VOICE_FILENAME);
	
	kal_prompt_trace(MOD_YXAPP," aaa path:%s\n", path);
	fh = FS_Open((const U16*)path,FS_CREATE_ALWAYS|FS_READ_WRITE|FS_CACHE_DATA);
	if(fh>=FS_NO_ERROR) {
		
		FS_Write(fh,(void*)data,size,(UINT*)&wrsize);

		FS_Commit(fh);
		FS_Close(fh);
	}
	return wrsize;
}

void Apollo_play_network_data(void) {
	StopTimer(APOLLO_PLAY_RECV_DATA_TIMER);

	YxAppSendMsgToMMIMod(APOLLO_PLAY_RECV_VOICE_DATA, 0, 0);
}

void ApolloSendSerialReady(void) {
	StopTimer(APOLLO_PLAY_RECV_VOICE_DATA);
	YxAppSendMsgToMMIMod(APOLLO_MSG_SERIALPORT_SEND, 0, 0);
}

extern void Uart1_sends(U8 *buff,U16 len);
S32 Apollo_read_voice_packet(U32 *readlen, U32 *readIndex, U8 *readBuffer) {
	S32 n = 0;
	S32 index  = *readIndex;
	U16 len = 0;
	U8 packet_index;
	PU8 voice_data = YxProtocolGetBigBuffer();
	UINT voice_size = *readlen;
	U16 i = 0;
	U8 *buffer = readBuffer;
	apollo_flag = YxAppGetRunFlag();
	
	
	while (1) {
		n = YxAppTcpSockRead(buffer+index,VOICE_PACKET_SIZE - index);

		if (n < 0) {
			*readlen = voice_size;
			*readIndex = index;
			break;
		}
		if (n + index >= VOICE_PACKET_SIZE && buffer[13] + 1 != buffer[29]) {
			len = buffer[30] * 256 + buffer[31];
			packet_index = buffer[13];
			for (i = 0;i < len;i ++) {
				voice_data[packet_index * (VOICE_PACKET_SIZE - VOICE_HEADER_SIZE) + i] = buffer[VOICE_HEADER_SIZE + i];
			}
#if 0 //Update By WangBoJing
			Uart1_sends(buffer+VOICE_HEADER_SIZE, len);
#endif
			index = 0;
			voice_size += len;
		} else {
			index += n;
		}
		kal_prompt_trace(MOD_YXAPP,"read data k:%d , size:%d\n", n, voice_size);
		if (buffer[13] + 1 == buffer[29] && index >= (buffer[30] * 256 + buffer[31])) {
			len = buffer[30] * 256 + buffer[31];
			packet_index = buffer[13];
			for (i = 0;i < len;i ++) {
				voice_data[packet_index * (VOICE_PACKET_SIZE - VOICE_HEADER_SIZE) + i] = buffer[VOICE_HEADER_SIZE + i];
			}
#if 0 //Upoate By WangBoJing
			Uart1_sends(buffer+VOICE_HEADER_SIZE, len);
#endif
			index = 0;
			voice_size += len;

			*readlen = 0;
			*readIndex = 0;
			
			if (apollo_flag & APOLLO_RUNKIND_AGPS_DOWNLOAD) {
				YxAppSaveAgpsData(voice_data, voice_size);
				apollo_flag &= (~APOLLO_RUNKIND_AGPS_DOWNLOAD);
				
				u8NeedInitAgpsDataFlag = 0x0; //download ok and clear flag
				yxGpsParam.yxAgpsDatLen = YxAppGetAgpsData(yxGpsParam.yxAgpsDataBuf,AGPS_MAX_RECEIVE_BUFFER_LEN);
			} else if (apollo_flag & APOLLO_RUNKIND_VOICE_DOWNLOAD) {
				Apollo_write_voice_data(voice_data, voice_size);
				apollo_flag &= (~APOLLO_RUNKIND_VOICE_DOWNLOAD);

				StartTimer(APOLLO_PLAY_RECV_VOICE_DATA, YX_HEART_TICK_UNIT, ApolloSendSerialReady);
				blockDataFlag = 0x01;
			//download complete
			}
			ApolloAppSetRunFlag(apollo_flag);
			break;
		}

	}
	return 0;
}

S32 YxAppSockConnectedCallback(U8 *sendBuf,U16 maxLen)//maxlen:YX_SOCK_BUFFER_LEN,连接服务器成功时的回调函数,可以在里面准备要上传的数据,并上传到服务器上.返回>0:代表要发送数据到服务器,0则不要
{

	U16  flag = YxAppGetRunFlag();
	YxProtocolFreeRes();
	
	if (apollo_key_ready == 0) { //download key
		return Apollo_send_burnkey_package(sendBuf,maxLen);
	} 

	if (flag & APOLLO_RUNKIND_INIT_WATCHDATA) {
		
	} else if (flag & APOLLO_RUNKIND_VOICE_UPLOAD) {
		return Apollo_send_record_data(sendBuf,maxLen);
	} else if ((flag & APOLLO_RUNKIND_VOICE_DOWNLOAD) || (flag & APOLLO_RUNKIND_AGPS_DOWNLOAD)) {
		return Apollo_send_request_data(sendBuf, maxLen);
	} else if (flag & YX_RUNKIND_OWNER_GPS){
		return Apollo_send_gps_data(sendBuf, maxLen);
	} else /* if(flag & YX_RUNKIND_OWNER_HEART) */{
		return Apollo_send_heart_package(sendBuf, maxLen);
	}


}

static U16 YxAPPCommandProc(S8 *cmdStr,U16 length,U8 *resCmd)
{
#define MAX_CMD_LIST_NUM    20
	S8     cmdKind = 0,find = 0;
	U16    i = 0;
	const  S8 *cmdList[MAX_CMD_LIST_NUM]=
	{
		"+IKQXHM=",
		"+IKBMD=",
		"+IKFHQ=",
		"+IKMDR=",
		"+IKSET=",
		"+IKDT=",
		"+IKDZWL=",
		"+IKALM=",
		"+IKRES=",
		"+IKBT=",
		"+IKOFF=",
		"+IKSWT=",
		"+IKDIAL=",
		"+IKSMS=",
		"+IKLCDT=",
		"+IKSCRK=",
		"+IKRPS=",
		"+IKPIC=",
		"+IKSLP=",
		"+IKDTK="
	};
	while(cmdKind<MAX_CMD_LIST_NUM)
	{
		if(strncmp(cmdStr,(char*)cmdList[cmdKind],strlen((char*)cmdList[cmdKind]))==0)
		{
			find = 1;
			break;
		}
		cmdKind++;
	}
	if(find==0)
	{
		i = 0;
		while(i<length)
		{
			if((cmdStr[i]==0x0D)&&(cmdStr[i+1]==0x0A))
			{
				i += 2;
				break;
			}
			i++;
		}
		return i;
	}
	*resCmd = 0;
	switch(cmdKind)
	{
	case 0://+IKQXHM=爸爸&#1232123131231|妈妈&#1231233322\r\n
	case 1://+IKBMD=叔叔&#1232123131231|大伯父&#1231233322\r\n
		{
			char num[YX_APP_CALL_NUMBER_MAX_LENGTH+1],j = 0,k = 0,names[(YX_APP_CALL_NAMES_MAX_LENGTH+1)<<2];
			U16  oldi = 0;
			if(cmdKind==0)
				cmdStr += 8;
			else
				cmdStr += 7;
			while((cmdStr[i]!=0x0D)&&(cmdStr[i+1]!=0x0A))
			{
				j = 0;
				oldi = i;
				while((cmdStr[i]!='&')&&(cmdStr[i+1]!='#'))
				{
					j++;
					i++;
				}
				if(j>0)
					YxAppUTF8ToUincode((U8*)(cmdStr+oldi),j,(U8*)names,(U16)((YX_APP_CALL_NAMES_MAX_LENGTH+1)<<2));
				else
				{
					names[0] = 0;
					names[1] = 0;
				}
				i += 2;
				j = 0;
				while((cmdStr[i]!='|')&&(cmdStr[i]!='\r'))
				{
					num[j++] = cmdStr[i++];
					if(j>=YX_APP_CALL_NUMBER_MAX_LENGTH)
						break;
				}
				num[j++] = 0x00;
				if(cmdKind==0)
					YxAppSetMonitorInfor(k,num,(U8*)names);
				else
					YxAppSetFirewallInfor(k,num,(U8*)names);
				k++;
				if(cmdStr[i]=='|')
					i++;
			}	
			if(cmdKind==0)
			{
				i += 10;
				while(k<YX_MAX_MONITOR_NUM)
				{
					YxAppSetMonitorInfor(k,NULL,NULL);
					k++;
				}
				YxAppSaveMonitorList();
			}
			else
			{
				i += 9;
				while(k<YX_WHITE_NUM_LIST_MAX)
				{
					YxAppSetFirewallInfor(k,NULL,NULL);
					k++;
				}
				YxAppSaveFirewallList();
			}
		}
		return i;
	case 2://+IKFHQ=ON\r\n
		cmdStr += 7;
		if(strncmp((S8*)cmdStr,"ON",2)==0)
		{
			if(YxAppFirewallReadWriteStatus(3)!=1)
			{
				YxAppFirewallReadWriteStatus(1);
				YxAppSaveFirewallList();
			}
		}
		else//off
		{
			if(YxAppFirewallReadWriteStatus(3)!=0)
			{
				YxAppFirewallReadWriteStatus(0);
				YxAppSaveFirewallList();
			}
		}
		while((cmdStr[i]!=0x0D)&&(cmdStr[i+1]!=0x0A))
		{
			i++;
		}
		i += 9;
		return i;
	case 3://+IKMDR=08301150,14301750,00000000\r\n
		{
			U16    sTime[3],eTime[3];
			cmdStr += 7;
			sTime[0] = (((cmdStr[0]-'0')*10 + (cmdStr[1]-'0'))<<8)|((cmdStr[2]-'0')*10 + (cmdStr[3]-'0'));
			eTime[0] = (((cmdStr[4]-'0')*10 + (cmdStr[5]-'0'))<<8)|((cmdStr[6]-'0')*10 + (cmdStr[7]-'0'));
			sTime[1] = (((cmdStr[9]-'0')*10 + (cmdStr[10]-'0'))<<8)|((cmdStr[11]-'0')*10 + (cmdStr[12]-'0'));
			eTime[1] = (((cmdStr[13]-'0')*10 + (cmdStr[14]-'0'))<<8)|((cmdStr[15]-'0')*10 + (cmdStr[16]-'0'));
			sTime[2] = (((cmdStr[18]-'0')*10 + (cmdStr[19]-'0'))<<8)|((cmdStr[20]-'0')*10 + (cmdStr[21]-'0'));
			eTime[2] = (((cmdStr[22]-'0')*10 + (cmdStr[23]-'0'))<<8)|((cmdStr[24]-'0')*10 + (cmdStr[25]-'0'));
			if(YxAppFirewallReadWriteTime(0,sTime,eTime)==1)
				YxAppSaveFirewallList();
			while((cmdStr[i]!=0x0D)&&(cmdStr[i+1]!=0x0A))
			{
				i++;
			}
			i += 9;
		}
		return i;
	case 4://+IKSET=BAT:20,PRO:1,GPRS:00005,GPS:00005\r\n
		{
			char   number[7],bat=0,pro=0,*odlStr = cmdStr;
			U16    gprs = 0,gps = 0;
			YXSYSTEMPARAM *setParam = YxAppGetSystemParam(0);
			cmdStr += 7;
			while((cmdStr[0]!=0x0D)&&(cmdStr[1]!=0x0A))
			{
				cmdStr = strstr(cmdStr,"BAT:");
				if(cmdStr)
				{
					cmdStr += 4;
					number[0] = cmdStr[0];
					number[1] = cmdStr[1];
					number[2] = 0;
					bat = (char)atoi(number);
					cmdStr += 3;
				}
				else
					bat = 0;
				cmdStr = strstr(cmdStr,"PRO:");
				if(cmdStr)
				{
					cmdStr += 4;
					pro = cmdStr[0];
					cmdStr += 2;
				}
				else
					pro = 0;
				cmdStr = strstr(cmdStr,"GPRS:");
				if(cmdStr)
				{
					cmdStr += 5;
					number[0] = cmdStr[0];
					number[1] = cmdStr[1];
					number[2] = cmdStr[2];
					number[3] = cmdStr[3];
					number[4] = cmdStr[4];
					number[5] = 0;
					gprs = (U16)atoi(number);
					cmdStr += 6;
				}
				else
					gprs = 0;
				cmdStr = strstr(cmdStr,"GPS:");
				if(cmdStr)
				{
					cmdStr += 4;
					number[0] = cmdStr[0];
					number[1] = cmdStr[1];
					number[2] = cmdStr[2];
					number[3] = cmdStr[3];
					number[4] = cmdStr[4];
					number[5] = 0;
					gps = (U16)atoi(number);
					cmdStr += 5;
				}
				else
				{
					gprs = 0;
					break;
				}
			}
			if((pro>='1')&&(YxAppGetCurrentProfile()!=pro))
				YxAppSetCurrentProfile(pro);
			if((bat>0)||(setParam->uploadTick != gprs)||(setParam->gpsTimes != gps))
			{
				char needSave = 0;
				if(gprs>=YX_HEAT_TIME_MIN && gprs<=YX_HEAT_TIME_MAX)
				{
					needSave = 1;
					setParam->uploadTick = gprs;
				}
				if(gps>=YX_GPS_TIME_MIN && gps<=YX_GPS_TIME_MAX)
				{
					needSave = 2;
					setParam->gpsTimes = gps;
				}
				if(bat>0)
					needSave = 3;
				if(needSave)
				{
					if(YxAppSystemReadWriteLowBattery(0,bat)==0)
						YxAppSaveSystemSettings();
				}
			}
			cmdStr += 2;
			i = (U16)(cmdStr - odlStr);
		}
		return i;
	case 5://+IKDT=20150713202458\r\n
		{
			MYTIME  oldtime;
			cmdStr += 6;
			oldtime.nSec = (cmdStr[12]-'0')*10+(cmdStr[13]-'0');
			oldtime.nMin = (cmdStr[10]-'0')*10+(cmdStr[11]-'0');
			oldtime.nHour = (cmdStr[8]-'0')*10+(cmdStr[9]-'0');
			oldtime.nDay = (cmdStr[6]-'0')*10+(cmdStr[7]-'0');
			oldtime.nMonth = (cmdStr[4]-'0')*10+(cmdStr[5]-'0');
			oldtime.nYear = (cmdStr[0]-'0')*1000+(cmdStr[1]-'0')*100+(cmdStr[2]-'0')*10+(cmdStr[3]-'0');
			if(oldtime.nYear>=2015)
				mmi_dt_set_dt((const MYTIME*)&oldtime, NULL, NULL);
			i += 22;
		}
		return i;
	case 6://+IKDZWL=N:名称&#,T:2,LON:114.233344,LAT:23.444232,RAD:1500|N:名称&#,T:1,LON:114.233344,LAT:23.444232,RAD:1500|N:名称&#,T:0,LON:114.233344,LAT:23.444232,RAD:1500\r\n
		{
			S8   number[13],k = 0,names[(YX_EFENCE_NAME_LENGTH+1)<<2],*odlStr = cmdStr,type = '0',j = 0;
			double  lon = 0.0,lat = 0.0,rad = 0.0;
			cmdStr += 8;
			while((cmdStr[0]!=0x0D)&&(cmdStr[1]!=0x0A))
			{
				cmdStr = strstr(cmdStr,"N:");
				names[0] = 0;
				names[1] = 0;
				if(cmdStr)
				{
					j = 0;
					cmdStr += 2;
					while((cmdStr[j]!='&')&&(cmdStr[j+1]!='#'))
					{
						j++;
					}
					if(j>0)
						YxAppUTF8ToUincode((U8*)cmdStr,j,(U8*)names,(U16)((YX_EFENCE_NAME_LENGTH+1)<<2));
					j += 3;
					cmdStr += j;
				}
				cmdStr = strstr(cmdStr,"T:");
				if(cmdStr)
				{
					cmdStr += 2;
					type = cmdStr[0];
					cmdStr += 2;
				}
				else
					type = 0;
				cmdStr = strstr(cmdStr,"LON:");
				if(cmdStr)
				{
					j = 0;
					cmdStr += 4;
					while(cmdStr[j]!=',')
					{
						number[j] = cmdStr[j];
						j++;
						if(j>=12)
							break;
					}
					number[j++] = 0;
					lon = atof(number);
					cmdStr += j;
				}
				else
					lon = 0.0;
				cmdStr = strstr(cmdStr,"LAT:");
				if(cmdStr)
				{
					j = 0;
					cmdStr += 4;
					while(cmdStr[j]!=',')
					{
						number[j] = cmdStr[j];
						j++;
						if(j>=12)
							break;
					}
					number[j++] = 0;
					lat = atof(number);
					cmdStr += j;
				}
				else
					lat = 0.0;
				cmdStr = strstr(cmdStr,"RAD:");
				if(cmdStr)
				{
					j = 0;
					cmdStr += 4;
					while((cmdStr[j]!='|')&&(cmdStr[j]!='\r'))
					{
						number[j] = cmdStr[j];
						j++;
						if(j>=12)
							break;
					}
					number[j] = 0;
					rad = atof(number);
					if(cmdStr[j]=='|')
						j++;
					cmdStr += j;
				}
				else
				{
					rad = 0.0;
					break;//error
				}
				if(names[0]==0 && names[1]==0)
				{
					k++;
					continue;
				}
				else
				{
					YxAppEfenceUpdate(k,(U8*)names,type,lon,lat,rad);
					k++;
				}
			}
			cmdStr += 2;
			while(k<YX_EFENCE_MAX_NUM)
			{
				YxAppEfenceUpdate(k,NULL,0,0.0,0.0,0.0);
				k++;
			}
			YxAppEfenceSaveData();
			i = (U16)(cmdStr-odlStr);
		}
		return i;
	case 7://+IKALM=T:0810,ON:1,F:0,DAY:00|T:0810,ON:1,F:1,DAY:00|T:0810,ON:1,F:2,DAY:7F\r\n
		{
			S8  k = 0,hour = 0,min = 0,status = 0,freq = 0,*odlStr = cmdStr;
			U8  days = 0;
			cmdStr += 7;
			while((cmdStr[0]!=0x0D)&&(cmdStr[1]!=0x0A))
			{
				cmdStr = strstr(cmdStr,"T:");
				if(cmdStr)
				{
					cmdStr += 2;
					hour = (cmdStr[0]-'0')*10+(cmdStr[1]-'0');
					min = (cmdStr[2]-'0')*10+(cmdStr[3]-'0');
					cmdStr += 5;
				}
				else
				{
					hour = 0;
					min = 0;
				}
				cmdStr = strstr(cmdStr,"ON:");
				if(cmdStr)
				{
					cmdStr += 3;
					status = cmdStr[0]-'0';
					cmdStr += 2;
				}
				else
					status = 0;
				cmdStr = strstr(cmdStr,"F:");
				if(cmdStr)
				{
					cmdStr += 2;
					freq = cmdStr[0];
					cmdStr += 2;
				}
				else
					freq = 0;
				cmdStr = strstr(cmdStr,"DAY:");
				if(cmdStr)
				{
					S8     numb[3];
					int    tv = 0;
					cmdStr += 4;
					numb[0] = cmdStr[0];
					numb[1] = cmdStr[1];
					numb[2] = 0;
					sscanf(numb,"%x",&tv);
					days = (U8)(tv&0x7F);
					cmdStr += 2;
					if(cmdStr[0]=='|')
						cmdStr++;
				}
				else
				{
					days = 0;
					break;
				}
				YxAppSetAlarm(k,hour,min,status,freq,days);
				k++;
			}
			cmdStr += 2;
			while(k<YX_MAX_ALARM_NUM)
			{
				YxAppSetAlarm(k,0,0,0,0,0);
				k++;
			}
			i = (U16)(cmdStr - odlStr);
		}
		return i;
	case 8://+IKRES=1\r\n
		cmdStr += 7;
		if(cmdStr[0]=='1' || cmdStr[0]=='4')//ok
		{
			if(yxProtocolPkg.sendOk==0)
				yxProtocolPkg.sendOk = 1;
			if(yxProtocolPkg.isFirst==2)
				yxProtocolPkg.isFirst = 0;
			if(yxProtocolPkg.isSos==1)
				yxProtocolPkg.isSos = 0;
			if(cmdStr[0]=='4')
				*resCmd = 4;//close socket
		}
		else if(cmdStr[0]=='2')//ok,过去包数据OK
			*resCmd = 1;//需要回复LOG
		i += 10;
		return i;
	case 9://+IKBT=1\r\n
		cmdStr += 6;
		if(cmdStr[0]=='1')//ok
			*resCmd = 2;//需要回复BT mac
		i += 9;
		return i;
	case 10://+IKOFF=1\r\n
		cmdStr += 7;
		if(cmdStr[0]=='1')//ok
			*resCmd = 3;//不需要回复远程关机OK，直接CLOSE
		i += 10;
		return i;
	case 11://+IKSWT=1\r\n
		cmdStr += 7;
		if(cmdStr[0]=='1')//ok
			YxAppSearchWtPlayTone();
		i += 10;
		return i;
	case 12://+IKDIAL=ON:1,N:%d\r\n
		{
			S8  *callN = YxAppGetCallNumberBuffer();
			cmdStr += 11;
			memset((void*)callN,0,YX_APP_CALL_NUMBER_MAX_LENGTH+1);
			if(cmdStr[0]=='1')//语音监听
				*resCmd = 5;
			else //打指定号码电话
				*resCmd = 6;
			cmdStr = strstr(cmdStr,"N:");
			if(cmdStr)
			{
				i = 0;
				cmdStr += 2;
				while(cmdStr[i]!='\r')
				{
					if(i<YX_APP_CALL_NUMBER_MAX_LENGTH)
						callN[i] = cmdStr[i];
					i++;
				}
				i += 2;
			}
			i += 15;
		}
		return i;
	case 13://+IKSMS=S:发送者&#文本字符&#\r\n,暂时不支持
		cmdStr += 7;
		while(!(cmdStr[i]=='&'&&cmdStr[i+1]=='#'&&cmdStr[i+2]=='\r'&&cmdStr[i+3]=='n'))
		{
			i++;
		}
		i += 4;
		return i;
	case 14://+IKLCDT=T:%c\r\n,背光时间(0:5S,1:15S,2:30S,3:45S,4:60S)
		cmdStr += 10;
		YxAppSetBacklightTime((S8)cmdStr[0]);
		return 13;
	case 15://+IKSCRK=K:%c\r\n,暂时不支持
		return 13;
	case 16://+IKRPS=1\r\n,实时定位指令
		cmdStr += 7;
		if(cmdStr[0]=='1')//ok
			*resCmd = 7;//回复OK,再等服务器CLOSE后,运行定位包
		return 10;
	case 17://+IKPIC=1\r\n,实时拍照指令,暂时不支持
		return 10;
	case 18: //+IKSLP=ON:%c,T:%02d%02d%02d%02d\r\n,睡眠检测设置,暂时不支持
		return 24;
	case 19://+IKDTK=1\r\n,1:24小时制,0:12小时制
		cmdStr += 7;
		YxAppSetTimeFormat((S8)cmdStr[0]);
		return 10;
	default:
		return 0;
	}
}

static U16 YxAppShortCmdBuild(char kind,U8 *sendBuffer)
{
	U16  i = 0,checkSum = 0;
	YXSYSTEMPARAM *setParam = YxAppGetSystemParam(0);
	sprintf((char*)(sendBuffer+4),"%c1%05d%s+IKRES=%c\r\n",PROTOCOL_VER,CUSTOMER_ID,setParam->imei,kind);
	i = (U16)(strlen((char*)(sendBuffer+4)) + 6);
	sendBuffer[0] = (U8)(i&0x00FF);
	sendBuffer[1] = (U8)(i>>8);
	sendBuffer[2] = 0;
	sendBuffer[3] = 0;
	checkSum = YxProtocolCheckSum(sendBuffer,i);
	sendBuffer[i-2] = (U8)(checkSum&0x00FF);
	sendBuffer[i-1] = (U8)(checkSum>>8);
	return i;
}

static U16 YxAppTxtCmdProcMain(U8 *data,U16 dataLen,U8 *sendBuff,U16 sendBufMaxLen)
{
	YXSYSTEMPARAM    *setParam = YxAppGetSystemParam(0);
	S8    *tempStr = (S8*)(data+26),kind='1';
	U8    resCmd = 0,saveFlag = 0;
	U16   j = 0,checkSum = 0,i = 0;
	dataLen -= 26;
	while(i<dataLen)
	{
		j = YxAPPCommandProc(tempStr,(U16)(dataLen-i),&resCmd);
		if(resCmd != 0)
			saveFlag = resCmd;
		tempStr += j;
		i += j;
	}
	if(saveFlag==1)//log
	{
		YxAppDeleteUnsendPkgFile();
		return YxAppBuildTxtCmdGpsUploadPackage(0,sendBuff,sendBufMaxLen);
	}
	else if(saveFlag==2)//bt mac
	{
		S8   btMac[18];
		if(YxAppGetLocalBtMac(NULL,btMac)==1)
			sprintf((char*)(sendBuff+4),"%c1%05d%s+IKBT=%s\r\n",PROTOCOL_VER,CUSTOMER_ID,setParam->imei,btMac);
		else
			sprintf((char*)(sendBuff+4),"%c1%05d%s+IKBT=\r\n",PROTOCOL_VER,CUSTOMER_ID,setParam->imei);
		kind = 0;
	}
	else if(saveFlag==3)//远程关机OK,主动CLOSE
	{
		YxAppAutoShutdown();
		return 0;
	}
	else if(saveFlag==4)//收到通讯完毕指令,close socket
		return 0;
	else if(saveFlag==5)//语音监听,主动CLOSE
	{
		YxAppMakeWithCallType(YX_CALL_LISTEN,NULL);
		return 0;
	}
	else if(saveFlag==6)//回拨指定电话,主动CLOSE
	{
		YxAppMakeWithCallType(YX_CALL_NORMAL,YxAppGetCallNumberBuffer());
		return 0;
	}
	else if(saveFlag==7)//实时定位,先回复OK,等服务器发结束通讯指令后,再运行定位包
		YxAppSetOntimeCallGpsRunFlag();
	if(kind)
		sprintf((char*)(sendBuff+4),"%c1%05d%s+IKRES=%c\r\n",PROTOCOL_VER,CUSTOMER_ID,setParam->imei,kind);
	i = (U16)(strlen((char*)(sendBuff+4)) + 6);
	sendBuff[0] = (U8)(i&0x00FF);
	sendBuff[1] = (U8)(i>>8);
	sendBuff[2] = 0;
	sendBuff[3] = 0;
	checkSum = YxProtocolCheckSum(sendBuff,i);
	sendBuff[i-2] = (U8)(checkSum&0x00FF);
	sendBuff[i-1] = (U8)(checkSum>>8);
	return i;
}

static U16 YxAppVoiceCmdProcMain(U8 *data,U16 dataLen,U8 *sendBuff,U16 sendBufMaxLen)
{//not surpport now
	return YxAppShortCmdBuild('0',sendBuff);
}

static U16 YxAppImageCmdProcMain(U8 *data,U16 dataLen,U8 *sendBuff,U16 sendBufMaxLen)
{//not surpport now
	return YxAppShortCmdBuild('0',sendBuff);
}

static U16 YxAppAgpsCmdProcMain(U8 *data,U16 dataLen,U8 *sendBuff,U16 sendBufMaxLen)
{
	U16    length = 0;
	U8     *tempStr = data+26;
	tempStr += 8;
	length = (tempStr[1]<<8)|tempStr[0];
	tempStr += 2;
#if (YX_GPS_USE_AGPS==1)
	YxAgpsUpdateData(tempStr,length);
#endif
	return YxAppShortCmdBuild('1',sendBuff);//answer ok,ack
}

static U16 YxAppProtocolDataProcMain(U8 *data,U16 dataLen,U8 *sendBuff,U16 sendBufMaxLen)
{
	S8   idNum[6];
	U16  i = YxProtocolCheckSum(data,dataLen),cid = 0;
	memcpy((void*)idNum,(void*)(data+6),5);
	idNum[5] = 0;
	cid = atoi(idNum);
	if((i==((data[dataLen-1]<<8)|data[dataLen-2]))&&(data[4]==PROTOCOL_VER)&&(cid!=CUSTOMER_ID))//crc and version ok
	{
		if(data[5]=='1')//txt cmd
			return YxAppTxtCmdProcMain(data,dataLen,sendBuff,sendBufMaxLen);
		else if(data[5]=='2')//voice
			return YxAppVoiceCmdProcMain(data,dataLen,sendBuff,sendBufMaxLen);
		else if(data[5]=='3')//image
			return YxAppImageCmdProcMain(data,dataLen,sendBuff,sendBufMaxLen);
		else //if(data[5]=='4')//AGPS data
			return YxAppAgpsCmdProcMain(data,dataLen,sendBuff,sendBufMaxLen);
	}
	return 0;
}


extern YXAPPSOCKDATAPARAM yxAppSockData;
extern U8 recv_apollo_key[20];
extern U8 u8GpsProgress;
extern WatchConf WatchInstance;
extern void gps_start(void);
extern void ApolloGPSDataCallback(void);
extern void ApolloStartLab(void);

int ApolloWatchSave(void);
#if ENABLE_MMI_INITTIME
extern U8 u8NeedInitWatchTimeFlag;
#endif


char phonenumber[20] = {0};
extern void mmiFrameBufferReflushTimer(void);
//extern void mmiClockGetDateTime(rtc_format_struct *pTime);

S32 YxAppSockReadDataCallback(U8 *readBuf,U16 readLen,U8 *sendBuffer)//readBuf:从服务器读取的数据,readLen:数据长度,可以在此处理你们的数据协议,sendBuffer:如果有数据要上传到服务器上,请填写数据到此BUFFER中,注意,此BUFFER的最大长度为YX_SOCK_BUFFER_LEN,返回值为0时,不要上传数据，按SENDBUFFER的实际长度字节返回
{
#if (APOLLO_PROTOCOL_VERSION == APOLLO_ODM_AOXIN_OLDMAN_WATCH)
	int i = 0;	
	
	//apollo_init_aes_ctx();
	//apollo_decrypt(readBuf, readBuf);
	kal_prompt_trace(MOD_YXAPP,"aaa {wbj_debug} readBuf[0]:%c, %x %x, %x, %x, %x, %x, cmd:%x, readLen:%d\n", readBuf[0],readBuf[0], readBuf[1],
					readBuf[2], readBuf[3], readBuf[4], readBuf[5], readBuf[13], readLen);
#if 0
	if ((readBuf[0] == 0x01 && readBuf[1]== 0x02) ||(readBuf[0] == 'o' && readBuf[1] == 'k')) {
		ApolloAppSetRunFlag(APOLLO_RUNKIND_HEART_COMPLETE);
		return 1;
	}
#else
	if (readBuf[0] == 'R' && readBuf[1] == 'E' && readBuf[2] == 'T' && readBuf[3] == 'O' && readBuf[4] == 'K') {
		if ((readLen > 6) && u8NeedInitWatchTimeFlag) {
			rtc_format_struct rtcTime = {0};
			rtc_format_struct nowTime;
			rtcTime.rtc_year = readBuf[6];
			rtcTime.rtc_mon = readBuf[7]+1;
			rtcTime.rtc_wday = readBuf[8];
			rtcTime.rtc_day = readBuf[9];
			rtcTime.rtc_hour = readBuf[10];
			rtcTime.rtc_min = readBuf[11];
			rtcTime.rtc_sec = readBuf[12];

			mmiClockGetDateTime(&nowTime);
			mmiClockSetDateTime(&rtcTime);
			if (nowTime.rtc_min != rtcTime.rtc_min) {
				YxAppSendMsgToMMIMod(APOLLO_MSG_LED_REFLASH,0,0);
			}
			if (u8NeedInitWatchTimeFlag == 0x01) {
				//ApolloStartGPS();
				mmiFrameBufferReflushTimer();
				//YxAppSendMsgToMMIMod(APOLLO_MSG_LED_REFLASH,0,0);
				ApolloStartLab();
			}
			u8NeedInitWatchTimeFlag = 0x2;

			
			//u8NeedInitWatchTimeFlag = 0;
		}
		ApolloAppSetRunFlag(APOLLO_RUNKIND_HEART_COMPLETE);
		return 1;
	}
	if (readBuf[0] == 'S' && readBuf[1] == 'E' && readBuf[2] == 'T' && readBuf[3] == 'O' && readBuf[4] == 'K') {
		U8 ret[4] = {'O', 'K', 0x0, 0x0};
		//ApolloAppSetRunFlag(APOLLO_RUNKIND_HEART_COMPLETE);
		extern void Uart1_sends(U8 *buff,U16 len);
		Uart1_sends(ret, 2);

		if (APOLLO_RUNKIND_VOICE_UPLOAD == YxAppGetRunFlag()) {
			ApolloAppSetRunFlag(APOLLO_RUNKIND_HEART_COMPLETE);
		}
		return 1;
	}
#endif
	//recv apollo key
	if ((readBuf[0] == 0x6B && readBuf[1] == 0x65 && readBuf[2] == 0x79)) {
		for (i = 4;i < 20;i ++) {
			recv_apollo_key[i-4] = readBuf[i];
		}
		//APOLLO_BURN_KEY_MSG
		kal_prompt_trace(MOD_YXAPP, "APOLLO_BURN_KEY_MSG");
		YxAppSendMsgToMMIMod(APOLLO_BURN_KEY_MSG, 0, 0);
		return 1;
	}

	switch (readBuf[0]) {
		case HEART_PACKAGE_HEADER_CMD: {
			U8 cmd = get_heart_packet_response_cmd(readBuf);
			kal_prompt_trace(MOD_YXAPP, "cmd : %x", cmd);
			switch (cmd) {
				case COMMAND_NULL:
				case COMMAND_DEFAULT:
					break;


				case COMMAND_REALTIME_POS:
					//apollo_flag = YX_RUNKIND_OWNER_GPS;
					kal_prompt_trace(MOD_YXAPP,"  COMMAND_REALTIME_POS u8GpsProgress:%d\n", u8GpsProgress);
					if (u8GpsProgress == 0) {
						gps_start();	
						u8GpsProgress = 1;
						StartTimer(APOLLO_GPS_START_COUNTER_TIMER, YX_HEART_TICK_UNIT, ApolloGPSDataCallback);
					}
					break;
				case COMMAND_VOICE_RECV: {
					apollo_flag = YxAppGetRunFlag();
					apollo_flag |= APOLLO_RUNKIND_VOICE_DOWNLOAD;
					ApolloAppSetRunFlag(apollo_flag);
					break;
				}
				case COMMAND_AGPS_DOWNLOAD: {
					apollo_flag = YxAppGetRunFlag();
					apollo_flag |= APOLLO_RUNKIND_AGPS_DOWNLOAD;
					ApolloAppSetRunFlag(APOLLO_RUNKIND_AGPS_DOWNLOAD);
					break;
				}
				case COMMAND_COUNTSTEP_SETON:
				case COMMAND_COUNTSTEP_SETOFF:
				case COMMAND_SLEEPQUALITY_SETON:
				case COMMAND_SLEEPQUALITY_SETOFF:
				case COMMAND_ALARM:
				case COMMAND_SAFEZONE_ALARM:
				case COMMAND_LONGSAT_ALARM:
				case COMMAND_LOWPOWER_ALARM:
					
				case COMMAND_SOS_MSGREMAINER_OPEN:
				case COMMAND_SOS_MSGREMAINER_CLOSE:
				case COMMAND_POWER_MSGREMAINDER_OPEN:
				case COMMAND_POWER_MSGREMAINDER_CLOSE:
					
				case COMMAND_ALARMMUSIC_OPEN:
				case COMMAND_ALARMMUSIC_CLOSE:
				case COMMAND_SAFEZONEMUSIC_OPEN:
				case COMMAND_SAFEZONEMUSIC_CLOSE:
					
				case COMMAND_SOS_CLEAR:
				case COMMAND_PHONEFARE_SELECT: 
					kal_prompt_trace(MOD_YXAPP," cmd:%x\n", cmd);
					YxAppSendMsgToMMIMod(cmd, 0, 0); //send msg to queue
					break;
					
				case COMMAND_FAMILYNUMBER_SET: {
					int i = 0;
					U8 index = readBuf[IDX_PHONE_INDEX];
					if (index <= FAMILY_NUMBER_SIZE && index > 0) {					
						for (i = 0;i < 8;i ++) {
							phonenumber[2*i] = HIGH_HEXTOSTRING(readBuf[IDX_PHONE_NUMBER + i]);
							phonenumber[2*i+1] = LOW_HEXTOSTRING(readBuf[IDX_PHONE_NUMBER + i]);
						}
						phonenumber[15] = 0;

						//for (i = 0;i < 15;i ++) {
						//	WatchInstance.u8FamilyNumber[index-1][i] = phonenumber[i];
						//}
						strcpy(WatchInstance.u8FamilyNumber[index-1],phonenumber);
						ApolloWatchSave();
					}
					break;
				}
				case COMMAND_CONTRACT_SET: {
					int i = 0;
					U8 index = readBuf[IDX_PHONE_INDEX];
					if (index <= CONTACT_NUMBER_SIZE && index > 0) {					
						for (i = 0;i < 8;i ++) {
							phonenumber[2*i] = HIGH_HEXTOSTRING(readBuf[IDX_PHONE_NUMBER + i]);
							phonenumber[2*i+1] = LOW_HEXTOSTRING(readBuf[IDX_PHONE_NUMBER + i]);
						}
						phonenumber[15] = 0;

						//for (i = 0;i < 15;i ++) {
						//	WatchInstance.u8ContactNumber[index-1][i] = phonenumber[i];
						//}
						strcpy(WatchInstance.u8ContactNumber[index-1], phonenumber);
						ApolloWatchSave();
					}
					break;
				}
				case COMMAND_PHONEBOOK_SET: {
					int i = 0;
					U8 index = readBuf[IDX_PHONE_INDEX];
					if (index <= PHONE_BOOK_SIZE && index > 0) {					
						for (i = 0;i < 8;i ++) {
							phonenumber[2*i] = HIGH_HEXTOSTRING(readBuf[IDX_PHONE_NUMBER + i]);
							phonenumber[2*i+1] = LOW_HEXTOSTRING(readBuf[IDX_PHONE_NUMBER + i]);
						}
						phonenumber[15] = 0;

						//for (i = 0;i < 15;i ++) {
						//	WatchInstance.u8PhoneBook[index-1][i] = phonenumber[i];
						//}
						strcpy(WatchInstance.u8PhoneBook[index-1],phonenumber);
						ApolloWatchSave();
					}
					break;
				}
					
				case COMMAND_SET_PHONENUMBER: {
					int i = 0;
					for (i = 0;i < 8;i ++) {
						phonenumber[2*i] = HIGH_HEXTOSTRING(readBuf[IDX_PHONE_NUMBER + i]);
						phonenumber[2*i+1] = LOW_HEXTOSTRING(readBuf[IDX_PHONE_NUMBER + i]);
					}
					phonenumber[15] = 0;
					YxAppSendMsgToMMIMod(cmd, readBuf[IDX_PHONE_INDEX] - 1 + 0x30, 0);
					break;
				}

				case COMMAND_BUZZER_DEVICE_OPEN: 
				case COMMAND_BUZZER_DEVICE_CLOSE: 
				case COMMAND_LIGHT1_DEVICE_OPEN:
				case COMMAND_LIGHT1_DEVICE_CLOSE: 
				case COMMAND_LIGHT2_DEVICE_OPEN: 
				case COMMAND_LIGHT2_DEVICE_CLOSE: {
					kal_prompt_trace(MOD_YXAPP," cmd:%x\n", cmd);
					YxAppSendMsgToMMIMod(cmd, 0, 0); //send msg to queue
					break;
				}

				case COMMAND_FREQ_SET: {
					U8 index = readBuf[IDX_PHONE_INDEX];
					WatchInstance.u8Freq = (char)index;
					ApolloWatchSave();
					break;
				}
			}
			break;
		default:
			break;
		}
	}
	
	return 1;
#elif (APOLLO_PROTOCOL_VERSION == APOLLO_PROTOCOL_VERSION)

#else
	

#endif
}

PU8 YxProtocolGetBigBuffer(void)
{
	return yxProtocolPkg.bigBuffer;
}

void YxProtocolSetSosStatus(void)
{
	yxProtocolPkg.isSos = 1;
}

void YxProtocolInition(void)
{
	memset((void*)&yxProtocolPkg,0,sizeof(YXPROTOCOLPARAM));
	yxProtocolPkg.bigBuffer = YxAppMemMalloc(YX_BIG_MEMERY_LEN);
	yxProtocolPkg.isFirst = 1;
}
#endif
