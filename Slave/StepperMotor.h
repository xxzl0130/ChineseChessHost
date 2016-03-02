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
	BACKWORD = 0,
	FORWORD = 1
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
	const static uchr pulseTime = 10;

	// �������ת��һ��
	void OneStep() const;

public:
	/*
	���캯��������IO�ڵĳ�ʼ�������ݵĳ�ʼ��
	*/
	StepperMotor(uchr pPin, uchr dPin, uint step);
	/*
	�������ת��
	����0 Dir  ת������
	����1 Step ת������
	[����2 freq] ת��Ƶ��
	*/
	void run(Direction dir, uint step, uint freq = 1000);
	/*
	�������ת��
	����0 Step ת������, ������������ת
	[����2 freq] ת��Ƶ��
	*/
	void run(int step, uint freq = 1000);

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
};

#endif // __STEPPER_MOTOR_H__