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
inline void send2Engine(string &msg);

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
	cout << "Init all done." << endl;
	work();
	return 0;
}

void initSerialArg(int argc, char** argv)
{
	string port;
	uint baud;
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
	return;
	// ��ӻ�����������Ϣ
	slave.write(testComHost);
	// �����յ�������Ϣ
	for (auto i = 0; i < 3; ++i)
	{
		slave.readline(tmp);
		if (tmp.find(testComSlave) != string::npos)
		{
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
	WriteFile(hPipeInputWrite, "ucci\n", 5, nullptr, nullptr);

	//��1s����������
	Sleep(1000);
	ReadFile(hPipeOutputRead, buf, MAX_BUF_SIZE, nullptr, nullptr);
	if (strstr(buf, "ucciok") == nullptr)
		goto engineErr;
	cout << "�����ʼ���ɹ���" << endl;
	return;

engineErr:
	cerr << "�޷���ʼ�����档\nError " << GetLastError() << endl;
	exit(1);
}

inline void send2Engine(string& msg)
{
	WriteFile(hPipeInputWrite, msg.c_str(), msg.length(), nullptr, nullptr);
}

bool readOrderFromEngine(char* buf, size_t size)
{
	// �����治�϶���
	while(ReadFile(hPipeOutputRead, buf, size, nullptr, nullptr))
	{
		if(strstr(buf,"bestmove") != nullptr)
		{
			// �ҵ�bestmove���˳�
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
			// ����ӻ���������Ϣ
			if (slave.available())
			{
				// ��һ����
				slave.readline(tmp);
				// ����������
				send2Engine(tmp);
				// ����������ɫ
				SetConsoleTextAttribute(hScreen, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				// �����Ϣ
				cout << tmp;
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
	while(true)
	{
		try
		{
			// �����������Ϣ
			if(readOrderFromEngine(buf,MAX_BUF_SIZE))
			{
				// �����淢����Ϣ
				slave.write(reinterpret_cast<unsigned char*>(buf), strlen(buf));
				// ����������ɫ
				SetConsoleTextAttribute(hScreen,FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				// �����Ϣ
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
	// �����߳��ں�̨ת������
	for (auto it = th.begin();it != th.end();++it)
	{
		it->detach();
	}
	// ��������
	while(true)
	{
		Sleep(1000);
	}
}
