#include <Wire/Wire.h>
#include <LiquidCrystal_I2C\LiquidCrystal_I2C.h>
#include <string.h>
#include "CommonDef.h"
#include "SlaveDef.h"
#include "StepperMotor.h"
#include "SlipTable.h"
#include "SimpleSDAudio\SimpleSDAudio.h"
/*
Todo list:
// ��ʼ
void start();
// ��Ϸ����
void playing(); // ��Ҫ����
// ��������
void reset();

getAvailableRecycleBin()

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
// ����������2�㣬������һ��ɨ���״̬
Chess board[BoardRow][BoardCol] = {
	// �к� | ������(RowStart +)
	{ r,h,e,a,k,a,e,h,r },// 9    |       0
	{ b,b,b,b,b,b,b,b,b },// 8    |       1
	{ b,c,b,b,b,b,b,c,b },// 7    |       2
	{ p,b,p,b,p,b,p,b,p },// 6    |       3
	{ b,b,b,b,b,b,b,b,b },// 5    |       4
	{ b,b,b,b,b,b,b,b,b },// 4    |       5
	{ P,b,P,b,P,b,P,b,P },// 3    |       6
	{ b,C,b,b,b,b,b,C,b },// 2    |       7
	{ b,b,b,b,b,b,b,b,b },// 1    |       8
	{ R,H,E,A,K,A,E,H,R } // 0    |       9
//    a b c d e f g h i
};
// ��ҵ�ǰ����������ӣ�0Ϊ�Լ����ӣ�1Ϊ���Ե���
ChessPoint chessHold[2];
String tmp;
char buf[MAX_BUF_SIZE];
// �Ծֻغ���
ulong roundCnt = 0;
// ����ִ��
char AIColor = 'b', AIColorNumber = 'z';
// ��ͼ���
uchr drawCnt;
// �������
uchr resignCnt;
// �Ѷ�
DIFFICULTY diff = easy;
// ��Ϸ״̬
GameState gameState = Play;
// ��̨
SlipTable table(StepperMotor(6, 7, circleStep), StepperMotor(8, 9, circleStep),
	boardLength, boardWidth, 46, 47, 48, 49, pitch);
// ����̨
StepperMotor upDownMotor(10, 11, circleStep);
// �����
#define upMagnet 49

// �����������
bool detectPickUpChess();
// ����������
bool detectPutDownChess();
// ִ������
void executeOrder(String& order);
// ��� flag:false �� true ��
bool draw(bool flag);
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
void gameOver(GameState state);
// ��������FEN�淶�е�����
void boardDescribe(Chess board[BoardRow][BoardCol], char* buf, uchr& len);
// ����FEN��ʽ��
void creatFEN(Chess board[BoardRow][BoardCol], char* buf, char turn, ulong round);
// ��������������
String readOrderFromHost();
// ����go��˼�������state:0 ������1 ��ͣ�2 ����
void sendGo(GameState state = Play);
// ��������������state:0 ������1 ��ͣ�2 ����
void sendBoard(Chess board[BoardRow][BoardCol], GameState state = Play);
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
// ��ʼ��SDPlayer
void initSDPlayer();
// �ƶ�����
void moveChess(String order);
// ��������
void playAudio(char Filename[]);
// ����һ�����õ���������
Point<float> getAvailableRecycleBin();
// �������ӵ�����
Point<float> getChessPos(char col, char row);

void setup()
{
	initPin();
	//initLCD();
	initSerial();
	//initBoard();
	// ������ͨ�򲻽����˶�
	uchr flag = 0;
	while (digitalRead(jumpPinB) == LOW)
	{
		digitalWrite(ledPin, flag ^= 1);
		delay(300);
	}
}

void loop()
{
	/*
	waitStart();
	start();
	playing();
	reset();
	*/
	static uchr flag = 0;
	/*float pos[2][2] = { 0,0,50,50 };
	table.move(pos[flag][0], pos[flag][1]);*/
	playing();
	digitalWrite(ledPin, flag ^= 1);
	delay(1000);
}

bool detectPickUpChess()
{
	for (int i = 0; i < RowCnt; ++i)
	{
		digitalWrite(RowStart + i, HIGH);
		for (int j = 0; j < ColCnt; ++j)
		{
			if (board[i][j] != b /*�ô�֮ǰ����*/
				&& digitalRead(ColStart + j) == LOW /*���ڴ˴�����*/
				&& isPress(ColStart + j, LOW) /*�������*/)
			{
				if (isUpperCase(AIColorNumber) != isUpperCase(board[i][j]))
				{
					// ������Ӳ��ǵ��Ե���
					// �޸���Ϸ״̬
					gameState = PlayerHoldHis;
					// ��¼���������ҵ���
					chessHold[0] = ChessPoint(RowCnt - i - 1, j, board[i][j]);
				}
				else
				{
					gameState = PlayerHoldOpp;
					// ��¼������ǵ��Ե���
					chessHold[1] = ChessPoint(RowCnt - i - 1, j, board[i][j]);
				}
				// ��λ���޸�Ϊ����
				board[i][j] = b;
				return true;
			}
		}
	}
	return false;
}

bool detectPutDownChess()
{
	for (int i = 0; i < RowCnt; ++i)
	{
		digitalWrite(RowStart + i, HIGH);
		for (int j = 0; j < ColCnt; ++j)
		{
			if (board[i][j] == b /*�ô�֮ǰ����*/
				&& digitalRead(ColStart + j) == HIGH /*���ڴ˴�����*/
				&& isPress(ColStart + j, HIGH) /*�������*/)
			{
				switch (gameState)
				{
				case PlayerHoldHis:
					if (RowCnt - i - 1 == chessHold[0].row && j == chessHold[0].col)
					{
						// ��Ұ�ԭ�����������ӷ�����
						gameState = Play;
						board[i][j] = chessHold[0].chess;
					}
					else
					{
						// �������
						gameState = MoveDone;
						board[i][j] = chessHold[0].chess;
					}
					break;
				case PlayerHoldOpp:
					if (RowCnt - i - 1 == chessHold[1].row && j == chessHold[1].col)
					{
						// ��Ұ�ԭ�����������ӷ�����
						gameState = Play;
						board[i][j] = chessHold[1].chess;
					}
					else
					{
						// ����������Ϊ ��Ҫ��ʾ
						gameState = Play;
						board[i][j] = chessHold[0].chess;
					}
					break;
				case PlayerHoldTwo:
					if (RowCnt - i - 1 == chessHold[0].row && j == chessHold[0].col)
					{
						// ��Ұ�ԭ�����������Լ��ӷ�����
						gameState = PlayerHoldOpp;
						board[i][j] = chessHold[0].chess;
					}
					else if (RowCnt - i - 1 == chessHold[1].row && j == chessHold[1].col)
					{
						// ��ҳ�����
						gameState = MoveDone;
						board[i][j] = chessHold[0].chess;
					}
				default:
					break;
				}
				return true;
			}
		}
	}
	return false;
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
		gameOver(Draw);
		return true;
	}
	if (flag == false)
	{// �����
		sendBoard(board, Draw);
	}
	else
	{// �������
		Lcd.clear();
		Lcd.setCursor(0, 0);
		Lcd.print("Computer draw.");
		Lcd.setCursor(0, 1);
		Lcd.print(" Agree  Reject  ");
		while (true)
		{
			// ͬ�����
			if (isPress(StartKey, LOW))
			{
				// �൱����Ҳ������壬�ݹ�����
				draw(false);
				break;
			}
			// �ܾ�����
			else if (isPress(EndKey, LOW))
			{
				Lcd.setCursor(0, 1);
				Lcd.print(" You Rejected.");
				drawCnt = 0;
				delay(1000);
				break;
			}
		}
	}
	return false;
}

bool resign(bool flag)
{
	// ��¼��������˭�����
	static bool saveFlag;
	++resignCnt;
	if (resignCnt >= 2)
	{// ˫��ͬ������
		resignCnt = 0;
		if (saveFlag == false)
		{
			gameOver(Lose);
		}
		else
		{
			gameOver(Win);
		}
		return true;
	}
	if (flag == false)
	{// ������
		sendBoard(board, Resign);
	}
	else
	{// ��������
		Lcd.clear();
		Lcd.setCursor(0, 0);
		Lcd.print("Computer resign.");
		Lcd.setCursor(0, 1);
		Lcd.print("CONGRATUALTIONS!");
		delay(1000);
		gameOver(Win);
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
		if (isPress(StartKey, LOW))
		{
			break;
		}
	}
	
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
		// ���󽵵��Ѷ�
		if (isPress(LeftKey, LOW))
		{
			switch (diff)
			{
			case easy:
				// easy���޷�����
				Lcd.setCursor(0, 0);
				Lcd.print(" >Easy< Normal ");
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
				Lcd.setCursor(0, 1);
				Lcd.print("  Hard  Master ");
				diff = normal;
				break;
			case master:
				Lcd.setCursor(0, 1);
				Lcd.print(" >Hard< Master ");
				diff = hard;
				break;
			}
		}
		// ���Ҽ�����Ѷ�
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
				Lcd.setCursor(0, 0);
				Lcd.print("  Easy  Normal ");
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
				// master���޷�����Ѷ�
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
				// Ĭ�ϵ���ִ����
				AIColor = 'b';
				AIColorNumber = 'z';
				break;
			case 'r':
				// �л�������
				Lcd.setCursor(0, 1);
				Lcd.print("  >Red<  Black  ");
				// ���ִ�����ִ��
				AIColor = 'b';
				AIColorNumber = 'z';
				break;
			default:
				AIColor = 'b';
				AIColorNumber = 'z';
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
				// ���ִ�ڵ���ִ��
				AIColor = 'r';
				AIColorNumber = 'Z';
				break;
			case 'r':
				AIColor = 'b';
				AIColorNumber = 'z';
				break;
			default:
				AIColor = 'r';
				AIColorNumber = 'Z';
				break;
			}
		}
	}
}

void start()
{
	selectDiff();
	selectOrder();
}

void playing()
{
	// Ԥ���趨�õ��߷�
	char order[10][5] = {
		"g9e7",//b2b4 
		"b9c7",//h2e2 
		"h9f8",//h0g2 
		"c6c5",//i0h0 
		"g6g5",//b0a2 
		"b7b0",//a0b0 
		"g5g4",//a2b0 
		"a9b9",//g3g4 
		"c5c4" //b4c4 
	};
	for (int i = 0; i < 9; ++i)
	{
		digitalWrite(ledPin, i & 1);
		moveChess(String(order[i]));
		delay(5000);
	}
}

void reset()
{
}

void gameOver(GameState state)
{
	Lcd.clear();
	Lcd.setCursor(0, 0);
	gameState = state;
	// ��ʾ��ʾ��Ϣ
	switch (state)
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
	while (true)
	{
		if (Serial2.available())
		{
			int t = Serial2.read();
			if (t == StartKey)
			{
				reset();
				break;
			}
		}
	}
}

void boardDescribe(Chess board[BoardRow][BoardCol], char* buf, uchr& len)
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
		}
		if (i != BoardRow - 1)
		{
			buf[len++] = '/';
		}
	}
}

void creatFEN(Chess board[BoardRow][BoardCol], char* buf, char turn, ulong round)
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

String readOrderFromHost()
{
	while (true)
	{
		if (comSer.available())
		{
			String tmp = comSer.readString();
			if (strstr(tmp.c_str(), "bestmove") != NULL)
			{ // �ҵ�bestmove
				return tmp;
			}
		}
		delay(100);
	}
}

void sendGo(GameState state)
{
	comSer.print("go depth ");
	comSer.print(diff);
	if (state == Draw)
	{// ���
		comSer.print(" draw");
	}
	else if (state == Resign)
	{
		comSer.print(" resign");
	}
	comSer.print('\n');
}

void sendBoard(Chess board[BoardRow][BoardCol], GameState state)
{
	creatFEN(board, buf, AIColor, ++roundCnt);
	comSer.println(buf);
	sendGo(state);
}

bool isPress(uint8_t pin, uint8_t state)
{
	if (digitalRead(pin) == state)
	{
		// ��ʱ��������
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
	// ��ʼ����Hostͨ��
	comSer.begin(generalBaudRate);
	if (!comSer)
	{
		digitalWrite(ledPin, HIGH);
		delay(1000);
		return false;
	}
#ifdef DEBUG
	comSer.println("Hello");
#endif
	/*while (!comSer.available());
	for (uchr i = 0; i < 3; ++i)
	{
		tmp = comSer.readString();
		if (strstr(tmp.c_str(), testComHost) != NULL)
		{ // �ҵ�Ӧ���ַ�������ʾ���ӳɹ�
			comSer.println(testComSlave); // �ظ�
			Lcd.setCursor(0, 1);
			Lcd.print("SERIAL OK!");
			break;
		}
		delay(100);
	}
	// ��ʼ����Slave2ͨ��
	
	Serial2.begin(generalBaudRate);
	if (!Serial2)
	return false;
	Serial.println(testComHost);
	for (uchr i = 0; i < 3; ++i)
	{
	tmp = comSer.readString();
	if (strstr(tmp.c_str(), testComSlave) != NULL)
	{ // �ҵ�Ӧ���ַ�������ʾ���ӳɹ�
	Lcd.setCursor(0, 1);
	Lcd.print("SERIAL2 OK!");
	return true;
	}
	delay(100);
	}*/
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
	Lcd.backlight();//��������
	Lcd.noBlink();//�޹��
	Lcd.setCursor(1, 0);
	Lcd.print("SYSTEM INIT");
}

void initPin()
{
	pinMode(ledPin, OUTPUT);
	digitalWrite(13, LOW);
	pinMode(StartKey, INPUT_PULLUP);
	pinMode(EndKey, INPUT_PULLUP);
	pinMode(LeftKey, INPUT_PULLUP);
	pinMode(RightKey, INPUT_PULLUP);
	pinMode(upMagnet, OUTPUT);
	pinMode(jumpPinA, OUTPUT);
	// ����͵�ƽ�����������ͨ��BҲΪ�͵�ƽ
	pinMode(jumpPinB, INPUT_PULLUP);
	digitalWrite(jumpPinA, LOW);
	digitalWrite(upMagnet, LOW);
	for (char i = 0; i <= RowCnt; ++i)
	{
		pinMode(RowStart + i, OUTPUT);
	}
	for (char i = 0; i <= ColCnt; ++i)
	{
		pinMode(ColStart + i, INPUT);
	}
}

void initSDPlayer()
{
	SdPlay.setWorkBuffer(static_cast<uint8_t*>(static_cast<void*>(buf)), MAX_BUF_SIZE);
	SdPlay.setSDCSPin(CS_PIN);
	SdPlay.init(AudioMode);
}

void moveChess(String order)
{
	Point<float> scr, dst;
	// ��������
	scr = getChessPos(order[0], order[1]);
	// Ŀ������
	dst = getChessPos(order[2], order[3]);
#ifdef DEBUG
	comSer.println(order);
	comSer.print("(");
	comSer.print(scr.x);
	comSer.print(",");
	comSer.print(scr.y);
	comSer.print("),");
	comSer.print("(");
	comSer.print(dst.x);
	comSer.print(",");
	comSer.print(dst.y);
	comSer.print(")\n");
#endif
	if (board[9 - (order[3] - '0')][order[2] - 'a'] != b)
	{
		// Ŀ�괦���ӣ���Ҫ����
		// ���ƶ������Ӵ�
#ifdef DEBUG
		comSer.print("eat");
		comSer.println(board[9 - (order[3] - '0')][order[2] - 'a']);
#endif
		table.move(dst);
		digitalWrite(upMagnet, HIGH);
		table.move(getAvailableRecycleBin());
		digitalWrite(upMagnet, LOW);
	}
	table.move(scr);
	digitalWrite(upMagnet, HIGH);
	table.move(dst);
	digitalWrite(upMagnet, LOW);
#ifdef DEBUG
	comSer.println("done");
#endif
}

void playAudio(char Filename[])
{
	// �����ļ�
	SdPlay.setFile(Filename);
	SdPlay.worker();
	SdPlay.play();
	while (!SdPlay.isStopped())
	{
		SdPlay.worker();
	}
}

Point<float> getAvailableRecycleBin()
{
	Point<float> bin;
	static uchr list[10] = { 0 };
	for (int i = 0; i < 10; ++i)
	{
		if (list[i] < 2)
		{
			++list[i];
			bin.y = boxWidth * (i + 1);
			bin.x = boardLength + boxWidth;
		}
	}
	return bin;
}

Point<float> getChessPos(char col, char row)
{
	Point<float> pos(xAxisStart, yAxisStart);
	int _col = col - 'a', _row = row - '0';
	pos.x += _col * boxWidth;
	pos.y += _row * boxLength;
	if(_row > 4)
	{
		// ������һ�� ���ϳ��ӿ��
		pos.y += riverWidth - boxLength;
	}
	return pos;
}
