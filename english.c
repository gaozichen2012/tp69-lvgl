#include "general_include.h"

#ifdef LANGUAGE_SETTING_EN
const unsigned char ucWeek[][5] =
	{
		{"Sun"}, //日
		{"Mon"}, //一
		{"Tue"}, //二
		{"Wed"}, //三
		{"Thu"}, //四
		{"Fri"}, //五
		{"Sat"}	 //六
};

const char cDevIniTx[] = "Initializing..."; //初始化检测
const char cDebugTx[] = "8fdb51655199780172b66001";

const char cSetMenuTx[] = "Setting";			//本机设置
const char cInvRecvTx[] = "Inv Record";			//单呼记录
const char cRecordTx[] = "Playback";			//录音回放
const char cInviteMenuTx[] = "Temporary call";	//临时呼叫
const char cInviteExitTx[] = "Exit";			//挂断
const char cRecPlayTx[] = "Play";				//播放
const char cMemberDlLstTx[] = "Downloading..."; //列表下载中...
const char cMenberNoOneTx[] = "Nobody online!"; //无在线成员！
const char cSosStopTx[] = "Stop";				//结束

const char cDesktopOffTx[] = "Power OFF"; //正在关机
const char cDesktopOnTx[] = "Power ON";	  //正在开机

const char cFriendNoOneTx[] = "Nobody online!"; //无在线好友

const char cFriendSelTTS[] = "friend select"; //好友选择

const char cMenuMainTTS[] = "main menu, group select";		  //主菜单,群组选择
const char cMemberNoOneTTS[] = "nobody online";				  //无成员在线
const char cMemberInvOKTTS[] = "temporary call successfully"; //呼叫成功
const char cMemberInvFaildTTS[] = "temporary call faild";	  //呼叫失败

const char cInviteOffTTS[] = "please turn on temporary call"; //单呼功能未打开

const char cDesktopNotOnlineTTS[] = "off line"; //不在线

const char cDesktopLKLockTTS[] = "locked";	 //键盘已锁
const char cDesktopLKUnLockTTS[] = "unlock"; //解锁成功

const char cFriendNoOneTTS[] = "nobody online"; //无在线成员

const char cInviteExitTTS[] = "exit temporary call";			//退出单呼
const char cInviteCallMeTTS[] = "630061006c006c00200069006e00"; //呼叫我

const char cSetFaildTTS[] = "setting faild";

const char cRecClrOKTTS[] = "clear successfully"; //清除成功
const char cRecClrFAILDTTS[] = "clear faild";	  //清除失败

const char cUSBONTTS[] = "USB opened";	//USB已打开
const char cUSBOFFTTS[] = "USB closed"; //USB已关闭

const char cGpsLngTx[] = {"lon:"};
const char cGpsLatTx[] = {"lat:"};
const char cGpsStarTx[] = {"sate count:"};

const char cRecUserTx[] = {"Speaker:"}; //讲话成员:
const char cRecTimeTx[] = {"Time:"};	//讲话时间:

const char ucBusyTips[] = "Network Seting...";

const char ucLanguageTx[2][32] =
	{
		"0045006e0067006c006900730068",
		"005400fc0072006b00e700650000"};

const unsigned char ucMenuItem[9][24] =
	{
		"Prompt Tone", //"提示音设置",
		"BL & KB  ",   //"背光及键盘",
		"Net Mode ",   //"网络设置	 ",
		"Reset",	   //"恢复出厂	 ",
		"FW Update  ", //"软件升级	 ",
		"Play Back",   //
		"Time Zone",   //
		"Language ",
		"About   " //"关于本机	 "
};

const unsigned char ucTimeZoneItem[25][20] =
	{
		"UTC+0",  //中央时区
		"UTC+1",  //东一区
		"UTC+2",  //东二区
		"UTC+3",  //东三区
		"UTC+4",  //东四区
		"UTC+5",  //东五区
		"UTC+6",  //东六区
		"UTC+7",  //
		"UTC+8",  //
		"UTC+9",  //
		"UTC+10", //
		"UTC+11", //
		"UTC+12", //东十二区
		"UTC-1",  //西一区
		"UTC-2",
		"UTC-3",
		"UTC-4",
		"UTC-5",
		"UTC-6",
		"UTC-7",
		"UTC-8",
		"UTC-9",
		"UTC-10",
		"UTC-11",
		"UTC-12" //西十二区
};

const char cSetSubMenu[37][45] =
	{
		"MCU Version:",			//MCU版本
		"POC Version:",			//POC版本
		"FW Version:",			//模块固件
		"Account:   ",			//本机账号
		"IMEI:      ",			//IMEI
		"Mode:      ",			//端口模式
		"PTT prompt ",			//PTT提示音
		"Boot prompt",			//开关机提示音
		"BL time    ",			//背光时间
		"KB lock    ",			//自动锁键
		"Auto       ",			//自动模式
		"4G         ",			//4G模式
		"3G         ",			//3G模式
		"2G         ",			//2G模式
		"Upgrade MCU",			//MCU升级
		"Upgrade POC",			//POC升级
		"ON",					//打开
		"OFF",					//关闭
		"15S",					//15秒
		"20S",					//20秒
		"30S",					//30秒
		"ON",					//常开
		"System Reset?",		//确认要恢复?
		"Back",					//返回
		"OK",					//确认
		"Next",					//选择
		"Change",				//更改
		"Update",				//升级
		"Success!Rebooting...", //恢复成功，即将重启!
		"Account OTA ",			//空中写码
		"USB SW:",				//USB开关
		"USB ON",				//USB打开
		"USB OFF",				//USB关闭
		"Record SW",			//录音开关
		"Clear All",			//清除录音
		"Rx Backlit",			//接收背光
		"PTT TOT"				//接收限时
};

const char cMenuTTS[5][24] =
	{
		"Group Select",	 //群组选择
		"Member Select", //成员选择
		"Friend Select", //好友选择
		//"Play Back",		//单呼记录
		"Location Infor", //定位信息
		"Local Seting"	  //本机设置
};

const char cDsktopCruGroupTTS[] = "430075007200720065006e0074002000470072006f0075007000"; //当前群组
const char cDsktopCruUserTTS[] = "430075007200720065006e00740020007500730065007200";	  //当前用户
const char cDsktopBatLowTTS[] = "Battery Low";											  //点量低

const char cDsktopBatLvlTTS[11][30] =
	{
		"3575cf913a4ef696000000000000",
		"3575cf917e7606524b4e41530000",
		"3575cf917e7606524b4e8c4e4153",
		"3575cf917e7606524b4e094e4153",
		"3575cf917e7606524b4edb564153",
		"3575cf917e7606524b4e944e4153",
		"3575cf917e7606524b4e6d514153",
		"3575cf917e7606524b4e034e4153",
		"3575cf917e7606524b4e6b514153",
		"3575cf917e7606524b4e5d4e4153",
		"3575cf917e7606524b4e004e7e76",
};

const char cOtaPocStateTx[][40] =
	{
		"Upgrating...",		  //软件升级中...
		"Upgrate Seccess",	  //软件升级成功!
		"Upgrating...",		  //软件升级中...
		"No New Version",	  //版本已是最新!
		"File System Error!", //文件系统异常!
		"FW Download Faild!", //固件下载失败!
};


const char cOtaOwcStateTx[][40] =
	{
		"Waitting",		 //等待模块
		"Ready",		 //写码就绪
		"Write Success", //写码成功
		"Read Success",	 //读取成功
		"Write Faild",	 //写码失败
		"Read Faild",	 //读取失败
};

const char cOtaPocUpdateTx[] = "Upgrate POC"; //POC软件升级
const char cOtaMcuUpdateTx[] = "Upgrate MCU"; //MCU软件升级
const char cOtaOwcUpdateTx[] = "Account OTA"; //空中写码

const char cOtaWarningTx[] = "Don't Power OFF!";  //警告:请勿断电!
const char cOtaRestartTx[] = "Please reboot!   "; //重启机器以生效!
const char cOtaReturnTx[] = "No valid FW";		  //无需升级,请返回!
const char cOtaErrorTx[] = "Upgrate Faild!";	  //升级失败,请返回!
const char cOtaRebootTx[] = "Reboot";

const char cOtaAccountTx[] = "Acc:";   //账号:
const char cOtaOwcStaTx[] = "Status:"; //状态:

#endif
