#ifndef __CHESS_BOARD_H__
#define __CHESS_BOARD_H__

#include <Arduino.h>
#include "SlipTable.h"

/*
�������������̱�ʾ����ع淶����
http://www.xqbase.com/protocol/cchess_move.htm
http://www.xqbase.com/protocol/cchess_fen.htm
*/

#ifndef uchr
#define uchr unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef ulong
#define uint unsigned long
#endif

// ���ӵĶ��壬��дΪ�췽��СдΪ�ڷ�
enum Chess
{
	// ˧&��
	k = 'k', K = 'K',
	// ��&ʿ
	a = 'a', A = 'A',
	// ��&��
	e = 'e', E = 'E',
	// ��&�R
	h = 'h', H = 'H',
	// ��&܇
	r = 'r', R = 'R',
	// ��
	c = 'c', C = 'C',
	// ��&��
	p = 'p', P = 'P',
	// ��
	b = '\0'
};

class chessPoint
{
public:
	char row, col;
	chessPoint() :row(0), col(0) {}
	chessPoint(char _r, char _c) :row(_r), col(_c) {}

	char* operator - (const chessPoint& p)const
	{
		static char s[5] = { 0,0,0,0,0 };
		s[0] = (char)col + 'a';
		s[1] = (char)row + '0';
		s[2] = (char)p.col + 'a';
		s[3] = (char)p.row + '0';
		return s;
	}
};

class ChessBoard
{
private:
	// ����ʼ���������
	uchr rowStart, rowEnd;
	// ����ʼ���������
	uchr colStart, colEnd;
	// ��̨
	SlipTable table;
	/*
	�Լ���������Ҫ����Ϣ
	*/
public:
	// ����
	char board[BoardRow][BoardCol] = {
		{ r,h,e,a,k,a,e,h,r },// 9
		{ b,b,b,b,b,b,b,b,b },// 8
		{ b,c,b,b,b,b,b,c,b },// 7
		{ p,b,p,b,p,b,p,b,p },// 6
		{ b,b,b,b,b,b,b,b,b },// 5
		{ b,b,b,b,b,b,b,b,b },// 4
		{ P,b,P,b,P,b,P,b,P },// 3
		{ b,C,b,b,b,b,b,C,b },// 2
		{ b,b,b,b,b,b,b,b,b },// 1
		{ R,H,E,A,K,A,E,H,R } // 0
							  //    a b c d e f g h i
	};
	/*
	���캯�������������������
	*/
	ChessBoard();
	/*
	��ȡ�ƶ����ӵ���Ϣ��
	����ֵΪ���������׼��4�ַ���ʽ������a2a5
	*/
	String getMove();
	void moveChess(char order[4]);
	void moveChess(String order);
};

#endif // __CHESS_BOARD_H__
