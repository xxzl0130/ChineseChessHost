#ifndef __SLAVE_MAIN_H__
#define __SLAVE_MAIN_H__

#include "CommonDef.h"
#include "SlaveDef.h"

// ��������FEN�淶�е�����
void boardDescribe(char board[BoardRow][BoardCol], char* buf, uchr& len);
// ����FEN��ʽ��
void creatFEN(char board[BoardRow][BoardCol], char* buf, char turn, ulong round);

#endif // __SLAVE_MAIN_H__