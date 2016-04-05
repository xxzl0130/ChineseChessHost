#ifndef __DATA_SELECTION_H__
#define __DATA_SELECTION_H__

#include <Arduino.h>

class DataSelection
{
private:
	// ʹ�����
	char enPin;
	// ѡ�����
	char sPin;
	// �������
	char dPin;
	// ѡ���������
	char sCnt;
public:
	DataSelection(char en, char s, char cnt, char d, char mode = INPUT);
	// ʹ��
	void enable() const;
	void disable() const;
	// ����ѡ�����
	void setPort(char port) const;
	// ��ȡ����
	char getDigitalRead() const;
	unsigned int getAnalogRead();
};

#endif //__DATA_SELECTION_H__