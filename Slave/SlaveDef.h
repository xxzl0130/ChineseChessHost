#ifndef __SLAVE_DEF_H__
#define __SLAVE_DEF_H__

#include "CommonDef.h"

// 缓冲区最大大小(1KB)
#define MAX_BUF_SIZE (1 * 1024)
// 棋盘行数
#define BoardRow 10
// 棋盘列数
#define BoardCol 9

// LED针脚
#define ledPin 13

// 开始按键
#define StartKey	22
// 结束按键
#define EndKey		23
// 左按键
#define LeftKey		24
// 右按键
#define RightKey	25

#ifdef DEBUG
// 测试使用的串口
#define comSer Serial
#else
// 最终使用的串口
#define comSer Serial1
#endif // DEBUG

#endif // __SLAVE_DEF_H__