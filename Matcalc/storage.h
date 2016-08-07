#pragma once
#define _STORAGE_H_

#include "universe.h"


void stor_ini(void);

/*
 *���ؾ����һЩ����
 */
int stor_type(Matrix *p);

/*
 *Ϊ�������ռ�,pָ���matrixΪNULL
 */
Matrix* stor_createMatrix(Matrix *p, int m, int n, int type);

/*
 *����һ���µľ���ڵ�,���½�ans,����Ѵ��������½����ڴ����
 */
Matrix* stor_create(char* matrix_name, int m, int n, int label);

/*
 *�������Ʒ�����Ӧ����
 */
Matrix* stor_matrix(char* matrix_name);

/*
 *����ĳһ����ĳһԪ�ص�ָ��,�ɵ��÷�ͨ��type�ж�Ϊint����long double
 */
void* stor_entry(Matrix *p, int m, int n);

/*
 *Ϊans��ansex��������·���ռ�
 */
Matrix* stor_createAns(int label, int m, int n, int label2);

/*
 *�ͷ�������ڴ�
 *�����ͷ�ans0
 *ֻ�ͷ�matrix�����ͷŽڵ㣬����ͷŵ������һ���ڵ㣬��Ӻ���ǰ���ͷ���matrix�Ľڵ��ͷŵ�
 *û���Թ�
 */
int stor_freeAns(int label);

/*
 *���ص�label��matrix
 */
Matrix* stor_ans(int label);

/*
 *���ص�ǰ�м������󴴽���
 */
int stor_getAnsn(void);

/*
 *����ans��ansex����ĳһԪ�ص�ָ��,�ɵ��÷�ͨ��type�ж�Ϊint����long double
 */
void* stor_ansEntry(int label, int m, int n);

/*
 *������sour��ֵ������dest
 *����ͬtype��������
 */
Matrix* stor_assign(Matrix *dest, Matrix *sour);

/*
 *�������еľ���������Ϣ�����Ӿ���
 *m,nΪ�Ӿ���ĳ��Ϳ�
 *row �� colum�������Ӿ���Ϊ��������Щ�к���,��СΪ1��1��
 *���ɵ��Ӿ������ansex��
 *�д���1���޴���0
 *������Ҫȥ���ظ����еĴ���,������Ҫ����������
 */
Matrix* stor_subMatrix(Matrix *sour, int m, int n, int *row, int *colum);

/*
 *���Ӿ�����޸�ͬ�����������,����һЩ�����ж�
 */
Matrix* stor_assignSubMatrix(Matrix *dest, Matrix *sour, int m, int n, int *row, int *colum);

/*
 *���źϲ�,������ŵ��½��ľ�����
 */
Matrix* stor_mergeColum(Matrix *m1, Matrix *m2);

/*
 *���źϲ���������ŵ��½��ľ�����
 */
Matrix* stor_mergeRow(Matrix *m1, Matrix *m2);
