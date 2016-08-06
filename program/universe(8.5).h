#pragma once
#define _UNIVERSE_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <time.h>

typedef struct
{
	int m,n;
	long double **pd;
	long **pl;
}Matrix;

typedef struct matrix_node
{
	Matrix *matrix;
	char* name;
	struct matrix_node *left, *right;
}Matrix_Node;

extern Matrix *ans[1];
extern char buf[256];
extern FILE* fpin;
extern FILE* fpout;
extern int uni_flag;
extern char filein[256],fileout[256];