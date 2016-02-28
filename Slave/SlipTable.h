#ifndef __SLIP_TABLE_H__
#define __SLIP_TABLE_H__

#include <Arduino.h>
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

struct Point
{
	ulong x, y;
};

class SlipTable
{
private:
	// ����Ĳ������
	StepperMotor xAxis, yAxis;
	// λ����Ϣ
	ulong xPos, yPos;
	/*
	�Լ�����������Ҫ����Ϣ
	*/
public:
	/*
	���캯�������������������
	*/
	SlipTable();
	/*
	��ȡ��ǰλ��
	����ֵΪ�ṹ��Point
	*/
	Point getPos();
	/*
	�����������ȷ������ϵԭ��
	*/
	void reset();
	/*
	�ƶ���ָ������
	*/
	void move(Point pos);
	void move(ulong x, ulong y);
};

#endif // __SLIP_TABLE_H__