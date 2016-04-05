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
// 开始
void start();
// 游戏进行
void playing(); // 需要调整
// 重置棋盘
void reset();

*/

//LCD1602
LiquidCrystal_I2C Lcd(0x27, 16, 2);
/*
显示屏样式：
0123456789012345
******************
0*                *0x80
1*                *0xC0
******************
*/
// 棋盘描述
/*
Chess board[BoardRow][BoardCol] = {
						  // 行号 | 针脚序号(RowStart +)
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
*/
Chess board[BoardRow][BoardCol] = {
	// 行号 | 针脚序号(RowStart +)
	{ b,b,b,a,b,k,e,b,b },// 9    |       0
	{ b,b,b,b,a,H,b,b,b },// 8    |       1
	{ b,b,b,b,R,b,b,b,b },// 7    |       2
	{ b,b,b,b,b,b,b,b,b },// 6    |       3
	{ p,b,b,b,P,b,b,b,p },// 5    |       4
	{ b,b,b,b,b,h,P,b,b },// 4    |       5
	{ P,b,b,b,b,b,b,b,b },// 3    |       6
	{ b,b,b,b,E,b,b,b,b },// 2    |       7
	{ b,b,R,b,K,b,b,b,b },// 1    |       8
	{ b,b,b,A,b,A,E,b,b } // 0    |       9
						  //    a b c d e f g h i
};
// 玩家当前拿在手里的子，0为自己的子，1为电脑的子
ChessPoint chessHold[2];
String tmp;
char buf[MAX_BUF_SIZE];
// 对局回合数
ulong roundCnt = 0;
// 电脑执子
char AIColor = 'b', AIColorNumber = 'z';
// 提和计数
uchr drawCnt;
// 认输计数
uchr resignCnt;
// 难度
DIFFICULTY diff = easy;
// 游戏状态
GameState gameState = Play;
// 滑台
SlipTable table(StepperMotor(6, 7, circleStep), StepperMotor(8, 9, circleStep),
	boardLength, boardWidth, 46, 47, 48, 49, pitch);
// 升降台
StepperMotor upDownMotor(10, 11, circleStep);

// 检测拿起棋子
bool detectPickUpChess();
// 检测放下棋子
bool detectPutDownChess();
// 执行命令
void executeOrder(String& order);
// 求和 flag:false 人 true 机
bool draw(bool flag);
// 认输 flag:false 人 true 机
bool resign(bool flag);
// 等待开始
void waitStart();
// 选择难度
void selectDiff();
// 选择顺序
void selectOrder();
// 开始
void start();
// 游戏进行
void playing();
// 重置棋盘
void reset();
// 结束游戏
void gameOver(GameState state);
// 将局面变成FEN规范中的描述
void boardDescribe(Chess board[BoardRow][BoardCol], char* buf, uchr& len);
// 生成FEN格式串
void creatFEN(Chess board[BoardRow][BoardCol], char* buf, char turn, ulong round);
// 从主机接收命令
String readOrderFromHost();
// 发送go（思考）命令。state:0 正常，1 求和，2 认输
void sendGo(GameState state = Play);
// 发送棋盘描述。state:0 正常，1 求和，2 认输
void sendBoard(Chess board[BoardRow][BoardCol], GameState state = Play);
// 检测按键是否按下，默认检测高电平
bool isPress(uint8_t pin, uint8_t state = HIGH);
// 初始化串口
bool initSerial();
// 初始化棋盘，设置难度、先手等
void initBoard();
// 初始化LCD
void initLCD();
// 初始化GPIO Pin
void initPin();
// 初始化SDPlayer
void initSDPlayer();
// 移动棋子
void moveChess(String order);
// 播放音乐
void playAudio(char Filename[]);
// 返回一个可用的弃子区域
Point<double> getAvailableRecycleBin();
// 计算棋子的坐标
Point<double> getChessPos(char col, char row);
// 修改棋盘记录
void modifyBoard(Chess board[BoardRow][BoardCol], String order);
// 抓起棋子
void pickUpChess();
// 放下棋子
void putDownChess();
// 检查走子合法性
bool checkPath(String path, Chess chess);

void setup()
{
	initLCD();
	initPin();
	initSerial();
	initSDPlayer();
	//initBoard();
	// 跳线联通则不进行运动
	Lcd.setCursor(0, 1);
	Lcd.print("INIT ALL DONE   ");
	uchr flag = 0;
	digitalWrite(RowStart, HIGH);
	while (digitalRead(jumpPinB) == LOW)
	{
		digitalWrite(ledPin, flag ^= 1);
		for (int i = 0; i < ColCnt; ++i)
		{
			debugSer.print(digitalRead(ColStart + i));
		}
		debugSer.print("\n");
		delay(500);
	}
}

void loop()
{
	waitStart();
	start();
	playing();
	reset();
}

bool detectPickUpChess()
{
	for (int i = 0; i < RowCnt; ++i)
	{
		digitalWrite(RowStart + i, HIGH);
		for (int j = 0; j < ColCnt; ++j)
		{
			if (board[i][j] != b /*该处之前有子*/
				&& digitalRead(ColStart + j) == LOW /*现在此处无子*/
				&& isPress(ColStart + j) /*防抖检测*/)
			{
				if (isUpperCase(AIColorNumber) != isUpperCase(board[i][j]))
				{
					// 拿起的子不是电脑的子
					// 修改游戏状态
					gameState = PlayerHoldHis;
					// 记录拿起的是玩家的子
					chessHold[0] = ChessPoint(RowCnt - i - 1, j, board[i][j]);
				}
				else
				{
					gameState = PlayerHoldOpp;
					// 记录拿起的是电脑的子
					chessHold[1] = ChessPoint(RowCnt - i - 1, j, board[i][j]);
				}
				// 该位置修改为无子
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
			if (board[i][j] == b /*该处之前无子*/
				&& digitalRead(ColStart + j) == HIGH /*现在此处有子*/
				&& isPress(ColStart + j) /*防抖检测*/)
			{
				switch (gameState)
				{
				case PlayerHoldHis:
					if (RowCnt - i - 1 == chessHold[0].row && j == chessHold[0].col)
					{
						// 玩家把原来拿起来的子放下了
						gameState = Play;
						board[i][j] = chessHold[0].chess;
					}
					else
					{
						// 玩家落子
						gameState = MoveDone;
						board[i][j] = chessHold[0].chess;
					}
					break;
				case PlayerHoldOpp:
					if (RowCnt - i - 1 == chessHold[1].row && j == chessHold[1].col)
					{
						// 玩家把原来拿起来的子放下了
						gameState = Play;
						board[i][j] = chessHold[1].chess;
					}
					else
					{
						// ！！错误行为 需要提示
						gameState = Play;
						board[i][j] = chessHold[0].chess;
					}
					break;
				case PlayerHoldTwo:
					if (RowCnt - i - 1 == chessHold[0].row && j == chessHold[0].col)
					{
						// 玩家把原来拿起来的自己子放下了
						gameState = PlayerHoldOpp;
						board[i][j] = chessHold[0].chess;
					}
					else if (RowCnt - i - 1 == chessHold[1].row && j == chessHold[1].col)
					{
						// 玩家吃了子
						gameState = MoveDone;
						board[i][j] = chessHold[0].chess;
					}
				default:
					break;
				}
				// 计算走法
				char path[5] = { chessHold[0].col + 'a',chessHold[0].row + '0',
								'a' + j,'0' + i };
				if(checkPath(path,chessHold[0].chess))
				{
					return true;
				}
				else
				{
					// 需要提示
					gameState = Play;
					return false;
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
		// 引擎提和
		if (draw(true))
		{
			return;
		}
	}
	else
	{// 没有提和 清空计数
		drawCnt = 0;
	}
	if (strstr(order.c_str(), "resign") != NULL)
	{
		// 引擎认输
		if (resign(true))
		{
			return;
		}
	}
	else
	{// 没有认输 清空计数
		resignCnt = 0;
	}
	// 提取4位的走子信息
	char move[5] = { 0 };
	char* ptr = strstr(order.c_str(), "bestmove") + 9/*跳过"bestmove "*/;
	for (uint i = 0; i < 4; ++i)
	{
		move[i] = ptr[i];
	}
	// 走子
	moveChess(move);
	// 归位以避免影响视线
	//moveChess("a9a9");
	table.move(xAxisStart, yAxisStart);
}

bool draw(bool flag)
{
	++drawCnt;
	if (drawCnt >= 2)
	{// 双方同意和棋
		drawCnt = 0;
		gameOver(Draw);
		return true;
	}
	if (flag == false)
	{// 人提和
		sendBoard(board, Draw);
	}
	else
	{// 引擎提和
		Lcd.clear();
		Lcd.setCursor(0, 0);
		Lcd.print("Computer draw.");
		Lcd.setCursor(0, 1);
		Lcd.print(" Agree  Reject  ");
		while (true)
		{
			// 同意和棋
			if (isPress(StartKey, LOW))
			{
				// 相当于人也提出和棋，递归运行
				draw(false);
				break;
			}
			// 拒绝和棋
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
	// 记录认输是由谁提出的
	static bool saveFlag;
	++resignCnt;
	if (resignCnt >= 2)
	{// 双方同意认输
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
	{// 人认输
		sendBoard(board, Resign);
	}
	else
	{// 引擎认输
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
	// 等待按下开始按钮
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
	// 选择难度
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
			// 难度选择完成
			break;
		}
		// 按左降低难度
		if (isPress(LeftKey, LOW))
		{
			switch (diff)
			{
			case easy:
				// easy已无法降低
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
			delay(200);
		}
		// 按右键提高难度
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
				// master已无法提高难度
				diff = master;
				break;
			}
			delay(200);
		}
	}
}

void selectOrder()
{
	// 选择先后手
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print("Select Color:");
	Lcd.setCursor(0, 1);
	Lcd.print("  >Red<  Black  ");
	playAudio(OrderAudio);
	delay(500);
	while (true)
	{
		if (isPress(StartKey, LOW))
		{
			// 先后手选择完成
			break;
		}
		if (isPress(LeftKey, LOW))
		{
			switch (AIColor)
			{
			case 'b':
				// 默认电脑执黑棋
				AIColor = 'b';
				AIColorNumber = 'z';
				break;
			case 'r':
				// 切换到黑棋
				Lcd.setCursor(0, 1);
				Lcd.print("  >Red<  Black  ");
				// 玩家执红电脑执黑
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
				// 玩家执黑电脑执红
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
	// 游戏开始
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
	while (true)
	{
		// 求和
		if (isPress(StartKey, LOW))
		{
			draw(false);
		}
		// 认输
		if (isPress(EndKey, LOW))
		{
			resign(false);
		}
		switch (gameState)
		{
		case Play:
			detectPickUpChess();
			break;
		case PlayerHoldHis:case PlayerHoldOpp:case PlayerHoldTwo:
			detectPutDownChess();
			break;
		case MoveDone:
			// 提示当前为电脑回合
			Lcd.setCursor(0, 0);
			Lcd.print(" COMPUTER TURN  ");
			sendBoard(board);
			gameState = WaitOrder;
			break;
		case WaitOrder:
			// 如果主机发来了指令
			if (comSer.available())
			{
				tmp = readOrderFromHost();
				executeOrder(tmp);
				gameState = Play;
				// 提示当前为玩家回合
				Lcd.setCursor(0, 0);
				Lcd.print("     YOUR TURN  ");
			}
			break;
		case Win:case Lose:case Draw:case Resign:
			// 这4个状态是已经进入gameOver的，所以结束本函数
			return;
		default:
			break;
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
	// 显示提示信息
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
			String tmp = comSer.readString();
			if (strstr(tmp.c_str(), "bestmove") != NULL)
			{ // 找到bestmove
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
	{// 提和
		info += " draw";
	}
	else if (state == Resign)
	{// 认输
		info += " resign";
	}
	// 一次性发送所有信息
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
	Lcd.setCursor(0, 1);
	Lcd.print("    COM INIT   ");
	// 初始化与Host通信
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
		{ // 找到应有字符串，表示连接成功
			comSer.println(testComSlave); // 回复
			Lcd.setCursor(0, 1);
			Lcd.print("SERIAL OK!");
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
	Lcd.setCursor(0, 1);
	Lcd.print("BOARD OK!      ");
}

void initLCD()
{
	Lcd.init();
	Lcd.backlight();//开启背光
	Lcd.noBlink();//无光标
	Lcd.setCursor(0, 0);
	Lcd.print(" SYSTEM INIT");
}

void initPin()
{
	Lcd.setCursor(0, 1);
	Lcd.print("   PINS INIT   ");
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);
	// 默认LED熄灭
	pinMode(StartKey, INPUT_PULLUP);
	pinMode(EndKey, INPUT_PULLUP);
	pinMode(LeftKey, INPUT_PULLUP);
	pinMode(RightKey, INPUT_PULLUP);
	// 带上拉
	pinMode(MagnetUp, OUTPUT);
	pinMode(MagnetDown, OUTPUT);
	pinMode(jumpPinA, OUTPUT);
	// 输出低电平，如果跳线联通则B也为低电平
	pinMode(jumpPinB, INPUT_PULLUP);
	digitalWrite(jumpPinA, LOW);
	digitalWrite(MagnetUp, LOW);
	for (int i = 0; i < RowCnt; ++i)
	{
		pinMode(RowStart + i, OUTPUT);
		digitalWrite(RowStart + i, LOW);
	}
	for (char i = 0; i < ColCnt; ++i)
	{
		pinMode(ColStart + i, INPUT);
	}
}

void initSDPlayer()
{
	SdPlay.setWorkBuffer(static_cast<uint8_t*>(static_cast<void*>(buf)), MAX_BUF_SIZE);
	SdPlay.setSDCSPin(CS_PIN);
	if(!SdPlay.init(AudioMode))
	{
		debugSer.print(F("initialization failed:"));
		debugSer.println(SdPlay.getLastError());
	}
}

void moveChess(String order)
{
	Point<double> scr, dst;
	// 棋子坐标
	scr = getChessPos(order[0], order[1]);
	// 目标坐标
	dst = getChessPos(order[2], order[3]);
	if (order[0] == order[2] && order[1] == order[3])
	{
		table.move(dst);
		return;
	}
	if (board[9 - (order[0] - '0')][order[1] - 'a'] != b && // 原来有子
		board[9 - (order[3] - '0')][order[2] - 'a'] != b)// 目标处也有子，即要吃子
	{
		
		// 先移动到弃子处
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
	// 设置文件
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
		// 楚河另一侧 加上楚河宽度
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
	// 落下滑台
	upDownMotor.run(BACKWORD, zAxisStep,500);
	// 电磁铁正向通电
	digitalWrite(MagnetUp, HIGH);
	digitalWrite(MagnetDown, LOW);
	delay(500);
	// 等待主机发信号再移动
	while(true)
	{
		if (debugSer.available() && debugSer.read() > 0)
		{
			break;
		}
	}
	// 升起滑台
	upDownMotor.run(FORWORD, zAxisStep,500);
	
}

void putDownChess()
{
	// 落下滑台
	upDownMotor.run(BACKWORD, zAxisStep, 500);
	// 电磁铁反向通电
	digitalWrite(MagnetDown, HIGH);
	digitalWrite(MagnetUp, LOW);
	// 等待主机发信号再移动
	while (true)
	{
		if (debugSer.available() && debugSer.read() > 0)
		{
			break;
		}
	}
	// 升起滑台
	upDownMotor.run(FORWORD, zAxisStep, 500);
}

bool checkPath(String path, Chess chess)
{
	int x, y;
	switch(chess)
	{
	case k:case K:case a:case A://
		// 将/帅/仕/士限制相同
		if ((path[2] == 'd' && path[2] == 'e' && path[2] == 'f') && // 纵向
			(path[3] >= '7' && path[3] <= '9') || // 横向
			(path[3] >= '0' && path[3] <= '2'))   // 限制在田字格
			return true;
		else
		{
			return false;
		}
	case e:case E:
		x = path[2] - path[0];
		y = path[3] - path[1];
		if(abs(x) != 2 || abs(y) != 2)
		{// 检测田字
			return false;
		}
		if(board[path[1] - '9' + (y /2)][path[0] - 'a' + (x / 2)] != b)
		{// 检测象眼
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
			{// 检测日字
				return false;
			}
			if (board[path[1] - '9'][path[0] - 'a' + (x / 2)] != b)
			{// 检测马脚
				return false;
			}
			return true;
		}
		else
		{
			if (abs(x) != 1 || abs(y) != 2)
			{// 检测日字
				return false;
			}
			if (board[path[1] - '9' + (y / 2)][path[0] - 'a'] != b)
			{// 检测马脚
				return false;
			}
			return true;
		}
	case r:case R:
		if(path[2] == path[0])
		{// 竖着走
			int i = min(path[3], path[1]), j = max(path[1], path[3]);
			for (; i < j;++i)
			{
				if (board[i - '9'][path[0] - 'a'] != b)
				{// 检查一路上有没有子
					return false;
				}
			}
			return true;
		}
		else if(path[3] == path[1])
		{// 横着走
			int i = min(path[0], path[2]), j = max(path[0], path[2]);
			for (; i < j; ++i)
			{
				if (board[path[1] - '9'][j - 'a'] != b)
				{// 检查一路上有没有子
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
		{// 竖着走
			int i = min(path[3], path[1]), j = max(path[1], path[3]),count = 0;
			for (; i <= j; ++i)
			{
				if (board[i - '9'][path[0] - 'a'] != b)
				{// 检查一路上有几个子
					++count;
				}
			}
			// 走一步或者翻山打一个
			return count == 0 || count == 2;
		}
		else if (path[3] == path[1])
		{// 横着走
			int i = min(path[0], path[2]), j = max(path[0], path[2]), count = 0;
			for (; i <= j; ++i)
			{
				if (board[path[1] - '9'][j - 'a'] != b)
				{// 检查一路上有多少子
					++count;
				}
			}
			// 走一步或者翻山打一个
			return count == 0 || count == 2;
		}
		else
		{
			return false;
		}
	case p:
		if(path[1] >= '5')
		{// 还没过河
			if(path[2] == path[0] && path[3] == path[1] - 1)
			{
				// 只能前进
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{// 过河可以随便走了
			x = path[2] - path[0];
			y = path[3] - path[1];
			if(max(abs(x),abs(y)) >= 1)
			{
				//　只能走一格
				return false;
			}
			else
			{
				return true;
			}
		}
	case P:
		if (path[1] <= '4')
		{// 还没过河
			if (path[2] == path[0] && path[3] == path[1] + 1)
			{
				// 只能前进
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{// 过河可以随便走了
			x = path[2] - path[0];
			y = path[3] - path[1];
			if (max(abs(x), abs(y)) >= 1)
			{
				//　只能走一格
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
