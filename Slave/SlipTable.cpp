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
	{// �Ӵ���λ���غ��ź�Ϊlow ֹͣ
		yAxis.run(FORWORD, 1);
	}
	while (digitalRead(xSwitch) == HIGH)
	{// �Ӵ���λ���غ��ź�Ϊlow ֹͣ
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
