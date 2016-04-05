#ifndef __DATA_SELECTION_H__
#define __DATA_SELECTION_H__

#include <Arduino.h>

class DataSelection
{
private:
	// 使能针脚
	char enPin;
	// 选择针脚
	char sPin;
	// 数据针脚
	char dPin;
	// 选择针脚总数
	char sCnt;
public:
	DataSelection(char en, char s, char cnt, char d, char mode = INPUT);
	// 使能
	void enable() const;
	void disable() const;
	// 设置选择序号
	void setPort(char port) const;
	// 读取数据
	char getDigitalRead() const;
	unsigned int getAnalogRead();
};

#endif //__DATA_SELECTION_H__