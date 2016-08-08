#pragma once

#include "universe.h"

/*
*��ʼ��,NoError
*/
void stor_ini(void);

/*
*Ϊ�������ռ�,pָ��Ҫ�����matrix�����ΪNULL�Զ��������������ͷŵ�,Error
*/
Matrix* stor_createMatrix(Matrix *p, int m, int n);

/*
*����һ���µľ���ڵ�,���½�ans,����Ѵ��������½����ڴ����,Error
*/
Matrix* stor_create(char* matrix_name, int m, int n, int label);

/*
*�������Ʒ�����Ӧ����,Error
*/
Matrix* stor_matrix(char* matrix_name);

/*
*����ĳһ����ĳһԪ��,m,n are scripts,Error
*/
double* stor_entry(Matrix *p, int m, int n);

/*
*Ϊans��������·���ռ�,Error
*/
Matrix* stor_createAns(int m, int n);

/*
*������sour��ֵ������dest,Error
*/
Matrix* stor_assign(Matrix *dest, Matrix *sour);

/*
*�������еľ���������Ϣ�����Ӿ���
*m,nΪ�Ӿ���ĳ��Ϳ�
*row �� colum�������Ӿ���Ϊ��������Щ�к���,��СΪ1��1��
*���ɵ��Ӿ��������������Ӿ�����
*�д���1���޴���0
*������Ҫȥ���ظ����еĴ���
*Error
*/
SubMatrix* stor_subMatrix(Matrix *sour, int m, int n, int *row, int *colum);

/*
*���Ӿ�����޸�ͬ�����������,NoError
*/
Matrix* stor_mergeSubMatrix(SubMatrix *subMatrix);

/*
*���źϲ�,������ŵ��½��ľ�����,Error
*/
Matrix* stor_mergeColum(Matrix *m1, Matrix *m2);

/*
*���źϲ���������ŵ��½���ans��,Error
*/
Matrix* stor_mergeRow(Matrix *m1, Matrix *m2);

/*
*�ͷ��ڴ棬NoError
*/
void stor_freeMatrix(Matrix *p);

/*
*�ͷ��ڴ棬NoError
*/
void stor_freeSubMatrix(SubMatrix *p);
