#ifndef __STEPPER_MOTOR_H__
#define __STEPPER_MOTOR_H__

#include <arduino.h>

#ifndef uchr
#define uchr unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

enum Direction
{
	FORWORD = 0,
	BACKWORD = 1
};

class StepperMotor
{
private:
	// 脉冲信号，方向信号
	uchr pulsePin, dirPin;
	// 当前位置
	uint Pos;
	// 细分格数
	uint StepPreCircle;
	// 脉冲时长(us)
	const static uchr pulseTime = 20;

public:
	/*
	构造函数，进行IO口的初始化，数据的初始化
	*/
	StepperMotor(uchr pPin, uchr dPin, uint step);
	// 步进电机转动一步
	void OneStep() const;
	/*
	步进电机转动
	参数0 Dir  转动方向
	参数1 Step 转动步数
	[参数2 dely] 延时(us)
	*/
	void run(Direction dir, uint step, uint dely = 50);
	/*
	步进电机转动
	参数0 Step 转动步数, 正负代表正反转
	[参数2 dely] 延时(us)
	*/
	void run(int step, uint dely = 50);

	// 获得当前位置
	uint getPos() const
	{
		return Pos;
	}

	// 设置当前位置
	void setPos(int pos)
	{
		Pos = pos;
	}

	//获得当前角度
	float getAngle() const
	{
		return 360.0 / StepPreCircle * Pos;
	}

	// 设置转动方向
	void setDirection(Direction dir) const
	{
		digitalWrite(dirPin, dir == FORWORD ? HIGH : LOW);
	}

	uint getStepPerCircle() const
	{
		return StepPreCircle;
	}
};

#endif // __STEPPER_MOTOR_H__

#include <Arduino.h>

// 步进电机转动一步
void StepperMotor::OneStep() const
{
	digitalWrite(pulsePin, HIGH);
	delayMicroseconds(pulseTime);
	digitalWrite(pulsePin, LOW);
	delayMicroseconds(pulseTime);
}

StepperMotor::StepperMotor(uchr pPin, uchr dPin, uint step) :
	pulsePin(pPin), dirPin(dPin), Pos(0), StepPreCircle(step)
{
	pinMode(pulsePin, OUTPUT);
	digitalWrite(pulsePin, LOW);
	pinMode(dirPin, OUTPUT);
	digitalWrite(dirPin, LOW);
}

void StepperMotor::run(Direction dir, uint step, uint dely)
{
	setDirection(dir);
	if (dir == FORWORD)
		Pos = (Pos + step) % StepPreCircle;
	else
		Pos = (Pos - step + StepPreCircle) % StepPreCircle;
	while (step--)
	{
		OneStep();
		delayMicroseconds(dely);
	}
}

void StepperMotor::run(int step, uint dely)
{
	if (step > 0)
		run(FORWORD, step, dely);
	else
		run(BACKWORD, -step, dely);
}

StepperMotor motor(4, 5, 1600);


void setup()
{
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	delay(500);
}


void loop()
{
	static char flag = 0;
	/*motor.setDirection((flag ^= 1) ? FORWORD : BACKWORD);
	for (int i = 0; i < 1600;++i)
	{
		motor.OneStep();
		delayMicroseconds(50);
	}*/
	motor.run((flag ^= 1) ? FORWORD : BACKWORD, 1600, 50);
	delay(1000);
}
