#pragma once

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
	double **pd;
}Matrix;

typedef struct
{
	int *row, *colum;
	Matrix *sour, *sub;
}SubMatrix;

typedef struct matrix_node
{
	Matrix *matrix;
	char* name;
	struct matrix_node *left, *right;
}Matrix_Node;

struct
{
	unsigned in : 1;//1:is an input file
	unsigned out : 1;//1:is an output file
	unsigned show : 1;//1:show the result

}uniFlag;

extern Matrix *ans;
extern FILE* fpin;
extern FILE* fpout;
extern char filein[256],fileout[256];
extern char buf[512];
extern char iobuf[512];
