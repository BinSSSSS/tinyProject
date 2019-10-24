#include <Windows.h>
#include <WinUser.h>
#include <iostream>
#include <sstream>
#include <process.h>
#include <fstream>

#pragma comment(linker,"/subsystem:\"windows\" /entry:\"wmainCRTStartup\"")	//改变程序入口点。此时窗口被隐藏


HHOOK g_hook;			//全局键盘钩子
LPKBDLLHOOKSTRUCT kbdStruct;		//保存键盘输入事件的信息
std::streampos lastPos = 0;	//上一次遍历的文件的位置
std::streampos fileBeg = 0;	//保存着文件的起始位置
std::streampos fileEnd = -1;//保存着文件的末尾位置


STARTUPINFO si = { sizeof(si) };
PROCESS_INFORMATION pi;

/* 对wall.ini文件进行写入文件路径数据，默认数据为空- */
void writeWallIni(std::string filePath) {
	
	std::ofstream wallIni("lastWall.ini");	//打开需要设置的图片路径
	wallIni << filePath;
	wallIni.close();
}
//将遍历文件的位置设置到末尾
void setFileEndPos() {
	std::ifstream ifs("wallpaperPath.ini", std::ios::ate);
	//直接跳到文件末尾
	lastPos = ifs.tellg() - (std::streamoff)1;
	ifs.close();
}
//打开壁纸切换的程序
void processWallpaperSwitching() {
	TCHAR appPath[] = TEXT("C:/Users/13959/Desktop/WallpaperSwitching/Release/WallpaperSwitching.exe");
	CreateProcess(appPath, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
}

/*调用壁纸函数进行壁纸的切换，并读取上一次的设置过的壁纸信息进行设置*/
void setLastWallpaper() {

	//恢复之前设置的图片-打开该配置文件并定位到文件末尾
	std::ifstream ifs("wallpaperPath.ini", std::ios::ate);

	

	///第一次遍历或者是遍历完全整个文件了
	if (lastPos == fileBeg || lastPos == fileEnd) {
		setFileEndPos();	//设置到文件末尾的位置
	}
	lastPos -= (std::streampos)2;	//因为跨过了一个换行符-那么就需要再跳转到前一个地方
	ifs.seekg(lastPos);	//每次定位到上次被定位的地方

	ifs >> std::noskipws;	//不跳过空格
							
	char c;
	std::string filePath;

	while (ifs >> c && c != '\n') {
		filePath.insert(filePath.begin(), c);	//每次向最开始的位置插入
		lastPos = ifs.tellg() - (std::streamoff)2;	//每次定位到前一个位置
		ifs.seekg(lastPos);	//每次向前移动一位
	}
	/*std::cout << "\n file: " << filePath << std::endl;
*/
	
	///如果读取到了文件路径-那么则写入到配置文件中供壁纸切换程序读取
	writeWallIni(filePath);	//写入数据到文件- 如果数据为空不影响写入
	
	processWallpaperSwitching();
	
	ifs.close();
}
void setNextWallpaper() {
	
	setFileEndPos();	//设置到到末尾位置
	writeWallIni("");	//写入空数据
	processWallpaperSwitching();	//运行壁纸切换程序
}



/// 回调函数 需要加上CALLBACK
/// nCode 为Hook代码。Hook使用它来确定任务
LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam)			//键盘处理函数
{

	if (nCode >= 0)		//当nCode大于等于0的时候.此时表现正常，可以调用CallNextHookEx返回也可以返回0
	{

		/// 需要注意 现在的扫描键码值前缀都要添加上L 也就是说之前的VK_MEAU需要变为VK_LMEAU才是现在的扫描码
		/// ALT是系统按键  扫描码值为 56  宏定义为VK_LMEAU 
		///当系统按键被按下
		kbdStruct = (LPKBDLLHOOKSTRUCT)lParam;
		if (wParam == WM_SYSKEYDOWN)
		{

			if (kbdStruct->vkCode == 0x41)		//如果按下alt+A 打开截图
			{
				TCHAR appPath[] = TEXT("C:/Users/13959/Desktop/QtWorkSpace/Sceenshot/release/Sceenshot.exe");
				CreateProcess(appPath, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
				//	std::cout << "ALT+A" << std::endl;
			}
			else if (kbdStruct->vkCode == 0x47)	//按下alt+G 屏幕休眠程序
			{
				TCHAR appPath[] = TEXT("C:/Users/13959/Desktop/QtWorkSpace/GreenEye/release/GreenEye.exe");
				CreateProcess(appPath, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
				//std::cout << "ALT+G" << std::endl;
			}
			else if (kbdStruct->vkCode == 0x43)	//按下alt+C	打开计算器
			{
				TCHAR cmd[] = TEXT("calc");
				CreateProcess(NULL, cmd, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
				//std::cout << "ALT+C" << std::endl;	
			}
			else if (kbdStruct->vkCode == 'Q')		//按下alt+Q 打开QQ
			{
				TCHAR appPath[] = TEXT("C:/腾讯QQ/Bin/qq.exe");
				CreateProcess(appPath, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
			}
			//else if (kbdStruct->vkCode == 'S')		//alt+S 打开自动切换壁纸软件
			//{
			//	TCHAR appPath[] = TEXT("C:/Users/13959/Desktop/QtWorkSpace/AutomaticDesktop/release/AutomaticDesktop.exe");
			//	CreateProcess(appPath, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
			//}
			else if (kbdStruct->vkCode == 'N')		//alt + N 打开 notepad++
			{
				TCHAR appPath[] = TEXT("C:/GooleDownload/Notepad++/notepad++.exe");
				CreateProcess(appPath, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
			}
			else if (kbdStruct->vkCode == 0x25 )	// alt + left 设置上一次的壁纸
			{
				setLastWallpaper();	
			}
			else if (kbdStruct->vkCode == 0x27) {	//alt + right 设置新的壁纸
				setNextWallpaper();
			}
			else if (kbdStruct->vkCode == 'F') {		//Alt + f 打开everything搜索引擎
				TCHAR appPath[] = TEXT("C:/BlueDream/Everything/Everything.exe");
				CreateProcess(appPath, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
			}
			else if (kbdStruct->vkCode == 'P') {  //Alt + p 打开画图
				TCHAR cmd[] = TEXT("mspaint");
				CreateProcess(NULL, cmd, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
			}
			else if (kbdStruct->vkCode == 'M') {  //Alt + M 打开安静模式
				TCHAR appPath[] = TEXT("C:/Users/13959/Desktop/C++_Program/QuiteMode/Release/QuiteMode.exe");
				CreateProcess(appPath, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
			}

			//std::cout << std::hex<< kbdStruct->vkCode << std::endl;
		}
		//VK_SHIFT
		///当非系统按键被按下
		else if (wParam == WM_KEYDOWN)
		{
			//std::cout << std::hex<< kbdStruct->vkCode << std::endl;
		}
	}

	return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

void setHook()			//设置钩子监视全局键盘
{
	/// 监视全局键盘的全局钩子。 回调函数为keyboardProc， 与当前进程绑定。
	if (!(g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, NULL, 0)))			//钩子程序错误时候执行
	{
		char strCode[4];
		std::stringstream strm;
		strm << GetLastError();
		strm >> strCode;
		MessageBoxA(NULL, strCode, "Error", MB_OK);
	}
}

/* 释放键盘钩子 */
void ReleaseHook()
{
	UnhookWindowsHookEx(g_hook);
}

int wmain()
{
	HANDLE hMutex;

	//设置一个互斥量，来放置程序多开，只需要一个钩子程序监测键盘即可。
	hMutex = CreateMutexA(NULL, FALSE, "GlobalKeyboard");
	if (GetLastError() == ERROR_ALREADY_EXISTS)					//单实例程序
		return 0;			

	setHook();		//设置监视全局键盘信息的钩子

	MSG * msg = nullptr;
	bool bRet;		
	while ((bRet = GetMessage(msg, NULL, 0, 0)))
	{

	}
	ReleaseHook();
	return 0;
}