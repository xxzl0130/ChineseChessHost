#include <string>
#include <iostream>
#include <cstdio>
#include <windows.h>
#include <cstring>
#include <thread>
#include <vector>

#include "serial/serial.h"
#include "MySerial.h"
#include "HostDef.h"
#include "slave/CommonDef.h"

using namespace std;
using namespace serial;

MySerial slave;
char buf[MAX_BUF_SIZE + 1];
// 创建两个管道
HANDLE hPipeInputRead, hPipeInputWrite, hPipeOutputRead, hPipeOutputWrite;
HANDLE hScreen;
SECURITY_ATTRIBUTES sa;
STARTUPINFO engineInfo;
PROCESS_INFORMATION engineProcess;
/*
向引擎发送信息
*/
inline void send2Engine(const string &msg);
inline void send2Engine(const char *msg);

int main(int argc, char** argv)
{
	system("title 全自动象棋对弈平台 主机端 V1.0");
	vector<thread> th;
	hScreen = GetStdHandle(STD_OUTPUT_HANDLE);
	// 分多线程初始化
	th.push_back(thread(initSerialArg, argc, argv));
	th.push_back(thread(initEngine));
	for (auto it = th.begin(); it != th.end(); ++it)
	{
		it->join();
	}
	work();
	return 0;
}

void initSerialArg(int argc, char** argv)
{
	string port;
	uint baud = generalBaudRate;
	if (argc == 1)
	{// 命令行未提供参数则由屏幕输入
		cout << "Port:";
		cin >> port;
		cout << "Baud:";
		cin >> baud;
	}
	else if (argc == 2)
	{// 命令行参数只提供了串口，波特率默认
		port = string(argv[1]);
		baud = generalBaudRate;
	}
	else if (argc >= 3)
	{// 命令行参数提供了串口和波特率
		port = string(argv[1]);
		sscanf_s(argv[2], "%u", &baud);
	}
	try
	{
		initSerial(port, baud);
	}
	catch (exception& err)
	{
		cerr << "Unhandled Exception: " << err.what() << endl;
		exit(1);
	}
}

void initSerial(string port, uint baud)
{
	string tmp;
	Sleep(100);
	SetConsoleTextAttribute(hScreen, FOREGROUND_BLUE);
	cout << "Port:" << port << " Baud:" << baud << endl;
	// 设置串口属性并开启
	slave.setPort(port);
	slave.setBaudrate(baud);
	slave.setTimeout(Timeout::max(), 1000, 0, 1000, 0);
	slave.open();
	// 检测开启状态
	Sleep(100);
	if (slave.isOpen() == false)
	{
		cout << __LINE__ << ":";
		goto serialErr;
	}
	SetConsoleTextAttribute(hScreen, FOREGROUND_INTENSITY);
	//cout << "串口初始化成功。" << endl;
	//return;
	// 向从机发送握手信息
	slave.write(testComHost);
	// 检查回收的握手信息
	Sleep(1000);
	for (auto i = 0; i < 3; ++i)
	{
		slave.readline(tmp);
		if (tmp.find(testComSlave) != string::npos)
		{
			SetConsoleTextAttribute(hScreen, FOREGROUND_BLUE);
			cout << "串口初始化成功。" << endl;
			return;
		}
	}
	cerr << __LINE__ << ":";
	goto serialErr;
serialErr:
	cerr << "无法打开串口:" << slave.getPort();
	system("pause");
	exit(1);
}

void initEngine()
{
	SetConsoleTextAttribute(hScreen, FOREGROUND_BLUE);
	cout << "Engine path:" << engineAddr << endl;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = nullptr;
	sa.bInheritHandle = TRUE;
	//数据输入管道
	if (CreatePipe(&hPipeInputRead, &hPipeInputWrite, &sa, 0) == 0)
		goto engineErr;
	//数据输出管道
	if (CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &sa, 0) == 0)
		goto engineErr;
	engineInfo.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&engineInfo);
	engineInfo.hStdInput = hPipeInputRead;//输入由标准输入 -> 从管道中读取
	engineInfo.hStdOutput = hPipeOutputWrite;//输出由标准输出 -> 输出到管道
	engineInfo.wShowWindow = SW_HIDE;
	engineInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	if (CreateProcess(engineAddr, nullptr, nullptr, nullptr, TRUE, NULL,
	                  nullptr, nullptr, &engineInfo, &engineProcess) == 0)
		goto engineErr;
	send2Engine("ucci\n");

	//给1s让引擎启动
	Sleep(1000);
	ReadFile(hPipeOutputRead, buf, MAX_BUF_SIZE, nullptr, nullptr);
	if (strstr(buf, "ucciok") == nullptr)
		goto engineErr;
	// 设置文字颜色
	SetConsoleTextAttribute(hScreen, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	// 输出信息
	cout << buf;
	SetConsoleTextAttribute(hScreen, FOREGROUND_BLUE);
	cout << "引擎初始化成功。" << endl;
	// 启用4线程
	send2Engine("setoption threads 4\n");
	// 1G内存
	send2Engine("setoption hashsize 1024\n");
	// 风格
	send2Engine("setoption style normal\n");
	// 随机性
	send2Engine("setoption randomness small\n");
	
	return;

engineErr:
	cerr << "无法初始化引擎。\nError " << GetLastError() << endl;
	exit(1);
}

inline void send2Engine(const string& msg)
{
	WriteFile(hPipeInputWrite, msg.c_str(), msg.length(), nullptr, nullptr);
	// 设置文字颜色
	SetConsoleTextAttribute(hScreen, FOREGROUND_RED | FOREGROUND_INTENSITY);
	// 输出信息
	cout << msg;
}

inline void send2Engine(const char* msg)
{
	WriteFile(hPipeInputWrite, msg, strlen(msg), nullptr, nullptr);
	// 设置文字颜色
	SetConsoleTextAttribute(hScreen, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	// 输出信息
	cout << msg;
}

bool readOrderFromEngine(char* buf, size_t size)
{
	char* ptr;
	char order[64];
	// 从引擎不断读入
	memset(buf, 0, size);
	memset(order, 0, sizeof(order));
	while (ReadFile(hPipeOutputRead, buf, size, nullptr, nullptr))
	{
		// 设置文字颜色
		SetConsoleTextAttribute(hScreen, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		// 输出信息
		cout << buf;
		if ((ptr = strstr(buf, "bestmove")) != nullptr)
		{
			memset(order, 0, sizeof(order));
			// 将bestmove那一整行复制出来
			for (auto i = 0, j = strchr(ptr, '\n') - ptr; i < j; ++i)
				order[i] = ptr[i];
			// 再复制回buf
			strcpy_s(buf, size, order);
			// 找到bestmove后退出
			return true;
		}
		Sleep(500);
	}
	return false;
}

void slave2Engine()
{
	while (true)
	{
		try
		{
			// 如果从机发送了信息
			if (slave.available())
			{
				string tmp;
				// 读一整行
				slave.readline(tmp);
				if (tmp[tmp.length() - 1] != '\n')
				{
					// 保证有换行
					tmp += "\n";
				}
				// 发送至引擎
				send2Engine(tmp);
				// 检查是否退出
				if (tmp.find("quit") != string::npos)
				{
					cout << "从机已退出，程序结束。" << endl;
					break;
				}
			}
		}
		catch (exception& err)
		{
			cerr << "Unhandled Exception: " << err.what() << endl;
			exit(1);
		}
		Sleep(200);
	}
}

void engine2Slave()
{
	while (true)
	{
		try
		{
			// 从引擎读入信息
			if (readOrderFromEngine(buf,MAX_BUF_SIZE))
			{
				// 向从机发送信息
				slave.write(reinterpret_cast<unsigned char*>(buf), strlen(buf));
			}
		}
		catch (exception& err)
		{
			cerr << "Unhandled Exception: " << err.what() << endl;
			system("pause");
			exit(1);
		}
		Sleep(200);
	}
}

void work()
{
	vector<thread> th;
	th.push_back(thread(slave2Engine));
	th.push_back(thread(engine2Slave));
	// 两个线程在后台转发数据
	for (auto it = th.begin(); it != th.end(); ++it)
	{
		it->detach();
	}
	// 保持休眠
	while (true)
	{
		Sleep(1000);
	}
}
