#ifndef __SLIP_TABLE_H__
#define __SLIP_TABLE_H__

#include "StepperMotor.h"
#include "CommonDef.h"

#ifndef uchr
#define uchr unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef ulong
#define uint unsigned long
#endif

template <typename T>
class Point
{
public:
	T x, y;

	Point(): x(0), y(0)
	{
	}

	Point(T _x, T _y): x(_x), y(_y)
	{
	}
};

class SlipTable
{
private:
	// ����Ĳ������
	StepperMotor xAxis, yAxis;
	// λ����Ϣ
	Point<float> pos;
	// �ߴ� ��λmm
	uint xLength, yLength;
	// ��λ����
	uchr xSwitch1, xSwitch2, ySwitch1, ySwitch2;
	// �ܲ���
	ulong xTotalStep, yTotalStep;
	// ÿ������
	float xLengthPerStep, yLengthPerStep;
public:
	/*
	���캯��������IO�ڵĳ�ʼ�������ݵĳ�ʼ��
	*/
	SlipTable(StepperMotor x, StepperMotor y, ulong xMax, ulong yMax, uchr xS1, uchr xS2, uchr yS1, uchr yS2);
	/*
	��ȡ��ǰλ��
	����ֵΪ�ṹ��Point
	*/
	Point<float> getPos() const;
	/*
	�����������ȷ������ϵԭ��
	*/
	void reset();
	/*
	�ƶ���ָ������
	*/
	void move(Point<float> pos);
	void move(float x, float y);
};

#endif // __SLIP_TABLE_H__