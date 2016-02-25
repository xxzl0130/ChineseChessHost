#include "MatrixKeyboard.h"

MatrixKeyboard::MatrixKeyboard(unsigned char rs, unsigned char rt, unsigned char cs, unsigned char ct) :
	rowS(rs), rowT(rt), colS(cs), colT(ct)
{
	for (unsigned char i = rowS;i <= rowT;++i)
	{
		// Use row as OUTPUT.
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}
	for (unsigned char i = colS;i <= colT;++i)
	{
		// Use column as OUTPUT.
		pinMode(i, INPUT);
	}
}

void MatrixKeyboard::getKeyPresing(unsigned char* buf, unsigned char* count) const
{
	*count = 0;
	for (unsigned char i = rowS;i <= rowT;++i)
	{
		// Pull up a whole row.
		digitalWrite(i, HIGH);
		// Test each column to confirm if the key is bing pressed
		for (unsigned char j = colS;j <= colT;++j)
		{
			if (isHigh(j))
			{
				buf[*count++] = ((i - rowS) << 4) | (j - colS);
			}
		}
		digitalWrite(i, LOW);
	}
}

bool MatrixKeyboard::isPressing(unsigned char row, unsigned char col) const
{
	bool ret = false;
	if (row + rowS > rowT)
		return false;
	if (col + colS > colT)
		return false;
	digitalWrite(row + rowS, HIGH);
	if (isHigh(col + colS))
		ret = true;
	return ret;
}

bool MatrixKeyboard::isPressing(unsigned char key) const
{
	return isPressing(key & 0x0f, (key >> 4) & 0x0f);
}

bool isHigh(unsigned char pin)
{
	if(digitalRead(pin) == HIGH)
	{
		// Delay 10ms to eliminate jitter.
		delay(10);
		if (digitalRead(pin) == HIGH)
			return true;
	}
	return false;
}