#include "StepperMotor.h"
#include <Arduino.h>

// 步进电机转动一步
void StepperMotor::OneStep() const
{
	digitalWrite(pulsePin, HIGH);
	delayMicroseconds(pulseTime);
	digitalWrite(pulsePin, LOW);
	delayMicroseconds(pulseTime);
}

StepperMotor::StepperMotor(uchr pPin, uchr dPin, uint step) :
	pulsePin(pPin), dirPin(dPin), Pos(0), StepPreCircle(step)
{
	pinMode(pulsePin, OUTPUT);
	digitalWrite(pulsePin, LOW);
	pinMode(dirPin, OUTPUT);
	digitalWrite(dirPin, LOW);
}

void StepperMotor::run(Direction dir, uint step, uint dely)
{
	setDirection(dir);
	if (dir == FORWORD)
		Pos = (Pos + step) % StepPreCircle;
	else
		Pos = (Pos - step + StepPreCircle) % StepPreCircle;
	while (step--)
	{
		OneStep();
		delayMicroseconds(dely);
	}
}

void StepperMotor::run(int step, uint freq)
{
	if (step > 0)
		run(FORWORD, step, freq);
	else
		run(BACKWORD, -step, freq);
}