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
// ���������ܵ�
HANDLE hPipeInputRead, hPipeInputWrite, hPipeOutputRead, hPipeOutputWrite;
HANDLE hScreen;
SECURITY_ATTRIBUTES sa;
STARTUPINFO engineInfo;
PROCESS_INFORMATION engineProcess;
/*
�����淢����Ϣ
*/
inline void send2Engine(const string &msg);
inline void send2Engine(const char *msg);

int main(int argc, char** argv)
{
	system("title ȫ�Զ��������ƽ̨ ������ V1.0");
	vector<thread> th;
	hScreen = GetStdHandle(STD_OUTPUT_HANDLE);
	// �ֶ��̳߳�ʼ��
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
	{// ������δ�ṩ����������Ļ����
		cout << "Port:";
		cin >> port;
		cout << "Baud:";
		cin >> baud;
	}
	else if (argc == 2)
	{// �����в���ֻ�ṩ�˴��ڣ�������Ĭ��
		port = string(argv[1]);
		baud = generalBaudRate;
	}
	else if (argc >= 3)
	{// �����в����ṩ�˴��ںͲ�����
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
	// ���ô������Բ�����
	slave.setPort(port);
	slave.setBaudrate(baud);
	slave.setTimeout(Timeout::max(), 1000, 0, 1000, 0);
	slave.open();
	// ��⿪��״̬
	Sleep(100);
	if (slave.isOpen() == false)
	{
		cout << __LINE__ << ":";
		goto serialErr;
	}
	SetConsoleTextAttribute(hScreen, FOREGROUND_INTENSITY);
	//cout << "���ڳ�ʼ���ɹ���" << endl;
	//return;
	// ��ӻ�����������Ϣ
	slave.write(testComHost);
	// �����յ�������Ϣ
	Sleep(1000);
	for (auto i = 0; i < 3; ++i)
	{
		slave.readline(tmp);
		if (tmp.find(testComSlave) != string::npos)
		{
			SetConsoleTextAttribute(hScreen, FOREGROUND_BLUE);
			cout << "���ڳ�ʼ���ɹ���" << endl;
			return;
		}
	}
	cerr << __LINE__ << ":";
	goto serialErr;
serialErr:
	cerr << "�޷��򿪴���:" << slave.getPort();
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
	//��������ܵ�
	if (CreatePipe(&hPipeInputRead, &hPipeInputWrite, &sa, 0) == 0)
		goto engineErr;
	//��������ܵ�
	if (CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &sa, 0) == 0)
		goto engineErr;
	engineInfo.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&engineInfo);
	engineInfo.hStdInput = hPipeInputRead;//�����ɱ�׼���� -> �ӹܵ��ж�ȡ
	engineInfo.hStdOutput = hPipeOutputWrite;//����ɱ�׼��� -> ������ܵ�
	engineInfo.wShowWindow = SW_HIDE;
	engineInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	if (CreateProcess(engineAddr, nullptr, nullptr, nullptr, TRUE, NULL,
	                  nullptr, nullptr, &engineInfo, &engineProcess) == 0)
		goto engineErr;
	send2Engine("ucci\n");

	//��1s����������
	Sleep(1000);
	ReadFile(hPipeOutputRead, buf, MAX_BUF_SIZE, nullptr, nullptr);
	if (strstr(buf, "ucciok") == nullptr)
		goto engineErr;
	// ����������ɫ
	SetConsoleTextAttribute(hScreen, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	// �����Ϣ
	cout << buf;
	SetConsoleTextAttribute(hScreen, FOREGROUND_BLUE);
	cout << "�����ʼ���ɹ���" << endl;
	// ����4�߳�
	send2Engine("setoption threads 4\n");
	// 1G�ڴ�
	send2Engine("setoption hashsize 1024\n");
	// ���
	send2Engine("setoption style normal\n");
	// �����
	send2Engine("setoption randomness small\n");
	
	return;

engineErr:
	cerr << "�޷���ʼ�����档\nError " << GetLastError() << endl;
	exit(1);
}

inline void send2Engine(const string& msg)
{
	WriteFile(hPipeInputWrite, msg.c_str(), msg.length(), nullptr, nullptr);
	// ����������ɫ
	SetConsoleTextAttribute(hScreen, FOREGROUND_RED | FOREGROUND_INTENSITY);
	// �����Ϣ
	cout << msg;
}

inline void send2Engine(const char* msg)
{
	WriteFile(hPipeInputWrite, msg, strlen(msg), nullptr, nullptr);
	// ����������ɫ
	SetConsoleTextAttribute(hScreen, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	// �����Ϣ
	cout << msg;
}

bool readOrderFromEngine(char* buf, size_t size)
{
	char* ptr;
	char order[64];
	// �����治�϶���
	memset(buf, 0, size);
	memset(order, 0, sizeof(order));
	while (ReadFile(hPipeOutputRead, buf, size, nullptr, nullptr))
	{
		// ����������ɫ
		SetConsoleTextAttribute(hScreen, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		// �����Ϣ
		cout << buf;
		if ((ptr = strstr(buf, "bestmove")) != nullptr)
		{
			memset(order, 0, sizeof(order));
			// ��bestmove��һ���и��Ƴ���
			for (auto i = 0, j = strchr(ptr, '\n') - ptr; i < j; ++i)
				order[i] = ptr[i];
			// �ٸ��ƻ�buf
			strcpy_s(buf, size, order);
			// �ҵ�bestmove���˳�
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
			// ����ӻ���������Ϣ
			if (slave.available())
			{
				string tmp;
				// ��һ����
				slave.readline(tmp);
				if (tmp[tmp.length() - 1] != '\n')
				{
					// ��֤�л���
					tmp += "\n";
				}
				// ����������
				send2Engine(tmp);
				// ����Ƿ��˳�
				if (tmp.find("quit") != string::npos)
				{
					cout << "�ӻ����˳������������" << endl;
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
			// �����������Ϣ
			if (readOrderFromEngine(buf,MAX_BUF_SIZE))
			{
				// ��ӻ�������Ϣ
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
	// �����߳��ں�̨ת������
	for (auto it = th.begin(); it != th.end(); ++it)
	{
		it->detach();
	}
	// ��������
	while (true)
	{
		Sleep(1000);
	}
}
