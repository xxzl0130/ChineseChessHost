#ifndef __STEPPER_MOTOR_H__
#define __STEPPER_MOTOR_H__

#include <arduino.h>

#ifndef uchr
#define uchr unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

enum Direction
{
	FORWORD = 0,
	BACKWORD = 1
};

class StepperMotor
{
private:
	// �����źţ������ź�
	uchr pulsePin, dirPin;
	// ��ǰλ��
	uint Pos;
	// ϸ�ָ���
	uint StepPreCircle;
	// ����ʱ��(us)
	const static uchr pulseTime = 20;

public:
	/*
	���캯��������IO�ڵĳ�ʼ�������ݵĳ�ʼ��
	*/
	StepperMotor(uchr pPin, uchr dPin, uint step);
	// �������ת��һ��
	void OneStep() const;
	/*
	�������ת��
	����0 Dir  ת������
	����1 Step ת������
	[����2 dely] ��ʱ(us)
	*/
	void run(Direction dir, uint step, uint dely = 50);
	/*
	�������ת��
	����0 Step ת������, ������������ת
	[����2 dely] ��ʱ(us)
	*/
	void run(int step, uint dely = 50);

	// ��õ�ǰλ��
	uint getPos() const
	{
		return Pos;
	}

	// ���õ�ǰλ��
	void setPos(int pos)
	{
		Pos = pos;
	}

	//��õ�ǰ�Ƕ�
	float getAngle() const
	{
		return 360.0 / StepPreCircle * Pos;
	}

	// ����ת������
	void setDirection(Direction dir) const
	{
		digitalWrite(dirPin, dir == FORWORD ? HIGH : LOW);
	}

	uint getStepPerCircle() const
	{
		return StepPreCircle;
	}
};

#endif // __STEPPER_MOTOR_H__

#include <Arduino.h>

// �������ת��һ��
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

void StepperMotor::run(int step, uint dely)
{
	if (step > 0)
		run(FORWORD, step, dely);
	else
		run(BACKWORD, -step, dely);
}

StepperMotor motor(4, 5, 1600);


void setup()
{
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	delay(500);
}


void loop()
{
	static char flag = 0;
	/*motor.setDirection((flag ^= 1) ? FORWORD : BACKWORD);
	for (int i = 0; i < 1600;++i)
	{
		motor.OneStep();
		delayMicroseconds(50);
	}*/
	motor.run((flag ^= 1) ? FORWORD : BACKWORD, 1600, 50);
	delay(1000);
}
