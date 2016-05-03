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
	// λ����Ϣ
	Point<double> pos;
	// �ߴ� ��λmm
	double xLength, yLength;
	// ��λ����
	uchr xSwitch, ySwitch;
	// �ݾ�
	double screwPitch;
	// ÿ������
	double LengthPerStep;
public:
	// ����Ĳ������
	StepperMotor xAxis, yAxis;
	/*
	���캯��������IO�ڵĳ�ʼ�������ݵĳ�ʼ��
	*/
	SlipTable(StepperMotor x, StepperMotor y, double xLen, double yLen, uchr xS, uchr yS,float sP);
	/*
	��ȡ��ǰλ��
	����ֵΪ�ṹ��Point
	*/
	Point<double> getPos() const;
	/*
	�����������ȷ������ϵԭ��
	*/
	void reset();
	/*
	�ƶ���ָ������
	*/
	void move(Point<double> pos);
	void move(double x, double y);
};

#endif // __SLIP_TABLE_H__