#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include "CommonDef.h"

// ����������С(256B)
#define MAX_BUF_SIZE (256)

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
#define upMagnet 12

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
#define AudioMode	(SSDA_MODE_FULLRATE | SSDA_MODE_STEREO)

// ��������
#define jumpPinA		41
#define jumpPinB		42

// ����ʹ�õĴ���
#define comSer Serial1
// ���������Ϣ�õĴ���
#define debugSer Serial1

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

// ϸ�ֲ���
#define circleStep		1600u
// �ݾ�(mm)
#define pitch			8
// ���̳��� mm
#define boardLength		250
// ���̿�� mm
#define boardWidth		250
// ���ӿ��
#define boxWidth		30
// ���Ӹ߶�
#define boxLength		22
// ���Ӻ�����
#define riverWidth		24
// ������ʼ������
#define xAxisStart		15
// ������ʼ������
#define yAxisStart		15
// z�Ჽ��
#define zAxisStep		1600u

#endif // __SLAVE_DEF_H__