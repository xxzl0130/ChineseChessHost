/*
测试步进电机工作方式
*/

const unsigned char pulsePin = 3, dirPin = 4, enPin = 5;

void setup()
{
	pinMode(pulsePin, OUTPUT);
	pinMode(dirPin, OUTPUT);
	pinMode(enPin, OUTPUT);
}

inline void setDir(unsigned char dir)
{
	digitalWrite(dirPin, dir);
}

void step1()
{
	digitalWrite(pulsePin, HIGH);
	delayMicroseconds(10);
	digitalWrite(pulsePin, LOW);
}

void step2()
{
	static unsigned char pulseState = 0;
	digitalWrite(pulsePin, pulseState ^= 1);
}

void loop()
{
	for (int i = 0; i < 100;++i)
	{
		step1();
		delay(10);
	}
	for (int i = 0; i < 10;++i)
	{
		step2();
		delay(10);
	}
}
