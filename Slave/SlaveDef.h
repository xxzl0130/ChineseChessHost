#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include "CommonDef.h"

// ����������С(1024B)
#define MAX_BUF_SIZE (1024L)

// LED���
#define ledPin 13

// ��ʼ/��Ͱ��� ����
#define StartKey	2
// ����/���䰴�� ����
#define EndKey		3
// �󰴼� ����
#define LeftKey		4
// �Ұ��� ����
#define RightKey	5
// �����
#define MagnetUp	14
#define MagnetDown	15

// ������������ʼ��
#define RowStart	22
// ��������
#define RowCnt		10
// ������������ʼ��
#define ColStart	32
// ��������
#define ColCnt		9

// SPI�ӿ�
#define MISO_PIN	50
#define MOSI_PIN	51
#define SCK_PIN		52
#define CS_PIN		53

// ������
#define LeftAudio	44
// ������
#define RightAudio	45
// ��Ƶģʽ ȫ����&˫����
#define AudioMode	(SSDA_MODE_FULLRATE | SSDA_MODE_STEREO | SSDA_MODE_AUTOWORKER)

// ��������
#define jumpPinA		42
#define jumpPinB		43

// ����ʹ�õĴ���
#define comSer Serial1
// ���������Ϣ�õĴ���
#define debugSer Serial

// ��Ϸ״̬
enum GameState
{
	Play = 0,
	Draw,
	Resign,
	Win,
	Lose,
	// ��������Լ�����
	PlayerHoldHis,
	// ������ŵ��Ե���
	PlayerHoldOpp,
	// ���������������
	PlayerHoldTwo,
	// �������
	MoveDone,
	// �ȴ�����ָ��
	WaitOrder
};

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

// ϸ�ֲ���
#define circleStep		1600u
// �ݾ�(mm)
#define pitch			8
// ���̳��� mm
#define boardLength		230
// ���̿�� mm
#define boardWidth		230
// ���ӿ��
#define boxWidth		25
// ���Ӹ߶�
#define boxLength		25
// ���Ӻ�����
#define riverWidth		25
// ������ʼ������
#define xAxisStart		0
// ������ʼ������
#define yAxisStart		0
// z�Ჽ��
#define zAxisStep		1024UL
// x�᳤��
#define xAxisLength		283
// y�᳤��
#define yAxisLength		215

// ������Ƶ�ļ���
// ��ʼ��
#define InitAudio		"ini.afs"
// ����
#define CheckmateAudio	"che.afs"
// ѡ���Ѷ�
#define DifficultyAudio	"dif.afs"
// ���Ӯ
#define LoseAudio		"los.afs"
// ѡ��˳��
#define OrderAudio		"ord.afs"
// ����
#define FirstAudio		"fir.afs"
// ����
#define SecondAudio		"sec.afs"
// ��ӭ
#define WelcomeAudio	"wel.afs"
// �����
#define WinAudio		"win.afs"
// ���򵥡�
#define EasyAudio		"eas.afs"
// ����ͨ��
#define NormalAudio		"nor.afs"
// �����ѡ�
#define HardAudio		"har.afs"
// ����ʦ��
#define MasterAudio		"mas.afs"
// ѡ���ѶȽ���
#define DiffEndAudio	"die.afs"
// ����˼��̫�ã�5s) ��ʾ���������롱֮��
#define WaitLongAudio1	"wl1.afs"
// ����˼��̫�ã�30s) ��ʾ���ⲽ������ߡ�֮��
#define	WaitLongAudio2	"wl2.afs"
// �������Ͽ�����
#define DisconnectAudio	"dis.afs"
// ��������
#define ReconnectAudio	"rec.afs"
// �������
#define AIDrawAudio		"aid.afs"
// ����һ�֣�
#define AgainAudio		"aga.afs"

template<typename T>
void swap(T &a,T &b)
{
	T c = a;
	a = b;
	b = c;
}

#endif // __SLAVE_DEF_H__