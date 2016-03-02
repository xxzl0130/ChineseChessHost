#include "SlipTable.h"
#include <Arduino.h>

SlipTable::SlipTable(StepperMotor x, StepperMotor y, ulong xMax, ulong yMax,
	uchr xS1,uchr xS2, uchr yS1,uchr yS2)
	:xAxis(x),yAxis(y),pos(0,0),xLength(xMax),yLength(yMax),
	xSwitch1(xS1), xSwitch2(xS2),ySwitch1(yS1), ySwitch2(yS2),
	xTotalStep(0),yTotalStep(0),xLengthPerStep(0),yLengthPerStep(0)
{
	pinMode(xSwitch1, INPUT_PULLUP);
	pinMode(xSwitch2, INPUT_PULLUP);
	pinMode(ySwitch1, INPUT_PULLUP);
	pinMode(xSwitch2, INPUT_PULLUP);
}

Point<float> SlipTable::getPos() const
{
	return pos;
}

void SlipTable::reset()
{
	xTotalStep = yTotalStep = 0;
	while (digitalRead(xSwitch2) == HIGH)
	{// 接触限位开关后信号为低 停止
		xAxis.run(FORWORD, 1);
	}
	while (digitalRead(ySwitch2) == HIGH)
	{// 接触限位开关后信号为低 停止
		yAxis.run(FORWORD, 1);
	}
	// 找到一个底后反向回去
	while (digitalRead(xSwitch1) == HIGH)
	{// 接触限位开关后信号为低 停止
		xAxis.run(BACKWORD, 1);
		++xTotalStep;
	}
	while (digitalRead(ySwitch1) == HIGH)
	{// 接触限位开关后信号为低 停止
		yAxis.run(BACKWORD, 1);
		++yTotalStep;
	}
	xLengthPerStep = static_cast<float>(xLength) / xTotalStep;
	yLengthPerStep = static_cast<float>(yLength) / yTotalStep;
	pos.x = 0.0;
	pos.y = 0.0;
}

void SlipTable::move(Point<float> pos)
{
	move(pos.x, pos.y);
}

void SlipTable::move(float x, float y)
{
	/*
	软限位
	*/
	if(x < 0.0)
	{
		x = 0.0;
	}
	else if(x > xLength)
	{
		x = xLength;
	}
	if(y < 0.0)
	{
		y = 0.0;
	}
	else if(y > yLength)
	{
		y = yLength;
	}
	// x轴要移动的步数
	ulong xAxisToGo = abs(x - pos.x) / xLengthPerStep + 0.5;
	// y轴要移动的步数
	ulong yAxisToGo = abs(y - pos.y) / xLengthPerStep + 0.5;
	// x轴方向
	Direction xDir = x > pos.x ? FORWORD : BACKWORD;
	// y轴方向
	Direction yDir = y > pos.y ? FORWORD : BACKWORD;
	// 以实际走的步数计算位置，避免累积误差
	pos.x += xAxisToGo * xLengthPerStep * (xDir == FORWORD ? 1 : -1);
	pos.y += yAxisToGo * yLengthPerStep * (yDir == FORWORD ? 1 : -1);
	while (xAxisToGo || yAxisToGo)
	{
		// 两轴交替运动
		// 速度快时近似为两轴同步运动
		if (xAxisToGo)
		{
			xAxis.run(xDir, 1);
			--xAxisToGo;
		}
		if (yAxisToGo)
		{
			yAxis.run(yDir, 1);
			--yAxisToGo;
		}
	}
}
