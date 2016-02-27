#ifndef __SLAVE_MAIN_H__
#define __SLAVE_MAIN_H__

#include "CommonDef.h"
#include "SlaveDef.h"

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

#endif // __SLAVE_MAIN_H__