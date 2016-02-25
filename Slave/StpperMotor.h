#ifndef __STPPER_MOTOR_H__
#define __STPPER_MOTOR_H__

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

class StpperMotor_4P5L
{
private:
	// 4个信号针脚
	uchr Pin0, Pin1, Pin2, Pin3;
	// 当前信号状态
	uchr state;
	// 当前位置
	int Pos;
	// 细分格数
	unsigned int StepPreCircle;
	// 步进电机转动一步
	void OneStep() const;

public:
	StpperMotor_4P5L(uchr, uchr, uchr, uchr, uint, int pos = 0);
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
	int getPos() const
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
};

class StpperMotor_2P_TB6600
{
private:
	// 脉冲信号，方向信号，使能信号
	uchr pulsePin, dirPin, enPin;
	// 当前位置
	int Pos;
	// 细分格数
	unsigned int StepPreCircle;
	// 脉冲时长(ms)
	const static uchr pulseTime = 10;

	// 步进电机转动一步
	void OneStep() const;

public:
	StpperMotor_2P_TB6600(uchr, uchr, uint, uchr en = 0xff, int pos = 0);

	// 设置转动方向
	void setDirection(Direction dir) const
	{
		digitalWrite(dirPin, dir == FORWORD ? HIGH : LOW);
	}

	// 联机
	void enable() const
	{
		if (enPin != 0xff)
			digitalWrite(enPin, HIGH);
	}

	// 脱机
	void disable() const
	{
		if (enPin != 0xff)
			digitalWrite(enPin, LOW);
	}

	// 获得当前位置
	int getPos() const
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
};

#endif // __STPPER_MOTOR_H__