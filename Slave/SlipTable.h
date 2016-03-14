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
	// 位置信息
	Point<double> pos;
	// 尺寸 单位mm
	double xLength, yLength;
	// 限位开关
	uchr xSwitch1, xSwitch2, ySwitch1, ySwitch2;
	// 螺距
	double screwPitch;
	// 每步长度
	double LengthPerStep;
public:
	// 两轴的步进电机
	StepperMotor xAxis, yAxis;
	/*
	构造函数，进行IO口的初始化，数据的初始化
	*/
	SlipTable(StepperMotor x, StepperMotor y, double xLen, double yLen, uchr xS1, uchr xS2, uchr yS1, uchr yS2,float sP);
	/*
	获取当前位置
	返回值为结构体Point
	*/
	Point<double> getPos() const;
	/*
	将两轴归零以确定坐标系原点
	*/
	void reset();
	/*
	移动到指定坐标
	*/
	void move(Point<double> pos);
	void move(double x, double y);
};

#endif // __SLIP_TABLE_H__