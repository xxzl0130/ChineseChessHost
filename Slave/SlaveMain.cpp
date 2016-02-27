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
			{// 该位置没有子，增加计数
				++cnt;
			}
			else
			{// 计数不为0，先将计数加入
				if (cnt != 0)
				{
					// cnt不会大于10
					buf[len++] = '0' + cnt;
					cnt = 0;
				}
				buf[len++] = board[i][j];
			}
		}
		if (cnt != 0)
		{
			// cnt不会大于10
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
	// 该谁下
	buf[len++] = ' ';
	buf[len++] = turn;
	// 不变的参数
	buf[len++] = ' ';
	buf[len++] = '-';
	buf[len++] = ' ';
	buf[len++] = '-';
	buf[len++] = ' ';
	buf[len++] = '0';
	buf[len++] = ' ';
	// 局数
	sprintf(buf + len, "%lu", round);
}

String readOrderFromHost()
{
	while (true)
	{
		if (comSer.available())
		{
			tmp = comSer.readString();
			if (strstr(tmp.c_str(), "bestmove") != NULL)
			{ // 找到bestmove
				return tmp;
			}
		}
		delay(100);
	}
}

void sendGo(uchr state)
{
	comSer.print("go depth ");
	comSer.print(diff);
	if (state == 1)
	{// 提和
		comSer.print(" draw");
	}
	else if (state == 2)
	{
		comSer.print(" resign");
	}
	comSer.print('\n');
}

void sendBoard(char board[BoardRow][BoardCol], GameState state)
{
	creatFEN(board, buf, AIColor, ++roundCnt);
	comSer.println(buf);
	sendGo(state);
}
bool isPress(uint8_t pin, uint8_t state)
{
	if (digitalRead(pin) == state)
	{
		// 延时消除抖动
		delay(10);
		if (digitalRead(pin) == state)
		{
			return true;
		}
	}
	return false;
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
		{ // 找到应有字符串，表示连接成功
			comSer.println(testComSlave); // 回复
			Lcd.setCursor(0, 1);
			Lcd.print("SERIAL OK!");
			return true;
		}
		delay(100);
	}
	return false;
}

void initBoard()
{
	Lcd.setCursor(0, 1);
	Lcd.print("BOARD OK!");
}

void initLCD()
{
	Lcd.init();
	Lcd.backlight();//开启背光
	Lcd.noBlink();//无光标
	Lcd.setCursor(1, 0);
	Lcd.print("SYSTEM INIT");
}

void initPin()
{
	pinMode(StartKey, INPUT_PULLUP);
	pinMode(EndKey, INPUT_PULLUP);
	pinMode(LeftKey, INPUT_PULLUP);
	pinMode(RightKey, INPUT_PULLUP);
	pinMode(ledPin, OUTPUT);
	digitalWrite(13, LOW);
}