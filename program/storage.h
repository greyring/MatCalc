#pragma once
#define _STORAGE_H_

#include "universe.h"


void stor_ini(void);

/*
 *返回矩阵的一些特征
 */
int stor_type(Matrix *p);

/*
 *为矩阵分配空间,p指向的matrix为NULL
 */
Matrix* stor_createMatrix(Matrix *p, int m, int n, int type);

/*
 *创建一个新的矩阵节点,不新建ans,如果已存在则重新进行内存分配
 */
Matrix* stor_create(char* matrix_name, int m, int n, int label);

/*
 *根据名称返回相应矩阵
 */
Matrix* stor_matrix(char* matrix_name);

/*
 *返回某一矩阵某一元素的指针,由调用方通过type判断为int还是long double
 */
void* stor_entry(Matrix *p, int m, int n);

/*
 *为ans和ansex分配或重新分配空间
 */
Matrix* stor_createAns(int label, int m, int n, int label2);

/*
 *释放申请的内存
 *不能释放ans0
 *只释放matrix，不释放节点，如果释放的是最后一个节点，会从后往前把释放了matrix的节点释放掉
 *没测试过
 */
int stor_freeAns(int label);

/*
 *返回第label个matrix
 */
Matrix* stor_ans(int label);

/*
 *返回当前有几个矩阵创建了
 */
int stor_getAnsn(void);

/*
 *返回ans或ansex矩阵某一元素的指针,由调用方通过type判断为int还是long double
 */
void* stor_ansEntry(int label, int m, int n);

/*
 *将矩阵sour赋值给矩阵dest
 *处理不同type矩阵的情况
 */
Matrix* stor_assign(Matrix *dest, Matrix *sour);

/*
 *根据已有的矩阵和相关信息产生子矩阵
 *m,n为子矩阵的长和宽
 *row 和 colum数组存放子矩阵为大矩阵的哪些行和列,最小为1行1列
 *生成的子矩阵放在ansex中
 *有错返回1，无错返回0
 *调用者要去除重复行列的错误,调用者要对行列排序
 */
Matrix* stor_subMatrix(Matrix *sour, int m, int n, int *row, int *colum);

/*
 *将子矩阵的修改同步到大矩阵中,不做一些出错判断
 */
Matrix* stor_assignSubMatrix(Matrix *dest, Matrix *sour, int m, int n, int *row, int *colum);

/*
 *横着合并,结果将放到新建的矩阵中
 */
Matrix* stor_mergeColum(Matrix *m1, Matrix *m2);

/*
 *竖着合并，结果将放到新建的矩阵中
 */
Matrix* stor_mergeRow(Matrix *m1, Matrix *m2);
