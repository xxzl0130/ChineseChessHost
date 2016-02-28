#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include "CommonDef.h"

// ����������С(1KB)
#define MAX_BUF_SIZE (1 * 1024)

// LED���
#define ledPin 13

// ��ʼ/��Ͱ��� int.0
#define StartKey	2
// ����/���䰴�� int.1
#define EndKey		3
// �󰴼�
#define LeftKey		4
// �Ұ���
#define RightKey	5

#ifdef DEBUG
// ����ʹ�õĴ���
#define comSer Serial
#else
// ����ʹ�õĴ���
#define comSer Serial1
#endif // DEBUG

enum GameState
{
	Play = 0,
	Draw,
	Resign,
	Win,
	Lose
};

#endif // __SLAVE_DEF_H__