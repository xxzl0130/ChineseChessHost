#include <Wire/src/Wire.h>
#include <LiquidCrystal_I2C/LiquidCrystal_I2C.h>
#include <string.h>
#include "CommonDef.h"
#include "SlaveDef.h"
#include "StepperMotor.h"
#include "SlipTable.h"
#include "SimpleSDAudio/SimpleSDAudio.h"
/*
Todo:
sdcard
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
// ��������
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
// ��¼���ӵ�ƽ״̬������
bool boardState[BoardRow][BoardCol];
// ��ҵ�ǰ����������ӣ�0Ϊ�Լ����ӣ�1Ϊ���Ե���
ChessPoint chessHold[2];
// �ַ�������
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
	boardLength, boardWidth, 46, 47, pitch);
// ����̨
StepperMotor upDownMotor(10, 11, circleStep);

bool detectMoveChess();
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
// ��ʼ������״̬
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
Point<double> getAvailableRecycleBin();
// �������ӵ�����
Point<double> getChessPos(char col, char row);
// �޸����̼�¼
void modifyBoard(Chess board[BoardRow][BoardCol], String order);
// ץ������
void pickUpChess();
// ��������
void putDownChess();
// ������ӺϷ���
bool checkPath(String path, Chess chess);
// ��������
void reconnect();
// ��������������
void rowClear();
// ����·��
String generatePath(ChessPoint scr, ChessPoint dst);

void setup()
{
	initLCD();
	initPin();
	initSerial();
	initSDPlayer();
	initBoard();
	// ������ͨ�򲻽����˶�
	Lcd.setCursor(0, 1);
	Lcd.print("INIT ALL DONE   ");
	
}

void loop()
{
	waitStart();
	start();
	playing();
	reset();
}

bool detectMoveChess()
{
	static ChessPoint lastChangeChess(-1, -1, b);
	rowClear();
	for (int i = 0;i < RowCnt;++i)
	{
		// ����һ��
		digitalWrite(RowStart + i, HIGH);
		for (int j = 0;j < ColCnt;++j)
		{
			if(boardState[i][j] != isPress(ColStart + j))
			{
				if(lastChangeChess.row == -1)
				{// ��ʼ״̬
					lastChangeChess = ChessPoint(i, j, board[i][j]);
					return false;
				}
				if(i == lastChangeChess.row && j == lastChangeChess.col)
				{// �������֮ǰ���µ���
					return false;
				}
				else if(checkPath(generatePath(lastChangeChess,ChessPoint(i,j,board[i][j])),lastChangeChess.chess))
				{// ����仯���Ǹ��Ϸ���·������Ϊ������
					// �޸�����
					modifyBoard(board, generatePath(lastChangeChess, ChessPoint(i, j, board[i][j])));
					return true;
				}
				else
				{
					lastChangeChess = ChessPoint(i, j, board[i][j]);
					return false;
				}
			}
		}
	}
	return false;
}

bool detectPickUpChess()
{
	for (int i = 0; i < RowCnt; ++i)
	{
		digitalWrite(RowStart + i, HIGH);
		for (int j = 0; j < ColCnt; ++j)
		{
			//if (board[i][j] != b /*�ô�֮ǰ����*/
			//	&& digitalRead(ColStart + j) == LOW /*���ڴ˴�����*/
			//	&& isPress(ColStart + j) /*�������*/)
			if(isPress(ColStart + j))
			{
				if (isUpperCase(AIColorNumber) != isUpperCase(board[i][j]))
				{
					// ������Ӳ��ǵ��Ե���
					// �޸���Ϸ״̬
					gameState = PlayerHoldHis;
					// ��¼���������ҵ���
					if(chessHold[0].chess != b)
					{// ���֮ǰ��¼���������ӣ�����Ϊ�Ż�
						board[chessHold[0].row][chessHold[0].col] = chessHold[0].chess;
					}
					chessHold[0] = ChessPoint(RowCnt - i - 1, j, board[i][j]);
					chessHold[0].row = i;
					chessHold[0].col = j;
				}
				else
				{
					gameState = PlayerHoldOpp;
					// ��¼������ǵ��Ե���
					if (chessHold[1].chess != b)
					{// ���֮ǰ��¼���������ӣ�����Ϊ�Ż�
						board[chessHold[1].row][chessHold[1].col] = chessHold[1].chess;
					}
					chessHold[1] = ChessPoint(RowCnt - i - 1, j, board[i][j]);
					chessHold[1].row = i;
					chessHold[1].col = j;
				}
				// ��λ���޸�Ϊ����
				board[i][j] = b;
				return true;
			}
		}
		digitalWrite(RowStart + i, LOW);
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
			//if (board[i][j] == b /*�ô�֮ǰ����*/
			//	&& digitalRead(ColStart + j) == HIGH /*���ڴ˴�����*/
			//	&& isPress(ColStart + j) /*�������*/)
			if (isPress(ColStart + j))
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

				if (gameState == MoveDone)
				{
					// �����߷�
					char path[5] = { chessHold[0].col + 'a',chessHold[0].row + '0',
									'a' + j,'0' + i };
					if (checkPath(path, chessHold[0].chess))
					{
						// ��ճ��Ӽ�¼
						memset(chessHold, 0, sizeof(chessHold));
						return true;
					}
					else
					{
						// ��Ҫ��ʾ
						gameState = Play;
						swap(path[0], path[2]);
						swap(path[1], path[3]);
						// ���ߴ������Ų��ȥ
						moveChess(path);
						return false;
					}
				}
			}
		}
		digitalWrite(RowStart + i, LOW);
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
	char move[5] = { 0 };
	char* ptr = strstr(order.c_str(), "bestmove") + 9/*����"bestmove "*/;
	for (uint i = 0; i < 4; ++i)
	{
		move[i] = ptr[i];
	}
	// ����
	moveChess(move);
	// ��λ�Ա���Ӱ������
	//moveChess("a9a9");
	table.move(xAxisStart, yAxisStart);
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
	debugSer.println("waitStart");
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print(" Chinese Chess");
	Lcd.setCursor(0, 1);
	Lcd.print("Press Start key");
	playAudio(WelcomeAudio);
	// �ȴ����¿�ʼ��ť
	while (true)
	{
		if (isPress(StartKey, LOW))
		{
			break;
		}
	}
	delay(100);
}

void selectDiff()
{
	// ѡ���Ѷ�
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print(" >Easy< Normal");
	Lcd.setCursor(0, 1);
	Lcd.print("  Hard  Master");
	playAudio(DifficultyAudio);
	delay(500);
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
				playAudio(EasyAudio);
				break;
			case normal:
				Lcd.setCursor(0, 0);
				Lcd.print(" >Easy< Normal ");
				diff = easy;
				playAudio(EasyAudio);
				break;
			case hard:
				Lcd.setCursor(0, 0);
				Lcd.print("  Easy >Normal<");
				Lcd.setCursor(0, 1);
				Lcd.print("  Hard  Master ");
				diff = normal;
				playAudio(NormalAudio);
				break;
			case master:
				Lcd.setCursor(0, 1);
				Lcd.print(" >Hard< Master ");
				diff = hard;
				playAudio(HardAudio);
				break;
			}
			delay(200);
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
				playAudio(NormalAudio);
				break;
			case normal:
				Lcd.setCursor(0, 0);
				Lcd.print("  Easy  Normal ");
				Lcd.setCursor(0, 1);
				Lcd.print(" >Hard< Master ");
				diff = hard;
				playAudio(HardAudio);
				break;
			case hard:
				Lcd.setCursor(0, 1);
				Lcd.print("  Hard >Master<");
				diff = master;
				playAudio(MasterAudio);
				break;
			case master:
				// master���޷�����Ѷ�
				diff = master;
				break;
			}
			delay(200);
		}
	}
	playAudio(DiffEndAudio);
}

void selectOrder()
{
	// ѡ���Ⱥ���
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print("Select Color:");
	Lcd.setCursor(0, 1);
	Lcd.print("  >Red<  Black  ");
	playAudio(OrderAudio);
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
			delay(200);
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
			delay(200);
		}
	}
}

void start()
{
	Lcd.clear();
	selectDiff();
	//selectOrder();
	// ��Ϸ��ʼ
	Lcd.clear();
	delay(100);
	Lcd.setCursor(0, 0);
	Lcd.print("  GAME START!  ");
	Lcd.setCursor(0, 1);
	Lcd.print(" DRAW   RESIGN ");
	delay(5000);
}

void playing()
{
	static unsigned long waitTime = millis(), hitTime = millis();
	static bool waitTimeFlag1, waitTimeFlag2,hitFlag = true;
	while (true)
	{
		if(millis() - hitTime > 5000)
		{// 5s���һ������
			if(hitFlag == true)
			{// �ϴεĻظ����յ�
				comSer.println(testComSlave);
				hitFlag = false;
			}
			else
			{
				reconnect();
			}
		}
		// ���
		if (isPress(StartKey, LOW))
		{
			draw(false);
		}
		// ����
		if (isPress(EndKey, LOW))
		{
			resign(false);
		}
		switch (gameState)
		{
		case Play:
			//detectPickUpChess();
			if (detectMoveChess())
			{
				gameState = MoveDone;
			}
			break;
		case PlayerHoldHis:case PlayerHoldOpp:case PlayerHoldTwo:
			detectPutDownChess();
			break;
		case MoveDone:
			// ��ʾ��ǰΪ���Իغ�
			Lcd.setCursor(0, 0);
			Lcd.print(" COMPUTER TURN  ");
			sendBoard(board);
			gameState = WaitOrder;
			waitTime = millis();
			// ��¼�ȴ�ָ���ʱ��
			waitTimeFlag1 = waitTimeFlag2 = false;
			break;
		case WaitOrder:
			// �������������ָ��
			if (comSer.available())
			{
				tmp = readOrderFromHost();
				executeOrder(tmp);
				gameState = Play;
				// ��ʾ��ǰΪ��һغ�
				Lcd.setCursor(0, 0);
				Lcd.print("     YOUR TURN  ");
			}
			if(millis() - waitTime > 5000 && !waitTimeFlag1)
			{// �ȴ�5s
				playAudio(WaitLongAudio1);
				waitTimeFlag1 = true;
			}
			if(millis() - waitTime > 30000 && !waitTimeFlag2)
			{
				playAudio(WaitLongAudio2);
				waitTimeFlag2 = true;
			}
			break;
		case Win:case Lose:case Draw:case Resign:
			// ��4��״̬���Ѿ�����gameOver�ģ����Խ���������
			return;
		default:
			break;
		}
		// ����������������Ӳ��Իظ�
		if (comSer.available())
		{
			tmp = comSer.readString();
			if (strstr(tmp.c_str(), testComHost) != NULL)
			{ // �ҵ�Ӧ���ַ�������ʾ���ӳɹ�
				hitFlag = true;
				break;
			}
		}
	}
	/*
	while(true)
	{
		while(true)
		{
			if(debugSer.available())
			{
				tmp = debugSer.readString();
				debugSer.println(tmp);
				break;
			}
		}
		digitalWrite(ledPin, buf[0] ^= 1);
		modifyBoard(board, tmp);
		sendBoard(board);
		tmp = readOrderFromHost();
		executeOrder(tmp);
	}
	*/
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
		playAudio(LoseAudio);
		break;
	case Win:
		Lcd.print("You Win!");
		playAudio(WinAudio);
		break;
	case Lose:
		Lcd.print("You lose!");
		playAudio(LoseAudio);
		break;
	default:
		break;
	}
	Lcd.setCursor(0, 1);
	Lcd.print("Play again?");
	while (true)
	{
		if(isPress(StartKey,LOW))
		{
			reset();
			break;
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
	String info = "go depth ";
	info += String(static_cast<int>(diff), 10);
	if (state == Draw)
	{// ���
		info += " draw";
	}
	else if (state == Resign)
	{// ����
		info += " resign";
	}
	// һ���Է���������Ϣ
	comSer.println(info);
}

void sendBoard(Chess board[BoardRow][BoardCol], GameState state)
{
	creatFEN(board, buf, AIColor, ++roundCnt);
	comSer.print("position fen ");
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
	Lcd.setCursor(0, 1);
	Lcd.print("    COM INIT   ");
	// ��ʼ����Hostͨ��
	comSer.begin(generalBaudRate);
	if (!comSer)
	{
		digitalWrite(ledPin, HIGH);
		delay(1000);
		return false;
	}
	while (!comSer.available());
	for (uchr i = 0; i < 3; ++i)
	{
		tmp = comSer.readString();
		if (strstr(tmp.c_str(), testComHost) != NULL)
		{ // �ҵ�Ӧ���ַ�������ʾ���ӳɹ�
			comSer.println(testComSlave); // �ظ�
			Lcd.setCursor(0, 1);
			Lcd.print("SERIAL OK!   ");
			break;
		}
		delay(100);
	}

#ifdef DEBUG
	debugSer.begin(generalBaudRate);
#endif
	return true;
}

void initBoard()
{
	table.reset();
	rowClear();
	// ��ȡ��ʼ���̽Ӵ�״̬
	for (int i = 0; i < RowCnt; ++i)
	{
		digitalWrite(RowStart + i, HIGH);
		for (int j = 0; j < ColCnt; ++j)
		{
			boardState[i][j] = isPress(ColStart + j);
		}
		digitalWrite(RowStart + i, LOW);
	}
} 

void initLCD()
{
	Lcd.init();
	Lcd.backlight();//��������
	Lcd.noBlink();//�޹��
	Lcd.setCursor(0, 0);
	Lcd.print(" SYSTEM INIT");
}

void initPin()
{
	Lcd.setCursor(0, 1);
	Lcd.print("   PINS INIT   ");
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);
	// Ĭ��LEDϨ��
	pinMode(StartKey, INPUT_PULLUP);
	pinMode(EndKey, INPUT_PULLUP);
	pinMode(LeftKey, INPUT_PULLUP);
	pinMode(RightKey, INPUT_PULLUP);
	// ������
	pinMode(MagnetUp, OUTPUT);
	pinMode(MagnetDown, OUTPUT);
	pinMode(jumpPinA, OUTPUT);
	// ����͵�ƽ�����������ͨ��BҲΪ�͵�ƽ
	pinMode(jumpPinB, INPUT_PULLUP);
	digitalWrite(jumpPinA, LOW);
	digitalWrite(MagnetUp, LOW);
	for (int i = 0; i < RowCnt; ++i)
	{
		pinMode(RowStart + i, OUTPUT);
		digitalWrite(RowStart + i, HIGH);
	}
	for (char i = 0; i < ColCnt; ++i)
	{
		pinMode(ColStart + i, INPUT_PULLUP);
	}
}

void initSDPlayer()
{
	Lcd.setCursor(0, 1);
	Lcd.print("  SDCARD INIT  ");
	SdPlay.setWorkBuffer(static_cast<uint8_t*>(static_cast<void*>(buf)), MAX_BUF_SIZE);
	SdPlay.setSDCSPin(CS_PIN);
	if(!SdPlay.init(AudioMode))
	{
		comSer.print(F("initialization failed:"));
		comSer.println(SdPlay.getLastError());
	}
	Lcd.setCursor(0, 1);
	Lcd.print("  SDCARD OK  ");
}

void moveChess(String order)
{
	Point<double> scr, dst;
	// ��������
	scr = getChessPos(order[0], order[1]);
	// Ŀ������
	dst = getChessPos(order[2], order[3]);
	if (order[0] == order[2] && order[1] == order[3])
	{
		table.move(dst);
		return;
	}
	if (board[9 - (order[0] - '0')][order[1] - 'a'] != b && // ԭ������
		board[9 - (order[3] - '0')][order[2] - 'a'] != b)// Ŀ�괦Ҳ���ӣ���Ҫ����
	{
		
		// ���ƶ������Ӵ�
		table.move(dst);
		pickUpChess();
		table.move(getAvailableRecycleBin());
		putDownChess();
	}
	table.move(scr);
	pickUpChess();
	table.move(dst);
	putDownChess();
	modifyBoard(board, order);
}

void playAudio(char Filename[])
{
	// �����ļ�
	//SdPlay.setFile(Filename);
	if (!SdPlay.setFile(Filename))
	{
		debugSer.print(Filename);
		debugSer.println(F(" not found on card! Error code: "));
		debugSer.println(SdPlay.getLastError());
	}
	SdPlay.worker();
	SdPlay.play();
	while (!SdPlay.isStopped())
	{
		SdPlay.worker();
	}
}

Point<double> getAvailableRecycleBin()
{
	Point<double> bin;
	static uchr list[10] = { 0 };
	for (int i = 0; i < 10; ++i)
	{
		if (list[i] < 2)
		{
			++list[i];
			bin.y = boxWidth * (i + 2);
			bin.x = boardLength + boxWidth * 2;
		}
	}
	return bin;
	//return Point<double>(0, 0);
}

Point<double> getChessPos(char col, char row)
{
	Point<double> pos(xAxisStart, yAxisStart);
	int _col = col - 'a', _row = 9 - (row - '0');
	pos.x += _col * boxWidth;
	pos.y += _row * boxLength;
	if(_row > 4)
	{
		// ������һ�� ���ϳ��ӿ��
		pos.y += riverWidth - boxLength;
	}
	return pos;
}

inline void modifyBoard(Chess board[BoardRow][BoardCol], String order)
{
	board[9 - (order[3] - '0')][order[2] - 'a'] = board[9 - (order[1] - '0')][order[0] - 'a'];
	board[9 - (order[1] - '0')][order[0] - 'a'] = b;
}

void pickUpChess()
{
	// ���»�̨
	upDownMotor.run(BACKWORD, zAxisStep,500);
	// ���������ͨ��
	digitalWrite(MagnetUp, HIGH);
	digitalWrite(MagnetDown, LOW);
	delay(200);
	// ����̨
	upDownMotor.run(FORWORD, zAxisStep,500);
}

void putDownChess()
{
	// ���»�̨
	upDownMotor.run(BACKWORD, zAxisStep, 500);
	// ���������ͨ��
	digitalWrite(MagnetDown, HIGH);
	digitalWrite(MagnetUp, LOW);
	delay(100);
	// ����̨
	upDownMotor.run(FORWORD, zAxisStep, 500);
}

bool checkPath(String path, Chess chess)
{
	int x, y;
	switch(chess)
	{
	case k:case K:case a:case A://
		// ��/˧/��/ʿ������ͬ
		if ((path[2] == 'd' && path[2] == 'e' && path[2] == 'f') && // ����
			(path[3] >= '7' && path[3] <= '9') || // ����
			(path[3] >= '0' && path[3] <= '2'))   // ���������ָ�
			return true;
		else
		{
			return false;
		}
	case e:case E:
		x = path[2] - path[0];
		y = path[3] - path[1];
		if(abs(x) != 2 || abs(y) != 2)
		{// �������
			return false;
		}
		if(board[path[1] - '9' + (y /2)][path[0] - 'a' + (x / 2)] != b)
		{// �������
			return false;
		}
		else
		{
			return false;
		}
	case h:case H:
		x = path[2] - path[0];
		y = path[3] - path[1];
		if(abs(x) > abs(y))
		{
			if(abs(x) != 2 || abs(y) != 1)
			{// �������
				return false;
			}
			if (board[path[1] - '9'][path[0] - 'a' + (x / 2)] != b)
			{// ������
				return false;
			}
			return true;
		}
		else
		{
			if (abs(x) != 1 || abs(y) != 2)
			{// �������
				return false;
			}
			if (board[path[1] - '9' + (y / 2)][path[0] - 'a'] != b)
			{// ������
				return false;
			}
			return true;
		}
	case r:case R:
		if(path[2] == path[0])
		{// ������
			int i = min(path[3], path[1]), j = max(path[1], path[3]);
			for (; i < j;++i)
			{
				if (board[i - '9'][path[0] - 'a'] != b)
				{// ���һ·����û����
					return false;
				}
			}
			return true;
		}
		else if(path[3] == path[1])
		{// ������
			int i = min(path[0], path[2]), j = max(path[0], path[2]);
			for (; i < j; ++i)
			{
				if (board[path[1] - '9'][j - 'a'] != b)
				{// ���һ·����û����
					return false;
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	case c:case C:
		if (path[2] == path[0])
		{// ������
			int i = min(path[3], path[1]), j = max(path[1], path[3]),count = 0;
			for (; i <= j; ++i)
			{
				if (board[i - '9'][path[0] - 'a'] != b)
				{// ���һ·���м�����
					++count;
				}
			}
			// ��һ�����߷�ɽ��һ��
			return count == 0 || count == 2;
		}
		else if (path[3] == path[1])
		{// ������
			int i = min(path[0], path[2]), j = max(path[0], path[2]), count = 0;
			for (; i <= j; ++i)
			{
				if (board[path[1] - '9'][j - 'a'] != b)
				{// ���һ·���ж�����
					++count;
				}
			}
			// ��һ�����߷�ɽ��һ��
			return count == 0 || count == 2;
		}
		else
		{
			return false;
		}
	case p:
		if(path[1] >= '5')
		{// ��û����
			if(path[2] == path[0] && path[3] == path[1] - 1)
			{
				// ֻ��ǰ��
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{// ���ӿ����������
			x = path[2] - path[0];
			y = path[3] - path[1];
			if(max(abs(x),abs(y)) >= 1)
			{
				//��ֻ����һ��
				return false;
			}
			else
			{
				return true;
			}
		}
	case P:
		if (path[1] <= '4')
		{// ��û����
			if (path[2] == path[0] && path[3] == path[1] + 1)
			{
				// ֻ��ǰ��
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{// ���ӿ����������
			x = path[2] - path[0];
			y = path[3] - path[1];
			if (max(abs(x), abs(y)) >= 1)
			{
				//��ֻ����һ��
				return false;
			}
			else
			{
				return true;
			}
		}
	default:
		return false;
	}
}

void reconnect()
{
	while(true)
	{
		playAudio(DisconnectAudio);
		comSer.println(testComSlave);
		tmp = comSer.readString();
		if (strstr(tmp.c_str(), testComHost) != NULL)
		{ // �ҵ�Ӧ���ַ�������ʾ���ӳɹ�
			playAudio(ReconnectAudio);
			break;
		}
		delay(3000);
	}
}

void rowClear()
{
	for (int i = 0;i < RowCnt;++i)
	{
		digitalWrite(RowStart + i, LOW);
	}
}

String generatePath(ChessPoint scr, ChessPoint dst)
{
	String tmp;
	tmp += 'a' + scr.col;
	tmp += '0' + (9 - scr.row);
	tmp += 'a' + dst.col;
	tmp += '0' + (9 - dst.row);
	return tmp;
}
