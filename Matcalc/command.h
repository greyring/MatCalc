#pragma once

#include "universe.h"

typedef struct
{
	int offset;//��buf1�е��±�
	int length;//buf1�еĳ��ȣ�һ�㲻�����ո�
	int label;//�ھ����еĳɷ�
	union{
		double d;
		long l;
		Matrix *m;
		SubMatrix *sm;
	}value;
}word;

typedef struct
{
	char ch;
	int offset;
}letter;

int com_interpret(char * const buf);