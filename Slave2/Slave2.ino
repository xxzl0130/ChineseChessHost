#include "../slave/CommonDef.h"

// 开始/求和按键 int.0
#define StartKey	2
// 结束/认输按键 int.1
#define EndKey		3
// 左按键
#define LeftKey		4
// 右按键
#define RightKey	5

// 初始化GPIO Pin
void initPin();
// 检测按键是否按下，默认检测高电平
bool isPress(uint8_t pin, uint8_t state = HIGH);
// 初始化串口
bool initSerial();

void setup()
{
	initPin();
	Serial.begin(baudRate);
}

// 循环检测按键，向Slave发送
void loop()
{
	if(isPress(StartKey, LOW))
	{
		Serial.write(StartKey);
	}
	if (isPress(EndKey, LOW))
	{
		Serial.write(EndKey);
	}
	if (isPress(LeftKey, LOW))
	{
		Serial.write(LeftKey);
	}
	if (isPress(RightKey, LOW))
	{
		Serial.write(RightKey);
	}
}

void initPin()
{
	pinMode(StartKey, INPUT_PULLUP);
	pinMode(EndKey, INPUT_PULLUP);
	pinMode(LeftKey, INPUT_PULLUP);
	pinMode(RightKey, INPUT_PULLUP);
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
	String tmp;
	Serial.begin(baudRate);
	if (!Serial)
		return false;
	for (uchr i = 0; i < 5; ++i)
	{
		tmp = Serial.readString();
		//comSer.println(tmp);
		if (strstr(tmp.c_str(), testComHost) != NULL)
		{ // 找到应有字符串，表示连接成功
			Serial.println(testComSlave); // 回复
			return true;
		}
		delay(100);
	}
	return false;
}