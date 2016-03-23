const unsigned char pulsePin1 = 10, dirPin1 = 11;
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
	delayMicroseconds(20);
	digitalWrite(pulsePin1, LOW);
	delayMicroseconds(20);
}

void loop()
{
	static char flag = 0;
	setDir(flag ^= 1);
	digitalWrite(13, flag);
	for (int i = 0; i < 1024;++i)
	{
		step1();
		delayMicroseconds(500);
	}
	delay(1000);
}
