#include "SlaveMain.h"
#include <stdio.h>

void boardDescribe(char board[BoardRow][BoardCol], char* buf, uchr& len)
{
	len = 0;
	for (uchr i = 0; i < BoardRow; ++i)
	{
		uchr cnt = 0;
		for (uchr j = 0; j < BoardCol; ++j)
		{
			if (board[i][j] == b)
			{// ��λ��û���ӣ����Ӽ���
				++cnt;
			}
			else
			{// ������Ϊ0���Ƚ���������
				if (cnt != 0)
				{
					// cnt�������10
					buf[len++] = '0' + cnt;
					cnt = 0;
				}
				buf[len++] = board[i][j];
			}
		}
		if (cnt != 0)
		{
			// cnt�������10
			buf[len++] = '0' + cnt;
			cnt = 0;
		}
		if (i != BoardRow - 1)
		{
			buf[len++] = '/';
		}
	}
}

void creatFEN(char board[BoardRow][BoardCol], char* buf, char turn, ulong round)
{
	uchr len;

	boardDescribe(board, buf, len);
	// ��˭��
	buf[len++] = ' ';
	buf[len++] = turn;
	// ����Ĳ���
	buf[len++] = ' ';
	buf[len++] = '-';
	buf[len++] = ' ';
	buf[len++] = '-';
	buf[len++] = ' ';
	buf[len++] = '0';
	buf[len++] = ' ';
	// ����
	sprintf(buf + len, "%lu", round);
}