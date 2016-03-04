#pragma once

#include <string>
#include "slave/CommonDef.h"
using std::string;

// ���������ַ
const char *engineAddr = "Engine\\ELEEYE.exe";

// ����������С(4KB)
#define MAX_BUF_SIZE (4 * 1024)


/*
ͨ��main����������ʼ�����ڡ�
argv[1]������Ϊ�˿ڣ����������ɱ�׼�����ȡ
argv[2]������Ϊ�����ʣ���������ʹ��Ĭ�ϲ�����baudRate
��ȡ�˿ںͲ����ʺ����initSerial���н�һ����ʼ��
*/
void initSerialArg(int argc, char** argv);
/*
����portΪ�˿ڣ�����baudΪ�����ʣ�Ĭ��ΪbaurdRate��
ʹ��Serial���ṩ�ĳ�ʼ���������г�ʼ����
����ӻ����ͳ�ʼ����Ϣ��ȷ�����ӡ�
��ʼ��������ֹ��������
*/
void initSerial(string port, uint baud = generalBaudRate);
/*
��ʼ���������棬�����л�������
*/
void initEngine();
/*
�������ȡ��Ϣ
*/
bool readOrderFromEngine(char* buf, size_t size);
/*
�Ӵӻ���������ת������
�����̺߳�̨���С�
*/
void slave2Engine();
/*
���������ӻ���ת������
�����̺߳�̨���С�
*/
void engine2Slave();
void work();