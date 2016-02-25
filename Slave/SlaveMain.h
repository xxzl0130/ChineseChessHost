#ifndef __SLAVE_MAIN_H__
#define __SLAVE_MAIN_H__

#include "CommonDef.h"
#include "SlaveDef.h"

// 将局面变成FEN规范中的描述
void boardDescribe(char board[BoardRow][BoardCol], char* buf, uchr& len);
// 生成FEN格式串
void creatFEN(char board[BoardRow][BoardCol], char* buf, char turn, ulong round);

#endif // __SLAVE_MAIN_H__