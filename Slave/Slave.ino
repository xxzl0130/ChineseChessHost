#include <Wire\Wire.h>
#include <LiquidCrystal_I2C\LiquidCrystal_I2C.h>
#include <String.h>
#include "CommonDef.h"
#include "SlaveDef.h"
#include "StpperMotor.h"
#include "SlaveMain.h"
#include "MatrixKeyboard.h"
/*
Todo list:
// ��ʼ������
void initBoard();

// ������ƶ�����
bool humanMoveChess();

// ��̨�ƶ�����
void moveChess(char order[4]);
*/

//LCD1602
LiquidCrystal_I2C Lcd(0x27, 16, 2);
/*
��ʾ����ʽ��
 0123456789012345
 ******************
0*                *0x80
1*                *0xC0
 ******************
    >Red<  Black  
*/

String tmp;
char buf[MAX_BUF_SIZE];
// �Ծֻغ���
ulong roundCnt;
// ����ִ��
char AIColor = 'b';
// ��ͼ���
uchr drawCnt;
// �������
uchr resignCnt;
// �Ѷ�
DIFFICULTY diff = easy;
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

// ������ƶ�����
bool humanMoveChess();
// ִ������
void executeOrder(String& order);
// ��� flag:false �� true ��
bool draw(bool flag);
// ���� flag:false �� true ��
bool resign(bool flag);
// ��̨�ƶ�����
void moveChess(char order[4]);
// �ȴ���ʼ
void waitStart();
// ѡ���Ѷ�
void selectDiff();
// ѡ��˳��
void selectOrder();
// ��ʼ
void start();
// ��Ϸ����
void play();
// �ָ�����
void reset();

void setup()
{
	initPin();
	initLCD();
	initSerial();
	initBoard();
}

void loop()
{
	waitStart();
	start();
	play();
	reset();
}

bool humanMoveChess()
{
	return true;
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
	char* ptr = strstr(order.c_str(), "bestmove") + 9/*����"bestmove "*/;
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

void moveChess(char order[4])
{
}

void waitStart()
{
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print(" Chinese Chess");
	Lcd.setCursor(0, 1);
	Lcd.print("Press Start key");
	// �ȴ����¿�ʼ��ť
	while (!isPress(StartKey));
	selectDiff();
	selectOrder();
}

void selectDiff()
{
	// ѡ���Ѷ�
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print(" >Easy< Normal");
	Lcd.setCursor(0, 1);
	Lcd.print("  Hard  Master");
	while (true)
	{
		if (isPress(StartKey, LOW))
		{
			// �Ѷ�ѡ�����
			break;
		}
		if (isPress(LeftKey, LOW))
		{
			switch (diff)
			{
			case easy:
				diff = easy;
				break;
			case normal:
				Lcd.setCursor(0, 0);
				Lcd.print(" >Easy< Normal ");
				diff = easy;
				break;
			case hard:
				Lcd.setCursor(0, 0);
				Lcd.print("  Easy >Normal<");
				diff = normal;
				break;
			case master:
				Lcd.setCursor(0, 1);
				Lcd.print(" >Hard< Master ");
				diff = hard;
				break;
			}
		}
		if (isPress(RightKey, LOW))
		{
			switch (diff)
			{
			case easy:
				Lcd.setCursor(0, 0);
				Lcd.print("  Easy >Normal<");
				diff = normal;
				break;
			case normal:
				Lcd.setCursor(0, 1);
				Lcd.print(" >Hard< Master ");
				diff = hard;
				break;
			case hard:
				Lcd.setCursor(0, 1);
				Lcd.print("  Hard >Master<");
				diff = master;
				break;
			case master:
				diff = master;
				break;
			}
		}
	}
}

void selectOrder()
{
	// ѡ���Ⱥ���
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print("Select Color:");
	Lcd.setCursor(0, 1);
	Lcd.print("  >Red<  Black  ");
	while (true)
	{
		if (isPress(StartKey, LOW))
		{
			// �Ⱥ���ѡ�����
			break;
		}
		if (isPress(LeftKey, LOW))
		{
			switch (AIColor)
			{
			case 'b':
				AIColor = 'b';
				break;
			case 'r':
				Lcd.setCursor(0, 1);
				Lcd.print("  >Red<  Black  ");
				AIColor = 'b';
				break;
			default:
				AIColor = 'b';
				break;
			}
		}
		if (isPress(RightKey, LOW))
		{
			switch (AIColor)
			{
			case 'b':
				Lcd.setCursor(0, 1);
				Lcd.print("   Red  >Black< ");
				AIColor = 'r';
				break;
			case 'r':
				AIColor = 'b';
				break;
			default:
				AIColor = 'r';
				break;
			}
		}
	}
}

void start()
{
}

void play()
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

void reset()
{
}
