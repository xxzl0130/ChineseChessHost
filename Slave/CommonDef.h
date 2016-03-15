#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

// 调试标志
#define DEBUG

#ifndef uchr
#define uchr unsigned char
#endif
#ifndef uint
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif
#ifndef ull
#define ull unsigned long long
#endif

// 主机向从机发送的测试字符串
#define testComHost "Hello slave"
// 从机向主机回复的测试字符串
#define testComSlave "Hello host"
// 串口比特率
#define generalBaudRate 9600UL

// 棋盘行数
#define BoardRow 10
// 棋盘列数
#define BoardCol 9

enum DIFFICULTY
{
	easy = 3,
	normal = 5,
	hard = 10,
	master = 32
};

#endif // __COMMON_DEF_H__