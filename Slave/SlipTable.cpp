#include "SlipTable.h"
#include <Arduino.h>

SlipTable::SlipTable(StepperMotor x, StepperMotor y, double xLen, double yLen, uchr xS,uchr yS, float sP)
	:pos(0,0),xLength(xLen),yLength(yLen),xSwitch(xS),ySwitch(yS),screwPitch(sP), xAxis(x),yAxis(y)
{
	pinMode(xSwitch, INPUT_PULLUP);
	pinMode(ySwitch, INPUT_PULLUP);
	LengthPerStep = screwPitch / static_cast<double>(xAxis.getStepPerCircle());
}

Point<double> SlipTable::getPos() const
{
	return pos;
}

void SlipTable::reset()
{
	while (digitalRead(ySwitch) == HIGH)
	{// 接触限位开关后信号为low 停止
		yAxis.run(FORWORD, 1);
	}
	while (digitalRead(xSwitch) == HIGH)
	{// 接触限位开关后信号为low 停止
		xAxis.run(FORWORD, 1);
	}
	pos.x = xLength;
	pos.y = yLength;
	move(0, 0);
}

void SlipTable::move(Point<double> pos)
{
	move(pos.x, pos.y);
}

void SlipTable::move(double x, double y)
{
	/*
	软限位
	*/
	if (x < 0.0)
	{
		x = 0.0;
	}
	else if (x > xLength)
	{
		x = xLength;
	}
	if (y < 0.0)
	{
		y = 0.0;
	}
	else if (y > yLength)
	{
		y = yLength;
	}
	// x轴要移动的步数
	ulong xAxisToGo = abs(x - pos.x) / LengthPerStep + 0.5;
	// y轴要移动的步数
	ulong yAxisToGo = abs(y - pos.y) / LengthPerStep + 0.5;
	// x轴方向
	Direction xDir = x > pos.x ? FORWORD : BACKWORD;
	// y轴方向
	Direction yDir = y > pos.y ? FORWORD : BACKWORD;

	// 以实际走的步数计算位置，避免累积误差
	pos.x += xAxisToGo * LengthPerStep * (xDir == FORWORD ? 1 : -1);
	pos.y += yAxisToGo * LengthPerStep * (yDir == FORWORD ? 1 : -1);
	

	// 设置方向
	xAxis.setDirection(xDir);
	yAxis.setDirection(yDir); 
	while (xAxisToGo && yAxisToGo)
	{
		// 两轴交替运动
		// 速度快时近似为两轴同步运动
		xAxis.OneStep();
		--xAxisToGo;
		delayMicroseconds(50);
		yAxis.OneStep();
		--yAxisToGo;
		delayMicroseconds(50);
	}
	// 将剩余步数走完
	if (xAxisToGo)
	{
		xAxis.run(xDir, xAxisToGo, 50);
	}
	if(yAxisToGo)
	{
		yAxis.run(yDir, yAxisToGo, 50);
	}
}
