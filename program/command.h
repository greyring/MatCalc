#pragma once
#define _COMMAND_H_

#include "universe.h"

typedef struct
{
	int offset;//在buf1中的下标
	int length;//buf1中的长度，一般不包括空格
	int label;//在句子中的成分
	int *m, *n;//子矩阵的时候有用
	Matrix *bigMatrix;//子矩阵时有用
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