#include "DataSelection.h"

DataSelection::DataSelection(char en, char s, char cnt, char d,char mode) :
				enPin(en),sPin(s),sCnt(cnt),dPin(d)
{
	digitalWrite(enPin, HIGH);
	for (int i = 0; i < cnt;++i)
	{
		pinMode(sPin + i, OUTPUT);
		digitalWrite(sPin + i, LOW);
	}
	pinMode(d, mode);
}

void DataSelection::enable() const
{
	// 低有效
	digitalWrite(enPin, LOW);
}

void DataSelection::disable() const
{
	// 低有效
	digitalWrite(enPin, HIGH);
}

void DataSelection::setPort(char port) const
{
	if(port >= (1 << sCnt))
	{
		// 输入无效序号，置为disable
		disable();
	}
	else
	{
		for (int i = 0; i < sCnt;++i)
		{
			digitalWrite(sPin + i, (port >> i) & 1);
		}
		enable();
	}
}

char DataSelection::getDigitalRead() const
{
	enable();
	return digitalRead(dPin);
}

unsigned DataSelection::getAnalogRead()
{
	enable();
	return analogRead(dPin);
}
