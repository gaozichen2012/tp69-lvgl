#include "general_include.h"

#ifdef LANGUAGE_SETTING_CH
const char cTxGainTx[7][24] =
	{
		{"002b00337b497ea7"}, //+3等级
		{"002b00327b497ea7"}, //+2等级
		{"002b00317b497ea7"}, //+1等级
		{"680751c67b497ea7"}, //标准等级
		{"002d00317b497ea7"}, //-1等级
		{"002d00327b497ea7"}, //-2等级
		{"002d00337b497ea7"}, //-3等级
};

const unsigned char ucWeek[][5] =
	{
		{"65e5"}, //日
		{"4e00"}, //一
		{"4e8c"}, //二
		{"4e09"}, //三
		{"56db"}, //四
		{"4e94"}, //五
		{"516d"}  //六
};

const char cDevIniTx[] = "521d59cb531668c06d4b"; //初始化检测
const char cDebugTx[] = "8fdb51655199780172b66001";

const char cSetMenuTx1[] = "6309006F006B952E8FDB5165";			  //本机设置
const char cSetMenuTx[] = "672c673a8bbe7f6e";					  //本机设置
const char cInvRecvTx[] = "5355547c8bb05f55";					  //单呼记录
const char cRecordTx[] = "901a8bdd56de653e";					  //通话回放
const char cRecPlayTx[] = "64ad653e";							  //播放
const char cInviteMenuTx[] = "5355547c";						  //临时呼叫
const char cInviteExitTx[] = "630265ad";						  //挂断
const char cMemberDlLstTx[] = "521788684e0b8f7d4e2d002e002e002e"; //列表下载中...
const char cMenberNoOneTx[] = "65e057287ebf621054580021";		  //无在线成员！
const char cSosStopTx[] = "7ed3675f";							  //结束

const char cDesktopOffTx[] = "6b6357285173673a0021";	//正在关机
const char cDesktopOnTx[] = "6b6357285f00673a0021";		//正在开机
const char cDesktopLowPwrTx[] = "753591cf8fc74f4e0021"; //电量过低

const char cFriendNoOneTx[] = "65e057287ebf597d53cb0021"; //无在线好友

const char cFriendSelTTS[] = "7D59CB530990E962"; //好友选择

const char cMenuMainTTS[] = "3b4edc8355532c00a47fc47e0990e962"; //主菜单,群组选择
const char cMemberNoOneTTS[] = "e0652857bf7e106258542100";		//无成员在线
const char cMemberInvOKTTS[] = "7c54eb5310629f52";				//呼叫成功
const char cMemberInvFaildTTS[] = "7c54eb533159258d";			//呼叫失败

const char cInviteOffTTS[] = "394e7c549f52fd802a675362005f"; //单呼功能未打开

const char cFuncNotFinishTTS[] = "9f52fd802a679e5bb073"; //功能未实现

const char cDesktopNotOnlineTTS[] = "0d4e2857bf7e"; //不在线

const char cDesktopLKLockTTS[] = "2e95d876f25d0195";   //键盘已锁
const char cDesktopLKUnLockTTS[] = "d059019510629f52"; //解锁成功

const char cFriendNoOneTTS[] = "e0652857bf7e7d59cb532100"; //无在线成员

const char cInviteExitTTS[] = "0090fa51394e7c54"; //退出单呼
const char cInviteCallMeTTS[] = "7c54eb531162";	  //呼叫我

const char cSetFaildTTS[] = "be8b6e7f3159258d";		  //设置失败
const char cUSBONTTS[] = "550053004200f25d5362005f";  //USB已打开
const char cUSBOFFTTS[] = "550053004200f25d7351ed95"; //USB已关闭
const char cRecClrOKTTS[] = "056e649610629f52";		  //清除成功
const char cRecClrFAILDTTS[] = "056e64963159258d";	  //清除失败

const char cGpsLngTx[] = {"7ecf5ea6003a"};
const char cGpsLatTx[] = {"7eac5ea6003a"};
const char cGpsStarTx[] = {"536b661f4e2a6570003a"};

const char cRecUserTx[] = {"8bb28bdd62105458003a"}; //讲话成员:
const char cRecTimeTx[] = {"8bb28bdd65f695f4003a"}; //讲话时间:

const char ucBusyTips[] = "520763624e2d002e002e002e";

const char ucLanguageTx[2][32] =
	{
		"0045006e0067006c006900730068",
		"005400fc0072006b00e700650000"};

const unsigned char ucMenuItem[7][24] =
	{
		"63d0793a97f38bbe7f6e0000", //"提示音设置",
		"80cc514953ca952e76d80000", //"背光及键盘",
		"7f517edc8bbe7f6e00200020", //"网络设置	 ",
		"6062590d51fa538200200020", //"恢复出厂	 ",
		"8f6f4ef653477ea700200020", //"软件升级	 ",
		//"5f5597f38bbe7f6e00200020",		//"录音设置	 "
		"90018bdd7b497ea700200020", // 送话等级
		//"65f6533a8bbe7f6e00200020",		//"时区设置    "
		"51734e8e672c673a00200020" //"关于本机	 "
};

const unsigned char ucTimeZoneItem[25][20] =
	{
		"4e2d592e65f6533a0000", //中央时区
		"4e1c4e00533a00000000", //东一区
		"4e1c4e8c533a00000000", //东二区
		"4e1c4e09533a00000000", //东三区
		"4e1c56db533a00000000", //东四区
		"4e1c4e94533a00000000", //东五区
		"4e1c516d533a00000000", //东六区
		"4e1c4e03533a00000000", //
		"4e1c516b533a00000000", //
		"4e1c4e5d533a00000000", //
		"4e1c5341533a00000000", //
		"4e1c53414e00533a0000", //
		"4e1c53414e8c533a0000", //东十二区
		"897f4e00533a00000000", //西一区
		"897f4e8c533a00000000",
		"897f4e09533a00000000",
		"897f56db533a00000000",
		"897f4e94533a00000000",
		"897f516d533a00000000",
		"897f4e03533a00000000",
		"897f516b533a00000000",
		"897f4e5d533a00000000",
		"897f5341533a00000000",
		"897f53414e00533a0000",
		"897f53414e8c533a0000" //西十二区
};

const char cSetSubMenu[37][45] =
	{
		"004d004300557248672c003a0020",				//MCU版本
		"0050004f00437248672c003a0020",				//POC版本
		"6a21575756fa4ef6003a0020",					//模块固件
		"672c673a8d2653f7003a0020",					//本机账号
		"0049004d00450049003a0020",					//IMEI
		"7aef53e36a215f0f003a0020",					//端口模式
		"00500054005463d0793a97f30020",				//PTT提示音
		"5f005173673a63d0793a97f30020",				//开关机提示音
		"80cc514965f695f40020",						//背光时间
		"81ea52a89501952e0020",						//自动锁键
		"81ea52a86a215f0f0020",						//自动模式
		"003400476a215f0f0020",						//4G模式
		"003300476a215f0f0020",						//3G模式
		"79fb52a8003200470020",						//移动2G
		"004d0043005553477ea70020",					//MCU升级
		"0050004f004353477ea70020",					//POC升级
		"62535f000020",								//打开
		"517395ed0020",								//关闭
		"0031003579d20020",							//15秒
		"0032003079d20020",							//20秒
		"0033003079d20020",							//30秒
		"5e385f000020",								//常开
		"786e8ba489816062590d003f",					//确认要恢复?
		"8fd456de",									//返回
		"786e8ba4",									//确认
		"900962e9",									//选择
		"66f46539",									//更改
		"53477ea7",									//升级
		"6062590d6210529f002c53735c0691cd542f0021", //恢复成功，即将重启!
		"7a7a4e2d5199780100200020",					//空中写码
		"0055005300425f005173003a",					//USB开关
		"00550053004262535f00",						//USB打开
		"005500530042517395ed",						//USB关闭
		"5f5597f35f0051730020",						//录音开关
		"6e057a7a5f5597f30020",						//清空录音
		"63a5653680cc51490020",						//接收背光
		"53d15c048d8565f60020"						//发射超时
};

const char cMenuTTS[5][24] =
	{
		"A47FC47E0990E962", //群组选择
		"106258540990E962", //成员选择
		"7D59CB530990E962", //好友选择
		//"1a90dd8bde563e65",		//通话回放
		"9A5B4D4FE14F6F60", //定位信息
		"2C673A67BE8B6E7F"	//本机设置
};

const char cDsktopCruGroupTTS[] = "535f4d52a47fc47e3a00"; //当前群组
const char cDsktopCruUserTTS[] = "535f4d52287537623a00";  //当前用户
const char cDsktopBatLowTTS[] = "3575cf914e4f";			  //点量低

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
		"8f6f4ef653477ea74e2d002e002e002e", //软件升级中...
		"8f6f4ef64e0b8f7d6210529f0021",		//软件下载成功!
		"8f6f4ef653477ea74e2d002e002e002e", //软件升级中...
		"7248672c5df2662f670065b00021",		//版本已是最新!
		"65874ef67cfb7edf5f025e380021",		//文件系统异常!
		"56fa4ef64e0b8f7d59318d250021",		//固件下载失败!
};



const char cOtaOwcStateTx[][40] =
	{
		"7b495f856a2157570021", //等待模块
		"6a2157575c317eea0021", //写码就绪
		"519978016210529f0021", //写码成功
		"8bfb78016210529f0021", //读取成功
		"5199780159318d250021", //写码失败
		"8bfb780159318d250021", //读取失败
};

const char cOtaPocUpdateTx[] = "0050004f00438f6f4ef653477ea7"; //POC软件升级
const char cOtaMcuUpdateTx[] = "004d004300558f6f4ef653477ea7"; //MCU软件升级
const char cOtaOwcUpdateTx[] = "7a7a4e2d51997801";			   //空中写码

const char cOtaWarningTx[] = "8b66544a003a8bf752ff65ad75350021";	 //警告:请勿断电!
const char cOtaRestartTx[] = "91cd542f673a56685f0059cb53477ea70021"; //重启机器开始升级!
const char cOtaReturnTx[] = "65e0970053477ea7002c8bf78fd456de0021";	 //无需升级,请返回!
const char cOtaErrorTx[] = "53477ea759318d25002c8bf78fd456de0021";	 //升级失败,请返回!
const char cOtaRebootTx[] = "91cd542f";

const char cOtaAccountTx[] = "8d2653f7003a"; //账号:
const char cOtaOwcStaTx[] = "72b66001003a";	 //状态:

#endif
