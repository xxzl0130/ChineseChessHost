#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include "CommonDef.h"
#include "SimpleSDAudio/SimpleSDAudio.h"

// 缓冲区最大大小(256B)
#define MAX_BUF_SIZE (256)

// LED针脚
#define ledPin 13

// 开始/求和按键 int.0 上拉
#define StartKey	2
// 结束/认输按键 int.1 上拉
#define EndKey		3
// 左按键 上拉
#define LeftKey		4
// 右按键 上拉
#define RightKey	5

// 棋盘行引脚起始口
#define RowStart	22
// 棋盘行数
#define RowCnt		10
// 棋盘列引脚起始口
#define ColStart	32
// 棋盘列数
#define ColCnt		9

// SPI接口
#define MISO_PIN	50
#define MOSI_PIN	51
#define SCK_PIN		52
#define CS_PIN		53

// 左声道
#define LeftAudio	44
// 右声道
#define RightAudio	45
// 音频模式 全速率&双声道
#define AudioMode	(SSDA_MODE_FULLRATE | SSDA_MODE_STEREO)

#ifdef DEBUG
// 测试使用的串口
#define comSer Serial
#else
// 最终使用的串口
#define comSer Serial1
#endif // DEBUG

// 游戏状态
enum GameState
{
	Play = 0,
	Draw,
	Resign,
	Win,
	Lose,
	// 玩家拿着自己的子
	PlayerHoldHis,
	// 玩家拿着电脑的子
	PlayerHoldOpp,
	// 玩家拿着两个棋子
	PlayerHoldTwo,
	// 走子完成
	MoveDone,
	// 等待引擎指令
	WaitOrder
};

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

class ChessPoint
{
public:
	char row, col;
	Chess chess;
	ChessPoint() :row(0), col(0), chess(b) {}
	ChessPoint(char _r, char _c, Chess _ch) :row(_r), col(_c), chess(_ch) {}

	char* operator - (const ChessPoint& p)const
	{
		static char s[5] = { 0,0,0,0,0 };
		s[0] = static_cast<char>(col) + 'a';
		s[1] = static_cast<char>(row) + '0';
		s[2] = static_cast<char>(p.col) + 'a';
		s[3] = static_cast<char>(p.row) + '0';
		return s;
	}
};

// 细分步数
#define circleStep		6400u
// 螺距(mm)
#define pitch			5
// 棋盘长度 mm
#define boardLength		250
// 棋盘宽度 mm
#define boardWidth		250
// 格子宽度
#define boxWidth		20
// 棋盘起始横坐标
#define xAxisStart		20
// 棋盘起始纵坐标
#define yAxisStart		20
#endif // __SLAVE_DEF_H__