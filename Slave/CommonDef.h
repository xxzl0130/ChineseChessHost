#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

// ���Ա�־
#define DEBUG

#ifndef uchr
#define uchr unsigned char
#endif
#ifndef uint
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif
#ifndef ull
#define ull unsigned long long
#endif

// ������ӻ����͵Ĳ����ַ���
#define testComHost "Hello slave"
// �ӻ��������ظ��Ĳ����ַ���
#define testComSlave "Hello host"
// ���ڱ�����
#define generalBaudRate 9600UL

// ��������
#define BoardRow 10
// ��������
#define BoardCol 9

enum DIFFICULTY
{
	easy = 3,
	normal = 5,
	hard = 10,
	master = 32
};

#endif // __COMMON_DEF_H__