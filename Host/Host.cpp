#include <string>
#include <iostream>
#include <cstdio>
#include <windows.h>
#include <cstring>
#include <thread>
#include <vector>

#include "serial/serial.h"
#include "HostDef.h"
#include "slave/CommonDef.h"

using namespace std;
using namespace serial;

Serial slave;
char buf[MAX_BUF_SIZE + 1];
// 创建两个管道
HANDLE hPipeInputRead, hPipeInputWrite, hPipeOutputRead, hPipeOutputWrite;
HANDLE hScreen;
SECURITY_ATTRIBUTES sa;
STARTUPINFO engineInfo;
PROCESS_INFORMATION engineProcess;

void initSystem(int argc, char** argv);
void work();
void initSerial(string port, uint baud = baudRate);
void initSerialArg(int argc, char** argv);
void initEngine();
inline void send2Engine(string &msg);
bool readOrderFromEngine(char* buf, size_t size);
void slave2Engine();
void engine2Slave();

int main(int argc, char** argv)
{
	system("title 全自动象棋对弈平台 主机端 V1.0");
	initSystem(argc, argv);
	work();
	return 0;
}

void initSystem(int argc, char** argv)
{
	vector<thread> th;
	hScreen = GetStdHandle(STD_OUTPUT_HANDLE);
	// 分多线程初始化
	th.push_back(thread(initSerialArg, argc, argv));
	th.push_back(thread(initEngine));
	for (auto it = th.begin();it != th.end();++it)
	{
		it->join();
	}
}

void initSerialArg(int argc, char** argv)
{
	string port;
	uint baud;
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
		baud = baudRate;
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
	slave.setPort(port);
	slave.setBaudrate(baud);
	slave.setTimeout(Timeout::max(), 1000, 0, 1000, 0);
	slave.open();
	Sleep(1000);
	if (slave.isOpen() == false)
	{
		cerr << __LINE__ << ":";
		goto serialErr;
	}
	slave.write(testComHost);
	for (auto i = 0;i < 3;++i)
	{
		slave.readline(tmp);
		if(tmp.find(testComSlave) != string::npos)
		{
			cout << "串口初始化成功。" << endl;
			return;
		}
	}
	cerr << __LINE__ << ":";
	goto serialErr;

serialErr:
	cerr << "无法打开串口:" << slave.getPort();
	exit(1);
}

void initEngine()
{
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
	WriteFile(hPipeInputWrite, "ucci\n", 5, nullptr, nullptr);

	//给1s让引擎启动
	Sleep(1000);
	ReadFile(hPipeOutputRead, buf, MAX_BUF_SIZE, nullptr, nullptr);
	if (strstr(buf, "ucciok") == nullptr)
		goto engineErr;
	cout << "引擎初始化成功。" << endl;
	return;

engineErr:
	cerr << "无法初始化引擎。\nError " << GetLastError() << endl;
	exit(1);
}

inline void send2Engine(string& msg)
{
	WriteFile(hPipeInputWrite, msg.c_str(), msg.length(), nullptr, nullptr);
}

bool readOrderFromEngine(char* buf, size_t size)
{
	while(ReadFile(hPipeOutputRead, buf, size, nullptr, nullptr))
	{
		if(strstr(buf,"bestmove") != nullptr)
		{
			// 找到bestmove后退出
			return true;
		}
		Sleep(100);
	}
	return false;
}

void slave2Engine()
{
	string tmp;
	while(true)
	{
		try
		{
			if (slave.available())
			{
				slave.readline(tmp);
				send2Engine(tmp);
				SetConsoleTextAttribute(hScreen, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				cout << tmp;
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
	while(true)
	{
		try
		{
			if(readOrderFromEngine(buf,MAX_BUF_SIZE))
			{
				slave.write(reinterpret_cast<unsigned char*>(buf), strlen(buf));
				SetConsoleTextAttribute(hScreen,FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				cout << buf;
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

void work()
{
	vector<thread> th;
	th.push_back(thread(slave2Engine));
	th.push_back(thread(engine2Slave));
	// 两个线程在后台转发数据
	for (auto it = th.begin();it != th.end();++it)
	{
		it->detach();
	}
	// 保持休眠
	while(true)
	{
		Sleep(1000);
	}
}
