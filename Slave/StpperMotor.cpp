#include "StpperMotor.h"

// 步进电机转动一步
void StpperMotor_4P5L::OneStep() const
{
	switch (state)
	{
	case 0:
		digitalWrite(Pin0, LOW);
		digitalWrite(Pin1, LOW);
		digitalWrite(Pin2, LOW);
		digitalWrite(Pin3, HIGH);
		break;
	case 1:
		digitalWrite(Pin0, LOW);
		digitalWrite(Pin1, LOW);
		digitalWrite(Pin2, HIGH);
		digitalWrite(Pin3, HIGH);
		break;
	case 2:
		digitalWrite(Pin0, LOW);
		digitalWrite(Pin1, LOW);
		digitalWrite(Pin2, HIGH);
		digitalWrite(Pin3, LOW);
		break;
	case 3:
		digitalWrite(Pin0, LOW);
		digitalWrite(Pin1, HIGH);
		digitalWrite(Pin2, HIGH);
		digitalWrite(Pin3, LOW);
		break;
	case 4:
		digitalWrite(Pin0, LOW);
		digitalWrite(Pin1, HIGH);
		digitalWrite(Pin2, LOW);
		digitalWrite(Pin3, LOW);
		break;
	case 5:
		digitalWrite(Pin0, HIGH);
		digitalWrite(Pin1, HIGH);
		digitalWrite(Pin2, LOW);
		digitalWrite(Pin3, LOW);
		break;
	case 6:
		digitalWrite(Pin0, HIGH);
		digitalWrite(Pin1, LOW);
		digitalWrite(Pin2, LOW);
		digitalWrite(Pin3, LOW);
		break;
	case 7:
		digitalWrite(Pin0, HIGH);
		digitalWrite(Pin1, LOW);
		digitalWrite(Pin2, LOW);
		digitalWrite(Pin3, HIGH);
		break;
	default:
		digitalWrite(Pin0, LOW);
		digitalWrite(Pin1, LOW);
		digitalWrite(Pin2, LOW);
		digitalWrite(Pin3, LOW);
		break;
	}
}

StpperMotor_4P5L::StpperMotor_4P5L(uchr in0, uchr in1, uchr in2, uchr in3, uint step, int pos) :
	Pin0(in0), Pin1(in1), Pin2(in2), Pin3(in3), state(0), Pos(pos), StepPreCircle(step)
{
	pinMode(Pin0, OUTPUT);
	pinMode(Pin1, OUTPUT);
	pinMode(Pin2, OUTPUT);
	pinMode(Pin3, OUTPUT);
	digitalWrite(Pin0, LOW);
	digitalWrite(Pin1, LOW);
	digitalWrite(Pin2, LOW);
	digitalWrite(Pin3, LOW);
}

void StpperMotor_4P5L::run(Direction dir, uint step, uint freq)
{
	if (dir == FORWORD)
		Pos = (Pos + step) % StepPreCircle;
	else
		Pos = (Pos - step + StepPreCircle) % StepPreCircle;
	while (step--)
	{
		OneStep();
		if (dir)
		{
			++state;
		}
		else
		{
			--state;
		}
		state = (state + 7) & 7;
		delayMicroseconds(1000000L / freq);
	}
}

void StpperMotor_4P5L::run(int step, uint freq)
{
	if (step > 0)
		run(FORWORD, step, freq);
	else
		run(BACKWORD, -step, freq);
}

StpperMotor_2P_TB6600::StpperMotor_2P_TB6600(uchr pulse, uchr dir, uint step, uchr en, int pos) :
	pulsePin(pulse), dirPin(dir), enPin(en), Pos(pos), StepPreCircle(step)
{
	pinMode(pulsePin, OUTPUT);
	pinMode(dirPin, OUTPUT);
	if (enPin != 0xff)
	{
		pinMode(enPin, OUTPUT);
		digitalWrite(enPin, HIGH);
	}
	digitalWrite(pulsePin, LOW);
	digitalWrite(dirPin, LOW);
}

void StpperMotor_2P_TB6600::run(Direction dir, uint step, uint freq)
{
	setDirection(dir);
	if (dir == FORWORD)
		Pos = (Pos + step) % StepPreCircle;
	else
		Pos = (Pos - step + StepPreCircle) % StepPreCircle;
	while (step--)
	{
		OneStep();
		delayMicroseconds(max(10000000L / freq - pulseTime, 10));
	}
}

void StpperMotor_2P_TB6600::run(int step, uint freq)
{
	if (step > 0)
		run(FORWORD, step, freq);
	else
		run(BACKWORD, -step, freq);
}

void StpperMotor_2P_TB6600::OneStep() const
{
	digitalWrite(pulsePin, HIGH);
	delayMicroseconds(pulseTime);
	digitalWrite(pulsePin, LOW);
}
