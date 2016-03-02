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
	// 两轴的步进电机
	StepperMotor xAxis, yAxis;
	// 位置信息
	Point<float> pos;
	// 尺寸 单位mm
	uint xLength, yLength;
	// 限位开关
	uchr xSwitch1, xSwitch2, ySwitch1, ySwitch2;
	// 总步数
	ulong xTotalStep, yTotalStep;
	// 每步长度
	float xLengthPerStep, yLengthPerStep;
public:
	/*
	构造函数，进行IO口的初始化，数据的初始化
	*/
	SlipTable(StepperMotor x, StepperMotor y, ulong xMax, ulong yMax, uchr xS1, uchr xS2, uchr yS1, uchr yS2);
	/*
	获取当前位置
	返回值为结构体Point
	*/
	Point<float> getPos() const;
	/*
	将两轴归零以确定坐标系原点
	*/
	void reset();
	/*
	移动到指定坐标
	*/
	void move(Point<float> pos);
	void move(float x, float y);
};

#endif // __SLIP_TABLE_H__