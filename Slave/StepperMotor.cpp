#include "StepperMotor.h"

// 步进电机转动一步
void StepperMotor::OneStep() const
{
}

StepperMotor::StepperMotor(uchr in0, uchr in1, uchr in2, uchr in3, uint step, int pos) :
	Pin0(in0), Pin1(in1), Pin2(in2), Pin3(in3), state(0), Pos(pos), StepPreCircle(step)
{
}

void StepperMotor::run(Direction dir, uint step, uint freq)
{
}

void StepperMotor::run(int step, uint freq)
{
}