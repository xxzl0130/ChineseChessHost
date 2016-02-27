#ifndef __SLAVE_MAIN_H__
#define __SLAVE_MAIN_H__

#include "CommonDef.h"
#include "SlaveDef.h"

// ��������FEN�淶�е�����
void boardDescribe(char board[BoardRow][BoardCol], char* buf, uchr& len);
// ����FEN��ʽ��
void creatFEN(char board[BoardRow][BoardCol], char* buf, char turn, ulong round);
// ��������������
String readOrderFromHost();
// ����go��˼�������state:0 ������1 ��ͣ�2 ����
void sendGo(uchr state = 0);
// ��������������state:0 ������1 ��ͣ�2 ����
void sendBoard(char board[BoardRow][BoardCol], GameState state = Play);
// ��ⰴ���Ƿ��£�Ĭ�ϼ��ߵ�ƽ
bool isPress(uint8_t pin, uint8_t state = HIGH);
// ��ʼ������
bool initSerial();
// ��ʼ�����̣������Ѷȡ����ֵ�
void initBoard();
// ��ʼ��LCD
void initLCD();
// ��ʼ��GPIO Pin
void initPin();

#endif // __SLAVE_MAIN_H__