#ifndef _ENGLISH_TEXT_H
#define _ENGLISH_TEXT_H

//tts
#ifndef LANGUAGE_ENGLISH
#define TTS_quit "0090fa51"                  //退出
#define TTS_set_sucessful "be8b6e7f10629f52" //设置成功
#else
#define TTS_quit "back"                          //退出
#define TTS_set_sucessful "setting successfully" //设置成功
#endif

#define c_sub_close "OFF" //关闭

#define TTS_menu_main "3b4edc8355532c00a47fc47e0990e962"       //主菜单,群组选择
#define TTS_no_one_online "e065ba4e2857bf7e"                   //无人在线
#define TTS_call_ok "7c54eb5310629f52"                         //呼叫成功
#define TTS_call_faild "7c54eb533159258d"                      //呼叫失败
#define TTS_call_function_close "394e7c549f52fd802a675362005f" //单呼功能未打开
#define TTS_not_online "0d4e2857bf7e"                          //不在线
#define TTS_locked "2e95d876f25d0195"                          //键盘已锁
#define TTS_unlock "d059019510629f52"                          //解锁成功
#define TTS_invite_exit "0090fa51394e7c54"                     //退出单呼
#define TTS_invite_call_me "7c54eb531162"                      //呼叫我

#define TTS_clear_successfully "056e649610629f52" //清除成功
#define TTS_clear_failed "056e64963159258d"       //清除失败
#define TTS_charging "636B285745513575"           //正在充电
#define TTS_charge_end "45513575D37E5F67"         //充电结束
#define TTS_power_low "3575CF914E4F"              //电量低

#define TTS_UNICODE_Battery "3575CF912000"               //Battery
#define TTS_UNICODE_percent "7E7606524B4E2000"           //百分之
#define TTS_UNICODE_Current_Group "535F4D52A47FC47E2000" //当前群组
#define TTS_UNICODE_Current_User "535F4D52287537622000"  //当前用户
#define TTS_Volume "F397CF912000"                        //音量x级

//小头
#define c_smallhead_offline "2800bb79bf7e2900" //(离线)

#define c_tx "TX > > > > >"
#define c_rx "RX < < < < <"

#ifndef LANGUAGE_ENGLISH
#define c_Welcome "6B228FCE4F7F7528" //欢迎使用
#define c_back "8fd456de"            //返回
#define c_enter "786E5B9A"           //确定

#define c_net_mode "516C7F516A215F0F"      //公网模式
#define c_analog_mode "6A2162DF6A215F0F"   //模拟模式
#define c_mix_mode "6DF754086A215F0F"      //混合模式
#define c_repeater_mode "4E2D7EE76A215F0F" //中继模式

#define c_main_menu "4E3B83DC5355"               //主菜单
#define c_net_menu "516C7F5183DC5355"            //公网菜单
#define c_analog_menu "6A2162DF83DC5355"         //模拟菜单
#define c_general_settings "901A75288BBE7F6E"    //通用设置
#define c_mode_switch "5DE54F5C6A215F0F52076362" //工作模式切换

#define c_Group_selection "7FA47EC4900962E9"  // 群组选择
#define c_Member_selection "62105458900962E9" // 成员选择
#define c_Friend_selection "597D53CB900962E9" // 好友选择
#define c_GPS "5B9A4F4D4FE1606F"              // 定位信息
#define c_Net_settings "516C7F518BBE7F6E"     //公网设置

#define c_ptttone "00500054005463d0793a97f30020"      //PTT提示音
#define c_poweron_tips "5f005173673a63d0793a97f30020" //开关机提示音
#define c_netset "7f517edc8bbe7f6e00200020"           //网络设置
#define c_POC_upgrade "0050004f004353477ea70020"      //POC升级
#define c_MCU_upgrade "004d0043005553477ea70020"      //MCU升级
#define c_air_code "7a7a4e2d5199780100200020"         //空中写码
#define c_tx_volume "90018bdd7b497ea700200020"        // 送话等级

#define c_bk_light "80cc514965f695f40020"  //背光时间
#define c_kb_lock "81ea52a89501952e0020"   //自动锁键
#define c_reset "6062590d51fa538200200020" //恢复出厂
#define c_about "51734e8e672c673a00200020" //关于本机

#define c_open "62535F00"  //打开
#define c_close "517395ED" //关闭

#define c_network_auto "81ea52a86a215f0f0020" //自动模式
#define c_network_4G "003400476a215f0f0020"   //4G模式
#define c_network_3G "003300476a215f0f0020"   //3G模式
//#define c_network_2G "003200476a215f0f0020"   //2G模式
#define c_network_2G "79fb52a8003200470020" //移动2G

#define c_txvolume_p3 "002b00337b497ea7" //+3等级
#define c_txvolume_p2 "002b00327b497ea7" //+2等级
#define c_txvolume_p1 "002b00317b497ea7" //+1等级
#define c_txvolume_p0 "680751c67b497ea7" //标准等级
#define c_txvolume_r1 "002d00317b497ea7" //-1等级
#define c_txvolume_r2 "002d00327b497ea7" //-2等级
#define c_txvolume_r3 "002d00337b497ea7" //-3等级

#define c_input_password "8bf763094e0a4e0b952e8f9351655bc67801" //请按上下键输入密码

#define c_mcu_version "004d004300557248672c003a0020" //MCU版本
#define c_poc_version "0050004f00437248672c003a0020" //POC版本
#define c_module_version "6a21575756fa4ef6003a0020"  //模块固件
#define c_my_account "672c673a8d2653f7003a0020"      //本机账号
#define c_IMEI "0049004d00450049003a0020"            //IMEI

#define c_15s "0031003579d20020"                                     //15秒
#define c_20s "0032003079d20020"                                     //20秒
#define c_30s "0033003079d20020"                                     //30秒
#define c_always_open "5e385f000020"                                 //常开
#define c_factory_ask "786e8ba489816062590d003f"                     //确认要恢复?
#define c_reset_sucessful "6062590d6210529f002c53735c0691cd542f0021" //恢复成功，即将重启!

#define c_channel_selection "4FE19053900962E9" //信道选择
#define c_subaudio "4E9A97F38BBE7F6E"          //亚音设置
#define c_apower "529F7387900962E9"            //功率选择
#define c_TxRxFRE "98915DEE8BBE7F6E"           //频差设置
#define c_sqlevel "9759566A8BBE7F6E"           //静噪设置
#define c_chspace "5E265BBD"                   //带宽
#define c_stepfreq "6B658FDB98917387"          //步进频率
#define c_tailaudio "5C3E97F36D889664"         //尾音消除
#define c_atot "53D15C04965065F6"              //发射限时
#define c_apowersave "770175356A215F0F"        //省电模式
#define c_vox "58F063A753D15C04"               //声控发射
#define c_freq_match "4E00952E5BF99891"        //一键对频
#define c_wireless_copy "65E07EBF590D5236"     //无线复制

#define c_rxctcss "6A2162DF63A565364E9A97F3" //模拟接收亚音
#define c_txctcss "6A2162DF53D15C044E9A97F3" //模拟发射亚音
#define c_rxdcs "65705B5763A565364E9A97F3"   //数字接收亚音
#define c_txdcs "65705B5753D15C044E9A97F3"   //数字发射亚音

#define c_High "9AD8529F7387"
#define c_Medium "4E2D529F7387"
#define c_Low "4F4E529F7387"

#define c_level1 "7B497EA70031"
#define c_level2 "7B497EA70032"
#define c_level3 "7B497EA70033"
#define c_level4 "7B497EA70034"
#define c_level5 "7B497EA70035"
#define c_level6 "7B497EA70036"
#define c_level7 "7B497EA70037"
#define c_level8 "7B497EA70038"
#define c_level9 "7B497EA70039"

#define c_Wide "00320035006B0048007A"           //宽带25kHz
#define c_Narrow "00310032002E0035006B0048007A" //窄带12.5kHz

#define c_freqmatch_ask "786E8BA48FDB51654E00952E5BF99891003f" //确认进入一键对频?
#define c_get_freq "83B753D6989173874E2D"                      //获取频率中
#define c_get_subaudio "83B753D64E9A97F34E2D"                  //获取亚音中

#define c_fsk_ask "786E8BA48FDB516565E07EBF590D5236003f" //确认进入无线复制?

#define c_get_ok "83B753D66210529F"                                          //获取成功
#define c_back_to_desktop "519951654FE190530031002C53735C068FD456DE684C9762" //写入信道1,返回桌面
#define c_current_freq "98917387003A"                                        //频率：
#define c_current_subaudio_type "4E9A97F37C7B578B003A"                       //亚音类型：
#define c_current_subaudio_type_none "4E9A97F37C7B578B003A65E0"              //亚音类型：无
#define c_current_subaudio_value "4E9A97F3503C003A"                          //亚音值：
#define c_current_subaudio_value_none "4E9A97F3503C003A65E0"                 //亚音值：无
#else
#define c_Welcome "Welcome to use" //欢迎使用
#define c_back "Back"              //返回
#define c_enter "enter"            //确定

#define c_net_mode "net mode"           //公网模式
#define c_analog_mode "analog mode"     //模拟模式
#define c_mix_mode "mix mode"           //混合模式
#define c_repeater_mode "repeater mode" //中继模式

#define c_main_menu "Main menu"               //主菜单
#define c_net_settings "Network settings"     //公网设置
#define c_analog_settings "Analog settings"   //模拟设置
#define c_general_settings "general settings" //通用设置
#define c_mode_switch "Mode switch"           //工作模式切换

#define c_Group_selection "Group Select"   // 群组选择
#define c_Member_selection "Member Select" // 成员选择
#define c_Friend_selection "Friend Select" // 好友选择
#define c_GPS "Location Inf"               // 定位信息
#define c_Net_settings "Network settings"  //公网设置

#define c_input_password "Input Pwd by side key!" //请按上下键输入密码

#define c_15s "15S"                              //15秒
#define c_20s "20S"                              //20秒
#define c_30s "30S"                              //30秒
#define c_always_open "ON"                       //常开
#define c_factory_ask "System Reset?"            //确认要恢复?
#define c_reset_sucessful "Success!Rebooting..." //恢复成功，即将重启!

/* 模拟 */
#define c_channel_selection "CHANNEL"
#define c_subaudio "CTS/DCS"
#define c_apower " POWER "
#define c_TxRxFRE "TxRxFRE"
#define c_sqlevel "SQUELCH"
#define c_chspace "Wide/Narrow"
#define c_stepfreq "Step frequency"
#define c_tailaudio "Tail sound"
#define c_atot "Max_Tx time"
#define c_apowersave "Save power mode"
#define c_vox "Vox"

#define c_rxctcss "Rx CTCSS"
#define c_txctcss "Tx CTCSS"
#define c_rxdcs "Rx DCS"
#define c_txdcs "Tx DCS"

#define c_High "高功率"
#define c_Medium "中功率"
#define c_Low "低功率"

#define c_level1 "Level 1"
#define c_level2 "Level 2"
#define c_level3 "Level 3"
#define c_level4 "Level 4"
#define c_level5 "Level 5"
#define c_level6 "Level 6"
#define c_level7 "Level 7"
#define c_level8 "Level 8"
#define c_level9 "Level 9"

#define c_Wide "Wide 25kHz"
#define c_Narrow "Narrow 12.5kHz"
#endif

//默认大头
#define c_gps_function_close "672A5F00542F5B9A4F4D529F80FD" //未开启定位功能
#define c_No_This_Function "65E06B64529F80FD"               //无此功能

#define c_download "8BF77A0D5019002e002e002e" //请稍候...
#define c_Poweroff "6b6357285173673a"         //正在关机
//验证插usb异常关机问题
#define c_Poweroff1 "6b6357285173673a0031" //正在关机
#define c_Poweroff2 "6b6357285173673a0032" //正在关机
#define c_Poweroff3 "6b6357285173673a0033" //正在关机

#define c_PowerOn "6b6357285f00673a0021" //正在开机
#define c_no_sim_card "8bf763d253610021" //请插卡!
#define c_idle "7a7a95f2"                //空闲
#define c_unlock "89e39501"              //解锁
#define c_menu "83dc5355"                //菜单

#define c_Im_talking "672c673a57288bb28bdd" //本机在讲话
#define c_Out_of_group "4E0D57287FA47EC4"   // 4E0D57287FA47EC4 不在群组
#define c_Offline "002879BB7EBF0029"        //离线
#define c_Walkie_Talkie "5BF98BB2673A"      //对讲机

#define c_Platform_friends "5E7353F0597D53CB" // 平台好友
#define c_Online_friend "57287EBF597D53CB"    // 在线好友
#ifdef CALL_RECORD
#define c_Single_call_record "901a8bdd56de653e" // 通话回放
#else
#define c_Single_call_record "5355547C8BB05F55" // 单呼记录
#endif

#define c_Single_call_mode "5355547C6A215F0F" // 单呼模式

#define c_PTTTone "63d0793a97f38bbe7f6e" // 提示音设置
#define c_bl_kb "80cc514953ca952e76d8"   // 背光及键盘
#define c_upgrade "8f6f4ef653477ea7"     // 软件升级
#define c_record "5f5597f38bbe7f6e"      // 录音设置
#define c_8kcode "0038004B7F167801"      // 8k编码

#define cNoSimTTS "f78bd2636153"                     //请插卡
#define cGroupNoOneTx "672A83B753D652307FA47EC40021" //未获取到群组!
#define c_download_failed "4E0B8F7D59318D250021"     //下载失败!

#define No_one_online "65e04EBA57287EBF" //无人在线

#define c_reboot "91cd542f" //重启

#define c_name_colon "540d79f0003a"            //名称:
#define c_time_colon "65f695f4003a"            //时间:
#define c_state_colon "72b66001003a"           //状态:
#define c_Invite_out_failed "547c51fa59318d25" //呼出失败
#define c_Invite_out_ok "547c51fa6210529f"     //呼出成功
#define c_Invite_in_ok "5bf965b9547c5165"      //对方呼入

#define c_latitude "7EAC5ea6003a"                     //纬度:
#define c_longitude "7ecf5ea6003a"                    //经度:
#define c_Number_of_satellites "536b661f4e2a6570003a" //卫星个数:

#define c_15s "0031003579d20020"                         //15秒
#define c_20s "0032003079d20020"                         //20秒
#define c_30s "0033003079d20020"                         //30秒
#define c_always_open "5e385f000020"                     //常开
#define c_ready_for_reset "786E8BA46062590D51FA5382003F" //确认恢复出厂？
#define c_air_coding "7a7a4e2d5199780100200020"          //空中写码
#define c_record_switch "5f5597f35f0051730020"           //录音开关
#define c_record_clear "6e057a7a5f5597f30020"            //清空录音
#endif