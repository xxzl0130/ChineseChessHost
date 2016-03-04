#pragma once

#include <string>
#include "slave/CommonDef.h"
using std::string;

// 象棋引擎地址
const char *engineAddr = "Engine\\ELEEYE.exe";

// 缓冲区最大大小(4KB)
#define MAX_BUF_SIZE (4 * 1024)


/*
通过main函数参数初始化串口。
argv[1]存在则为端口，不存在则由标准输入获取
argv[2]存在则为波特率，不存在则使用默认波特率baudRate
获取端口和波特率后调用initSerial进行进一步初始化
*/
void initSerialArg(int argc, char** argv);
/*
参数port为端口，参数baud为波特率，默认为baurdRate。
使用Serial类提供的初始化方法进行初始化，
并向从机发送初始化信息以确认连接。
初始化错误将终止整个程序。
*/
void initSerial(string port, uint baud = generalBaudRate);
/*
初始化象棋引擎，并进行基本设置
*/
void initEngine();
/*
从引擎获取信息
*/
bool readOrderFromEngine(char* buf, size_t size);
/*
从从机到主机的转发程序。
独立线程后台运行。
*/
void slave2Engine();
/*
从主机到从机的转发程序。
独立线程后台运行。
*/
void engine2Slave();
void work();