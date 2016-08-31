#pragma once

#include "universe.h"

typedef struct
{
	int offset;//在buf1中的下标
	int length;//buf1中的长度，一般不包括空格
	int label;//在句子中的成分
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