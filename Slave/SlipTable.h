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
	// 两轴的步进电机
	StepperMotor xAxis, yAxis;
	// 位置信息
	ulong xPos, yPos;
	/*
	以及其他可能需要的信息
	*/
public:
	/*
	构造函数，视情况添加所需参数
	*/
	SlipTable();
	/*
	获取当前位置
	返回值为结构体Point
	*/
	Point getPos();
	/*
	将两轴归零以确定坐标系原点
	*/
	void reset();
	/*
	移动到指定坐标
	*/
	void move(Point pos);
	void move(ulong x, ulong y);
};

#endif // __SLIP_TABLE_H__