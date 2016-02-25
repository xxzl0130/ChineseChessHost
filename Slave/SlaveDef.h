#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include "CommonDef.h"

// ����������С(1KB)
#define MAX_BUF_SIZE (1 * 1024)
// ��������
#define BoardRow 10
// ��������
#define BoardCol 9

// LED���
#define ledPin 13

#ifdef DEBUG
// ����ʹ�õĴ���
#define comSer Serial
#else
// ����ʹ�õĴ���
#define comSer Serial1
#endif // DEBUG

// ���ӵĶ��壬��дΪ�췽��СдΪ�ڷ�
enum Chess
{
	// ˧&��
	k = 'k',K = 'K',
	// ��&ʿ
	a = 'a',A = 'A',
	// ��&��
	e = 'e',E = 'E',
	// ��&�R
	h = 'h',H = 'H',
	// ��&܇
	r = 'r',R = 'R',
	// ��
	c = 'c',C = 'C',
	// ��&��
	p = 'p',P = 'P',
	// ��
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