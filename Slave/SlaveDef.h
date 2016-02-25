#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include "CommonDef.h"

// 缓冲区最大大小(1KB)
#define MAX_BUF_SIZE (1 * 1024)
// 棋盘行数
#define BoardRow 10
// 棋盘列数
#define BoardCol 9

// LED针脚
#define ledPin 13

#ifdef DEBUG
// 测试使用的串口
#define comSer Serial
#else
// 最终使用的串口
#define comSer Serial1
#endif // DEBUG

// 棋子的定义，大写为红方，小写为黑方
enum Chess
{
	// 帅&将
	k = 'k',K = 'K',
	// 仕&士
	a = 'a',A = 'A',
	// 相&象
	e = 'e',E = 'E',
	// 马&馬
	h = 'h',H = 'H',
	// 车&車
	r = 'r',R = 'R',
	// 炮
	c = 'c',C = 'C',
	// 兵&卒
	p = 'p',P = 'P',
	// 空
	b = '\0'
};

class Point
{
public:
	char row,col;
	Point() :row(0),col(0){}
	Point(char _r,char _c) :row(_r),col(_c){}

	char* operator - (const Point& p)const 
	{
		static char s[5] = { 0,0,0,0,0 };
		s[0] = (char)col + 'a';
		s[1] = (char)row + '0';
		s[2] = (char)p.col + 'a';
		s[3] = (char)p.row + '0';
		return s;
	}
};

#endif // __SLAVE_DEF_H__