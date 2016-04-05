#include"DataSelection.h"

DataSelection data(36, 32, 3, 35, INPUT);

void setup()
{
	for (int i = 22; i <= 32;++i)
	{
		pinMode(i, OUTPUT);
		digitalWrite(i, HIGH);
	}
	pinMode(13, OUTPUT);
	Serial.begin(9600);
}


void loop()
{
	static char k = 0;
	digitalWrite(22, k ^= 1);
	digitalWrite(13, k);
	for (int i = 0; i < 8;++i)
	{
		data.setPort(i);
		delay(1);
		Serial.print((int)data.getDigitalRead());
	}
	Serial.print("\n");
	delay(1000);
}
