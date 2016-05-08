#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include "CommonDef.h"

// 缓冲区最大大小(1024B)
#define MAX_BUF_SIZE (1024L)

// LED针脚
#define ledPin 13

// 开始/求和按键 上拉
#define StartKey	2
// 结束/认输按键 上拉
#define EndKey		3
// 左按键 上拉
#define LeftKey		4
// 右按键 上拉
#define RightKey	5
// 电磁铁
#define MagnetUp	14
#define MagnetDown	15

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
#define AudioMode	(SSDA_MODE_FULLRATE | SSDA_MODE_STEREO | SSDA_MODE_AUTOWORKER)

// 屏蔽跳线
#define jumpPinA		42
#define jumpPinB		43

// 最终使用的串口
#define comSer Serial1
// 输出调试信息用的串口
#define debugSer Serial

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
	b = 'b'
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
	
	bool operator =(const ChessPoint &t)const
	{
		return row == t.row && col == t.col && chess == t.chess;
	}
};

// 细分步数
#define circleStep		1600u
// 螺距(mm)
#define pitch			8
// 棋盘长度 mm
#define boardLength		230
// 棋盘宽度 mm
#define boardWidth		230
// 格子宽度
#define boxWidth		25
// 格子高度
#define boxLength		25
// 楚河汉界宽度
#define riverWidth		25
// 棋盘起始横坐标
#define xAxisStart		0
// 棋盘起始纵坐标
#define yAxisStart		0
// z轴步数
#define zAxisStep		1024UL
// x轴长度
#define xAxisLength		283
// y轴长度
#define yAxisLength		215

// 各种音频文件名
// 初始化
#define InitAudio		"ini.afs"
// 将军
#define CheckmateAudio	"che.afs"
// 选择难度
#define DifficultyAudio	"dif.afs"
// 玩家赢
#define LoseAudio		"los.afs"
// 选择顺序
#define OrderAudio		"ord.afs"
// 先手
#define FirstAudio		"fir.afs"
// 后手
#define SecondAudio		"sec.afs"
// 欢迎
#define WelcomeAudio	"wel.afs"
// 玩家输
#define WinAudio		"win.afs"
// “简单”
#define EasyAudio		"eas.afs"
// “普通”
#define NormalAudio		"nor.afs"
// “困难”
#define HardAudio		"har.afs"
// “大师”
#define MasterAudio		"mas.afs"
// 选择难度结束
#define DiffEndAudio	"die.afs"
// 电脑思考太久（5s) 提示“让我想想”之类
#define WaitLongAudio1	"wl1.afs"
// 电脑思考太久（30s) 提示“这步棋很难走”之类
#define	WaitLongAudio2	"wl2.afs"
// 与主机断开连接
#define DisconnectAudio	"dis.afs"
// 重新连接
#define ReconnectAudio	"rec.afs"
// 电脑提和
#define AIDrawAudio		"aid.afs"
// 再来一局？
#define AgainAudio		"aga.afs"

template<typename T>
void swap(T &a,T &b)
{
	T c = a;
	a = b;
	b = c;
}

#endif // __SLAVE_DEF_H__