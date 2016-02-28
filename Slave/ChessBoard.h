#ifndef __CHESS_BOARD_H__
#define __CHESS_BOARD_H__

#include <Arduino.h>
#include "SlipTable.h"

/*
棋子描述、棋盘表示等相关规范见：
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

// 棋子的定义，大写为红方，小写为黑方
enum Chess
{
	// 帅&将
	k = 'k', K = 'K',
	// 仕&士
	a = 'a', A = 'A',
	// 相&象
	e = 'e', E = 'E',
	// 马&馬
	h = 'h', H = 'H',
	// 车&車
	r = 'r', R = 'R',
	// 炮
	c = 'c', C = 'C',
	// 兵&卒
	p = 'p', P = 'P',
	// 空
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
	// 行起始、结束针脚
	uchr rowStart, rowEnd;
	// 列起始、结束针脚
	uchr colStart, colEnd;
	// 滑台
	SlipTable table;
	/*
	以及其他所需要的信息
	*/
public:
	// 棋盘
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
	构造函数，视情况添加所需参数
	*/
	ChessBoard();
	/*
	获取移动棋子的信息。
	返回值为象棋引擎标准的4字符形式，例如a2a5
	*/
	String getMove();
	void moveChess(char order[4]);
	void moveChess(String order);
};

#endif // __CHESS_BOARD_H__
