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
	// �ź����
	uchr Pin0, Pin1, Pin2, Pin3;
	// ��ǰ�ź�״̬
	uchr state;
	// ��ǰλ��
	int Pos;
	// ϸ�ָ���
	uint StepPreCircle;
	// �������ת��һ��
	void OneStep() const;

public:
	/*
	���캯��������IO�ڵĳ�ʼ�������ݵĳ�ʼ��
	*/
	StepperMotor(uchr, uchr, uchr, uchr, uint, int pos = 0);
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
	int getPos() const
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
};

#endif // __STEPPER_MOTOR_H__