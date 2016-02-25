#ifndef __STPPER_MOTOR_H__
#define __STPPER_MOTOR_H__

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

class StpperMotor_4P5L
{
private:
	// 4���ź����
	uchr Pin0, Pin1, Pin2, Pin3;
	// ��ǰ�ź�״̬
	uchr state;
	// ��ǰλ��
	int Pos;
	// ϸ�ָ���
	unsigned int StepPreCircle;
	// �������ת��һ��
	void OneStep() const;

public:
	StpperMotor_4P5L(uchr, uchr, uchr, uchr, uint, int pos = 0);
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

class StpperMotor_2P_TB6600
{
private:
	// �����źţ������źţ�ʹ���ź�
	uchr pulsePin, dirPin, enPin;
	// ��ǰλ��
	int Pos;
	// ϸ�ָ���
	unsigned int StepPreCircle;
	// ����ʱ��(ms)
	const static uchr pulseTime = 10;

	// �������ת��һ��
	void OneStep() const;

public:
	StpperMotor_2P_TB6600(uchr, uchr, uint, uchr en = 0xff, int pos = 0);

	// ����ת������
	void setDirection(Direction dir) const
	{
		digitalWrite(dirPin, dir == FORWORD ? HIGH : LOW);
	}

	// ����
	void enable() const
	{
		if (enPin != 0xff)
			digitalWrite(enPin, HIGH);
	}

	// �ѻ�
	void disable() const
	{
		if (enPin != 0xff)
			digitalWrite(enPin, LOW);
	}

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
};

#endif // __STPPER_MOTOR_H__