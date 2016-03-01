#include "SlipTable.h"

SlipTable::SlipTable():xAxis(StepperMotor(1,2,3,4,1024)),yAxis(StepperMotor(1, 2, 3, 4, 1024))
{
}

Point SlipTable::getPos()
{
}

void SlipTable::reset()
{
}

void SlipTable::move(Point pos)
{
}

void SlipTable::move(unsigned long x, unsigned long y)
{
}
