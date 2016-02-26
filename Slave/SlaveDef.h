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

// ��ʼ����
#define StartKey	22
// ��������
#define EndKey		23
// �󰴼�
#define LeftKey		24
// �Ұ���
#define RightKey	25

#ifdef DEBUG
// ����ʹ�õĴ���
#define comSer Serial
#else
// ����ʹ�õĴ���
#define comSer Serial1
#endif // DEBUG

#endif // __SLAVE_DEF_H__