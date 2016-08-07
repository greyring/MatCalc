#pragma once
#define _COMMAND_H_

#include "universe.h"

typedef struct
{
	int offset;//��buf1�е��±�
	int length;//buf1�еĳ��ȣ�һ�㲻�����ո�
	int label;//�ھ����еĳɷ�
	int *m, *n;//�Ӿ����ʱ������
	Matrix *bigMatrix;//�Ӿ���ʱ����
	union{
		long valuel;
		long double valued;
		Matrix *matrix;
	}value;
}word;

typedef struct
{
	char ch;
	int offset;
}letter;

int com_interpret(void);