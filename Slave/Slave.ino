#include <Wire\Wire.h>
#include <LiquidCrystal_I2C\LiquidCrystal_I2C.h>
#include <String.h>
#include "CommonDef.h"
#include "SlaveDef.h"
#include "StepperMotor.h"
#include "SlaveMain.h"
#include "MatrixKeyboard.h"
#include "ChessBoard.h"
#include "SlipTable.h"
/*
Todo list:

###������͡������߼�
###�ж����޷�ʹ�ô���

// ��ʼ
void start();
// ��Ϸ����
void playing(); // ��Ҫ����
// ��������
void reset();

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
// ��Ϸ״̬ 0 ������1 ��ͣ�2 ����
GameState state = Play;
// ����
ChessBoard board;

// ������ƶ�����
bool humanMoveChess();
// ִ������
void executeOrder(String& order);
// ��� flag:false �� true ��
bool draw(bool flag);
// �ж��ú��� ����draw
void callDraw();
// �ж��ú��� ����resign
void callResign();
// ���� flag:false �� true ��
bool resign(bool flag);
// �ȴ���ʼ
void waitStart();
// ѡ���Ѷ�
void selectDiff();
// ѡ��˳��
void selectOrder();
// ��ʼ
void start();
// ��Ϸ����
void playing();
// ��������
void reset();
// ������Ϸ
void end(GameState state);

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
	playing();
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
	board.moveChess(move);
}

bool draw(bool flag)
{
	++drawCnt;
	if (drawCnt >= 2)
	{// ˫��ͬ�����
		drawCnt = 0;
		end(Draw);
		return true;
	}
	if (flag == false)
	{// �����
		sendBoard(board.board,Draw);
	}
	else
	{// �������
		Lcd.clear();
		Lcd.setCursor(0, 0);
		Lcd.print("Computer draw.");
		Lcd.setCursor(0, 1);
		Lcd.print(" Agree  Reject  ");
		while(true)
		{
			if(Serial2.available())
			{
				auto t = Serial.read();
				if(t == StartKey)
				{
					draw(false);
					break;
				}
				else if(t == EndKey)
				{
					Lcd.setCursor(0, 1);
					Lcd.print(" You Rejected.");
					drawCnt = 0;
					delay(1000);
					break;
				}
			}
		}
	}
	return false;
}

void callDraw()
{
	if(isPress(StartKey,LOW))
	{
		draw(false);
	}
}

void callResign()
{
	if (isPress(EndKey, LOW))
	{
		resign(false);
	}
}

bool resign(bool flag)
{
	static bool saveFlag;
	++resignCnt;
	if (resignCnt >= 2)
	{// ˫��ͬ������
		resignCnt = 0;
		if(saveFlag == false)
		{
			end(Lose);
		}
		else
		{
			end(Win);
		}
		return true;
	}
	if (flag == false)
	{// ������
		sendBoard(board.board,Resign);
	}
	else
	{// ��������
		Lcd.clear();
		Lcd.setCursor(0, 0);
		Lcd.print("Computer resign.");
		Lcd.setCursor(0, 1);
		Lcd.print("CONGRATUALTIONS!");
		delay(1000);
		end(Win);
	}
	saveFlag = flag;
	return false;
}

void waitStart()
{
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print(" Chinese Chess");
	Lcd.setCursor(0, 1);
	Lcd.print("Press Start key");
	// �ȴ����¿�ʼ��ť
	while (true)
	{
		if(Serial2.available())
		{
			auto t = Serial2.read();
			if(t == StartKey)
			{
				break;
			}
		}
	}
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
		if (Serial2.available())
		{
			auto t = Serial2.read();
			if (t == StartKey)
			{
				// �Ѷ�ѡ�����
				break;
			}
			if (t == LeftKey)
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
			if (t == RightKey, LOW)
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
		if (Serial2.available())
		{
			auto t = Serial2.read();
			if (t == StartKey)
			{
				// �Ⱥ���ѡ�����
				break;
			}
			if (t == LeftKey)
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
			if (t == RightKey)
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
}

void start()
{
}

void playing()
{
	while(true)
	{
		if (humanMoveChess())
		{
			sendBoard(board.board);
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
}

void reset()
{
}

void end(GameState state)
{
	Lcd.clear();
	Lcd.setCursor(0, 0);
	// ��ʾ��ʾ��Ϣ
	switch(state)
	{
	case Draw:
		Lcd.print("Game End!Draw!");
		break;
	case Win:
		Lcd.print("You Win!");
		break;
	case Lose:
		Lcd.print("You lose!");
		break;
	default:
		break;
	}
	Lcd.setCursor(0, 1);
	Lcd.print("Play again?");
	while(true)
	{
		if (Serial2.available())
		{
			auto t = Serial2.read();
			if (t == StartKey, LOW)
			{
				reset();
				break;
			}
		}
	}
}