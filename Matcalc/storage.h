#pragma once

#include "universe.h"

/*
*初始化,NoError
*/
void stor_ini(void);

/*
*为矩阵分配空间,p指向要分配的matrix，如果为NULL自动创建，否则先释放掉,Error
*/
Matrix* stor_createMatrix(Matrix *p, int m, int n);

/*
*创建一个新的矩阵节点,不新建ans,如果已存在则重新进行内存分配,Error
*/
Matrix* stor_create(char* matrix_name, int m, int n, int label);

/*
*根据名称返回相应矩阵,Error
*/
Matrix* stor_matrix(char* matrix_name);

/*
*返回某一矩阵某一元素,m,n are scripts,Error
*/
double* stor_entry(Matrix *p, int m, int n);

/*
*为ans分配或重新分配空间,Error
*/
Matrix* stor_createAns(int m, int n);

/*
*将矩阵sour赋值给矩阵dest,Error
*/
Matrix* stor_assign(Matrix *dest, Matrix *sour);

/*
*根据已有的矩阵和相关信息产生子矩阵
*m,n为子矩阵的长和宽
*row 和 colum数组存放子矩阵为大矩阵的哪些行和列,最小为1行1列
*生成的子矩阵放在新申请的子矩阵中
*有错返回1，无错返回0
*调用者要去除重复行列的错误
*Error
*/
SubMatrix* stor_subMatrix(Matrix *sour, int m, int n, int *row, int *colum);

/*
*将子矩阵的修改同步到大矩阵中,NoError
*/
Matrix* stor_mergeSubMatrix(SubMatrix *subMatrix);

/*
*横着合并,结果将放到新建的矩阵中,Error
*/
Matrix* stor_mergeColum(Matrix *m1, Matrix *m2);

/*
*竖着合并，结果将放到新建的ans中,Error
*/
Matrix* stor_mergeRow(Matrix *m1, Matrix *m2);

/*
*释放内存，NoError
*/
void stor_freeMatrix(Matrix *p);

/*
*释放内存，NoError
*/
void stor_freeSubMatrix(SubMatrix *p);
