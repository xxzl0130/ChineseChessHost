#include "../slave/CommonDef.h"

// ��ʼ/��Ͱ��� int.0
#define StartKey	2
// ����/���䰴�� int.1
#define EndKey		3
// �󰴼�
#define LeftKey		4
// �Ұ���
#define RightKey	5

// ��ʼ��GPIO Pin
void initPin();
// ��ⰴ���Ƿ��£�Ĭ�ϼ��ߵ�ƽ
bool isPress(uint8_t pin, uint8_t state = HIGH);
// ��ʼ������
bool initSerial();

void setup()
{
	initPin();
	Serial.begin(baudRate);
}

// ѭ����ⰴ������Slave����
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
	String tmp;
	Serial.begin(baudRate);
	if (!Serial)
		return false;
	for (uchr i = 0; i < 5; ++i)
	{
		tmp = Serial.readString();
		//comSer.println(tmp);
		if (strstr(tmp.c_str(), testComHost) != NULL)
		{ // �ҵ�Ӧ���ַ�������ʾ���ӳɹ�
			Serial.println(testComSlave); // �ظ�
			return true;
		}
		delay(100);
	}
	return false;
}