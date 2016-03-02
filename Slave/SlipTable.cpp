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
	{// �Ӵ���λ���غ��ź�Ϊ�� ֹͣ
		xAxis.run(FORWORD, 1);
	}
	while (digitalRead(ySwitch2) == HIGH)
	{// �Ӵ���λ���غ��ź�Ϊ�� ֹͣ
		yAxis.run(FORWORD, 1);
	}
	// �ҵ�һ���׺����ȥ
	while (digitalRead(xSwitch1) == HIGH)
	{// �Ӵ���λ���غ��ź�Ϊ�� ֹͣ
		xAxis.run(BACKWORD, 1);
		++xTotalStep;
	}
	while (digitalRead(ySwitch1) == HIGH)
	{// �Ӵ���λ���غ��ź�Ϊ�� ֹͣ
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
	����λ
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
	// x��Ҫ�ƶ��Ĳ���
	ulong xAxisToGo = abs(x - pos.x) / xLengthPerStep + 0.5;
	// y��Ҫ�ƶ��Ĳ���
	ulong yAxisToGo = abs(y - pos.y) / xLengthPerStep + 0.5;
	// x�᷽��
	Direction xDir = x > pos.x ? FORWORD : BACKWORD;
	// y�᷽��
	Direction yDir = y > pos.y ? FORWORD : BACKWORD;
	// ��ʵ���ߵĲ�������λ�ã������ۻ����
	pos.x += xAxisToGo * xLengthPerStep * (xDir == FORWORD ? 1 : -1);
	pos.y += yAxisToGo * yLengthPerStep * (yDir == FORWORD ? 1 : -1);
	while (xAxisToGo || yAxisToGo)
	{
		// ���ύ���˶�
		// �ٶȿ�ʱ����Ϊ����ͬ���˶�
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
