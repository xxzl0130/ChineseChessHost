/*
测试步进电机工作方式
*/

const unsigned char pulsePin = 3, dirPin = 4, enPin = 5;

void setup()
{
	pinMode(pulsePin, OUTPUT);
	pinMode(dirPin, OUTPUT);
	pinMode(enPin, OUTPUT);
	digitalWrite(enPin, HIGH);
	pinMode(13, OUTPUT);
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
	delayMicroseconds(10);
}

void step2()
{
	static unsigned char pulseState = 0;
	digitalWrite(pulsePin, pulseState ^= 1);
}

void loop()
{
	setDir(HIGH);
	digitalWrite(13, HIGH);
	for (int i = 0; i < 6400;++i)
	{
		step1();
		//delay(30);
		delayMicroseconds(50);
	}
	digitalWrite(13, LOW);
	delay(1000);
}
