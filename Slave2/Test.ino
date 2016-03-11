const unsigned char pulsePin1 = 4, dirPin1 = 5;
const unsigned char pulsePin2 = 6, dirPin2 = 7;

void setup()
{
	pinMode(pulsePin1, OUTPUT);
	pinMode(dirPin1, OUTPUT);
	pinMode(pulsePin2, OUTPUT);
	pinMode(dirPin2, OUTPUT);
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	delay(500);
}

inline void setDir(unsigned char dir)
{
	digitalWrite(dirPin1, dir);
	digitalWrite(dirPin2, dir);
}

void step1()
{
	digitalWrite(pulsePin1, HIGH);
	delayMicroseconds(10);
	digitalWrite(pulsePin1, LOW);
	delayMicroseconds(10);
}

void step2()
{
	digitalWrite(pulsePin2, HIGH);
	delayMicroseconds(10);
	digitalWrite(pulsePin2, LOW);
	delayMicroseconds(10);
}

void loop()
{
	static char flag = 0;
	setDir(flag ^= 1);
	for (int i = 0; i < 1600;++i)
	{
		step1();
		step2();
		delayMicroseconds(50);
	}
	delay(1000);
}
