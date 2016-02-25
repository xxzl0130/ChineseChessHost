#include <Wire\Wire.h>
#include <LiquidCrystal_I2C\LiquidCrystal_I2C.h>
#include <String.h>
#include "CommonDef.h"
#include "SlaveDef.h"
#include "StpperMotor.h"
#include "SlaveMain.h"
/*
Todo list:
// ��ʼ�����̣������Ѷȡ����ֵ�
void initBoard();

// ������ƶ�����
bool humanMoveChess();

��ʾ������

// ��̨�ƶ�����
void moveChess(char order[4]);
*/

String tmp;
char buf[MAX_BUF_SIZE];
// �Ծֻغ���
ulong roundCnt;
// ����ִ��
char AIColor;
// ��ͼ���
uchr drawCnt;
// �������
uchr resignCnt;
// �Ѷ�
DIFFICULTY diff;
// ����
char board[BoardRow][BoardCol] = {
	{ r,h,e,a,k,a,e,h,r },
	{ b,b,b,b,b,b,b,b,b },
	{ b,c,b,b,b,b,b,c,b },
	{ p,b,p,b,p,b,p,b,p },
	{ b,b,b,b,b,b,b,b,b },
	{ b,b,b,b,b,b,b,b,b },
	{ P,b,P,b,P,b,P,b,P },
	{ b,C,b,b,b,b,b,C,b },
	{ b,b,b,b,b,b,b,b,b },
	{ R,H,E,A,K,A,E,H,R }
};

// ��ʼ������
bool initSerial();
// ��ʼ�����̣������Ѷȡ����ֵ�
void initBoard();
// ������ƶ�����
bool humanMoveChess();
// ��������������
String readOrderFromHost();
// ִ������
void executeOrder(String& order);
// ��� flag:false �� true ��
bool draw(bool flag);
// ���� flag:false �� true ��
bool resign(bool flag);
// ����go��˼�������state:0 ������1 ��ͣ�2 ����
void sendGo(uchr state = 0);
// ��������������state:0 ������1 ��ͣ�2 ����
void sendBoard(uchr state = 0);
// ��̨�ƶ�����
void moveChess(char order[4]);

void setup()
{
	pinMode(ledPin, OUTPUT);
	digitalWrite(13, LOW);
	initSerial();
	initBoard();
}

void loop()
{
	if (humanMoveChess())
	{
		sendBoard();
		delay(500);
		tmp = readOrderFromHost();
		executeOrder(tmp);
	}
	if (comSer.available())
	{
		tmp = readOrderFromHost();
		executeOrder(tmp);
	}
	delay(100);
}

bool initSerial()
{
	comSer.begin(baudRate);
	if (!comSer)
		return false;
	for (uchr i = 0; i < 5; ++i)
	{
		tmp = comSer.readString();
		//comSer.println(tmp);
		if (strstr(tmp.c_str(), testComHost) != NULL)
		{ // �ҵ�Ӧ���ַ�������ʾ���ӳɹ�
			comSer.println(testComSlave); // �ظ�
			return true;
		}
		delay(100);
	}
	return false;
}

void initBoard()
{
}

bool humanMoveChess()
{
	return true;
}

String readOrderFromHost()
{
	while (true)
	{
		if (comSer.available())
		{
			tmp = comSer.readString();
			if (strstr(tmp.c_str(), "bestmove") != NULL)
			{ // �ҵ�bestmove
				return tmp;
			}
		}
		delay(100);
	}
}

void executeOrder(String& order)
{
	if (strstr(order.c_str(), "draw") != NULL)
	{
		// �������
		if (draw(true))
		{
			return;
		}
	}
	else
	{// û����� ��ռ���
		drawCnt = 0;
	}
	if (strstr(order.c_str(), "resign") != NULL)
	{
		// ��������
		if (resign(true))
		{
			return;
		}
	}
	else
	{// û������ ��ռ���
		resignCnt = 0;
	}
	// ��ȡ4λ��������Ϣ
	char move[4];
	char* ptr = strstr(order.c_str(), "bestmove") + 9/*����"bestmoove "*/;
	for (uint i = 0; i < 4; ++i)
	{
		move[i] = ptr[i];
	}
	// ����
	moveChess(move);
}

bool draw(bool flag)
{
	++drawCnt;
	if (drawCnt >= 2)
	{// ˫��ͬ�����
		drawCnt = 0;
		// todo: ��ʾ����ʾ������Ϣ
		return true;
	}
	if (flag == false)
	{// �����
		sendBoard(1);
	}
	else
	{// �������
		// todo: ��ʾ����ʾ������Ϣ
	}
	return false;
}

bool resign(bool flag)
{
	++resignCnt;
	if (resignCnt >= 2)
	{// ˫��ͬ������
		resignCnt = 0;
		// todo: ��ʾ����ʾ������Ϣ
		return true;
	}
	if (flag == false)
	{// ������
		sendBoard(2);
	}
	else
	{// �������
		// todo: ��ʾ����ʾ������Ϣ
	}
	return false;
}

void sendGo(uchr state)
{
	comSer.print("go depth");
	comSer.print(diff);
	if (state == 1)
	{// ���
		comSer.print(" draw");
	}
	else if (state == 2)
	{
		comSer.print(" resign");
	}
	comSer.print('\n');
}

void sendBoard(uchr state)
{
	creatFEN(board, buf, AIColor, ++roundCnt);
	comSer.println(buf);
	sendGo(state);
}

void moveChess(char order[4])
{
}
