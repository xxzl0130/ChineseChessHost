#include <Wire\Wire.h>
#include <LiquidCrystal_I2C\LiquidCrystal_I2C.h>
#include <String.h>
#include "CommonDef.h"
#include "SlaveDef.h"
#include "StepperMotor.h"
#include "MatrixKeyboard.h"
#include "ChessBoard.h"
#include "SlipTable.h"
#include "ctype.h"
/*
Todo list:

## 检测棋子的函数

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
// 棋盘描述，2层，保存上一次扫描的状态
char board[2][BoardRow][BoardCol] = {
	{
	{ r,h,e,a,k,a,e,h,r },// 9
	{ b,b,b,b,b,b,b,b,b },// 8
	{ b,c,b,b,b,b,b,c,b },// 7
	{ p,b,p,b,p,b,p,b,p },// 6
	{ b,b,b,b,b,b,b,b,b },// 5
	{ b,b,b,b,b,b,b,b,b },// 4
	{ P,b,P,b,P,b,P,b,P },// 3
	{ b,C,b,b,b,b,b,C,b },// 2
	{ b,b,b,b,b,b,b,b,b },// 1
	{ R,H,E,A,K,A,E,H,R } // 0
//    a b c d e f g h i
	},
	{
	{ r,h,e,a,k,a,e,h,r },// 9
	{ b,b,b,b,b,b,b,b,b },// 8
	{ b,c,b,b,b,b,b,c,b },// 7
	{ p,b,p,b,p,b,p,b,p },// 6
	{ b,b,b,b,b,b,b,b,b },// 5
	{ b,b,b,b,b,b,b,b,b },// 4
	{ P,b,P,b,P,b,P,b,P },// 3
	{ b,C,b,b,b,b,b,C,b },// 2
	{ b,b,b,b,b,b,b,b,b },// 1
	{ R,H,E,A,K,A,E,H,R } // 0
//   a b c d e f g h i
	}
};
// 当前使用的棋盘描述
uchr curBoardNo = 0;
String tmp;
char buf[MAX_BUF_SIZE];
// 对局回合数
ulong roundCnt;
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
// 棋盘
ChessBoard chessBoard;

// 检测人移动棋子
bool humanMoveChess();
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
void boardDescribe(char board[BoardRow][BoardCol], char* buf, uchr& len);
// 生成FEN格式串
void creatFEN(char board[BoardRow][BoardCol], char* buf, char turn, ulong round);
// 从主机接收命令
String readOrderFromHost();
// 发送go（思考）命令。state:0 正常，1 求和，2 认输
void sendGo(uchr state = 0);
// 发送棋盘描述。state:0 正常，1 求和，2 认输
void sendBoard(char board[BoardRow][BoardCol], GameState state = Play);
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
	for (auto i = 0; i <= RowCnt;++i)
	{
		digitalWrite(RowStart + i, HIGH);
		for (auto j = 0; j <= ColCnt;++j)
		{
			if (static_cast<bool>(digitalRead(ColStart + j) == HIGH) !=
			    static_cast<bool>(board[curBoardNo][i][j] != b) &&
				isPress(ColStart + j))
			{
				// 有子拿起或落下

			}
		}
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
	char move[4];
	char* ptr = strstr(order.c_str(), "bestmove") + 9/*跳过"bestmove "*/;
	for (uint i = 0; i < 4; ++i)
	{
		move[i] = ptr[i];
	}
	// 走子
	chessBoard.moveChess(move);
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
		sendBoard(board[curBoardNo], Draw);
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
		sendBoard(board[curBoardNo], Resign);
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
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print(" Chinese Chess");
	Lcd.setCursor(0, 1);
	Lcd.print("Press Start key");
	// 等待按下开始按钮
	while (true)
	{
		if (isPress(StartKey, LOW))
		{
			break;
		}
	}
	selectDiff();
	selectOrder();
}

void selectDiff()
{
	// 选择难度
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print(" >Easy< Normal");
	Lcd.setCursor(0, 1);
	Lcd.print("  Hard  Master");
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
	while (true)
	{
		if (isPress(StartKey,LOW))
		{
			// 先后手选择完成
			break;
		}
		if (isPress(LeftKey,LOW))
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
		}
		if (isPress(RightKey,LOW))
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
		}
	}
}

void start()
{
}

void playing()
{
	while (true)
	{
		// 求和
		if(isPress(StartKey,LOW))
		{
			draw(false);
		}
		// 认输
		if(isPress(EndKey,LOW))
		{
			resign(false);
		}
		switch(gameState)
		{
			/*
			先列好各种情况，等待完善
			*/
		case Play:
			break;
		case PlayerHoldHis:
			break;
		case PlayerHoldOpp:
			break;
		case PlayerHoldTwo:
			break;
		case MoveDone:
			break;
		case WaitOrder:
			break;
		case Win:case Lose:case Draw:case Resign:
			// 这4个状态是已经进入gameOver的，所以结束本函数
			return;
		default:
			break;
		}
		/*
		if (humanMoveChess())
		{
			sendBoard(board[curBoardNo]);
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
		*/
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
	// 显示提示信息
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
			String tmp = comSer.readString();
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
	// 初始化与Host通信
	comSer.begin(generalBaudRate);
	if (!comSer)
		return false;
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
	// 初始化与Slave2通信
	/*
	Serial2.begin(generalBaudRate);
	if (!Serial2)
		return false;
	Serial.println(testComHost);
	for (uchr i = 0; i < 3; ++i)
	{
		tmp = comSer.readString();
		if (strstr(tmp.c_str(), testComSlave) != NULL)
		{ // 找到应有字符串，表示连接成功
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
	Lcd.backlight();//开启背光
	Lcd.noBlink();//无光标
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
	for (auto i = 0; i <= RowCnt; ++i)
	{
		pinMode(RowStart + i, OUTPUT);
	}
	for (auto i = 0; i <= ColCnt;++i)
	{
		pinMode(ColStart + i, INPUT);
	}
}
