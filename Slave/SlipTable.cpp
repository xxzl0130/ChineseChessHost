#include "SlipTable.h"
#include <Arduino.h>

SlipTable::SlipTable(StepperMotor x, StepperMotor y, double xLen, double yLen, uchr xS1, uchr xS2, uchr yS1, uchr yS2, float sP)
	:pos(0,0),xLength(xLen),yLength(yLen),xSwitch1(xS1),xSwitch2(xS2),
	ySwitch1(yS1), ySwitch2(yS2),screwPitch(sP), xAxis(x),yAxis(y)
{
	pinMode(xSwitch1, INPUT_PULLUP);
	pinMode(xSwitch2, INPUT_PULLUP);
	pinMode(ySwitch1, INPUT_PULLUP);
	pinMode(xSwitch2, INPUT_PULLUP);
	LengthPerStep = screwPitch / static_cast<double>(xAxis.getStepPerCircle());
}

Point<double> SlipTable::getPos() const
{
	return pos;
}

void SlipTable::reset()
{
	while (digitalRead(xSwitch1) == HIGH)
	{// �Ӵ���λ���غ��ź�Ϊ�� ֹͣ
		xAxis.run(BACKWORD, 1);
	}
	while (digitalRead(ySwitch1) == HIGH)
	{// �Ӵ���λ���غ��ź�Ϊ�� ֹͣ
		yAxis.run(BACKWORD, 1);
	}
	pos.x = 0.0;
	pos.y = 0.0;
}

void SlipTable::move(Point<double> pos)
{
	move(pos.x, pos.y);
}

void SlipTable::move(double x, double y)
{
	/*
	����λ
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
	// x��Ҫ�ƶ��Ĳ���
	ulong xAxisToGo = abs(x - pos.x) / LengthPerStep + 0.5;
	// y��Ҫ�ƶ��Ĳ���
	ulong yAxisToGo = abs(y - pos.y) / LengthPerStep + 0.5;
	// x�᷽��
	Direction xDir = x > pos.x ? FORWORD : BACKWORD;
	// y�᷽��
	Direction yDir = y > pos.y ? FORWORD : BACKWORD;
	
	// ��ʵ���ߵĲ�������λ�ã������ۻ����
	pos.x += xAxisToGo * LengthPerStep * (xDir == FORWORD ? 1 : -1);
	pos.y += yAxisToGo * LengthPerStep * (yDir == FORWORD ? 1 : -1);

	// ���÷���
	xAxis.setDirection(xDir);
	yAxis.setDirection(yDir); 
	while (xAxisToGo && yAxisToGo)
	{
		// ���ύ���˶�
		// �ٶȿ�ʱ����Ϊ����ͬ���˶�
		xAxis.OneStep();
		--xAxisToGo;
		delayMicroseconds(50);
		yAxis.OneStep();
		--yAxisToGo;
		delayMicroseconds(50);
	}
	// ��ʣ�ಽ������
	if (xAxisToGo)
	{
		xAxis.run(xDir, xAxisToGo, 50);
	}
	if(yAxisToGo)
	{
		yAxis.run(yDir, yAxisToGo, 50);
	}
}
