#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include "CommonDef.h"
#include "SimpleSDAudio/SimpleSDAudio.h"

// ����������С(256B)
#define MAX_BUF_SIZE (256)

// LED���
#define ledPin 13

// ��ʼ/��Ͱ��� int.0 ����
#define StartKey	2
// ����/���䰴�� int.1 ����
#define EndKey		3
// �󰴼� ����
#define LeftKey		4
// �Ұ��� ����
#define RightKey	5

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

#ifdef DEBUG
// ����ʹ�õĴ���
#define comSer Serial
#else
// ����ʹ�õĴ���
#define comSer Serial1
#endif // DEBUG

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
#define circleStep		6400u
// �ݾ�(mm)
#define pitch			5
// ���̳��� mm
#define boardLength		250
// ���̿�� mm
#define boardWidth		250
// ���ӿ��
#define boxWidth		20
// ������ʼ������
#define xAxisStart		20
// ������ʼ������
#define yAxisStart		20
#endif // __SLAVE_DEF_H__