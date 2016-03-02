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
	BACKWORD = 0,
	FORWORD = 1
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
	const static uchr pulseTime = 10;

	// 步进电机转动一步
	void OneStep() const;

public:
	/*
	构造函数，进行IO口的初始化，数据的初始化
	*/
	StepperMotor(uchr pPin, uchr dPin, uint step);
	/*
	步进电机转动
	参数0 Dir  转动方向
	参数1 Step 转动步数
	[参数2 freq] 转动频率
	*/
	void run(Direction dir, uint step, uint freq = 1000);
	/*
	步进电机转动
	参数0 Step 转动步数, 正负代表正反转
	[参数2 freq] 转动频率
	*/
	void run(int step, uint freq = 1000);

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
};

#endif // __STEPPER_MOTOR_H__