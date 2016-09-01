#include "universe.h"
#include "storage.h"
#include "util.h"

/*
 *ans不能通过名字来create
 */

/*
 *创建的矩阵变量都通过二分查找
 */
static Matrix_Node *matrixHeadNode = NULL;
Matrix *ans = NULL;
static void freeMatrixNode(Matrix_Node *p);

/*
*初始化
*/
void stor_ini(void)
{
	matrixHeadNode = (Matrix_Node *)malloc(sizeof(Matrix_Node));
	matrixHeadNode->matrix = NULL;
	matrixHeadNode->left = NULL;
	matrixHeadNode->right = NULL;

	ans = (Matrix *)malloc(sizeof(Matrix));
	ans->m = 0;
	ans->n = 0;
	ans->pd = NULL;
}

/*
 *为矩阵分配空间,p is &m(m points to Matrix)，只负责创建, 默认全零，Error
 */
Matrix* stor_createMatrix(Matrix **p, int m, int n)
{
	int i;

	if (m == 0 || n == 0)
	{
		//Error
		return NULL;
	}
	*p = (Matrix*)malloc(sizeof(Matrix));
	(*p)->pd = NULL;

	(*p)->m = m;
	(*p)->n = n;
	(*p)->pd = (double**)malloc(m * sizeof(double*));
	if ((*p)->pd == NULL)
	{
		//Error
		return NULL;
	}
	for (i = 0; i<m; i++)
	{
		(*p)->pd[i] = (double *)calloc(n, sizeof(double));
		if ((*p)->pd[i] == NULL)
		{
			//Error
			return NULL;
		}
	}
	return *p;
}

/*
 *根据名称返回相应矩阵节点,Error
 */
static Matrix_Node* getMatrix_Node(char* matrix_name)
{
	Matrix_Node *p = matrixHeadNode;
	int flag;

	if (matrix_name == NULL)
	{
		return NULL;
	}
	p = p->left;
	while (p!= NULL && (flag = strcmp(matrix_name, p->name)))
	{
		if (flag > 0)
		{
			p = p->right;
		}
		else
		{
			p = p->left;
		}
	}
	return p;//if there doesn't exist such a matrix ,then return NULL
}

/*
 *创建一个新的矩阵节点,不新建ans,如果已存在则重新进行内存分配,Error
 */
Matrix* stor_create(char* matrix_name, int m, int n)
{
	Matrix_Node *p = NULL;
	int flag;

	if (matrix_name == NULL)
	{
		return NULL;
	}
	if (strlen(matrix_name) == 0)
	{
		return NULL;
	}
	if(!strcmp(matrix_name, "ans"))
	{
		//Error
		printf("ans can't be created\n");
		return NULL;
	}
	p = matrixHeadNode;
	if (p->left == NULL)//There is no node in the tree
	{
		p->left = (Matrix_Node *)malloc(sizeof(Matrix_Node));
		p = p->left;
		p->name = (char *)malloc(strlen(matrix_name)+1);
		if (p->name == NULL)
		{
			//Error
			return NULL;
		}
		util_strcpy(p->name, matrix_name);
		p->left = NULL;
		p->right = NULL;
		p->matrix = NULL;

		return (stor_createMatrix(&(p->matrix), m, n));
	}
	else
	{
		p = p->left;
		while (flag = strcmp(matrix_name, p->name))
		{
			if (flag > 0)
			{
				if (p->right == NULL)
				{
					p->right = (Matrix_Node *)malloc(sizeof(Matrix_Node));
					p = p->right;
					p->name = (char *)malloc(strlen(matrix_name)+1);
					if (p->name == NULL)
					{
						//Error
						return NULL;
					}
					util_strcpy(p->name, matrix_name);
					p->left = NULL;
					p->right = NULL;
					p->matrix = NULL;

					return (stor_createMatrix(&(p->matrix), m, n));
				}
				else
				{
					p = p->right;
				}
			}
			else
			{
				if (p->left == NULL)
				{
					p->left = (Matrix_Node *)malloc(sizeof(Matrix_Node));
					p = p->left;
					p->name = (char *)malloc(strlen(matrix_name)+1);
					if (p->name == NULL)
					{
						//Error
						return NULL;
					}
					util_strcpy(p->name, matrix_name);
					p->left = NULL;
					p->right = NULL;
					p->matrix = NULL;
					return (stor_createMatrix(&(p->matrix), m, n));
				}
				else
				{
					p = p->left;
				}
			}
		}
	}
	//has been created before
	stor_freeMatrix(p->matrix);
	return (stor_createMatrix(&(p->matrix), m, n));
}

/*
 *根据名称返回相应矩阵,Error
 */
Matrix* stor_matrix(char* matrix_name)
{
	Matrix_Node *p = NULL;
	if ((p = getMatrix_Node(matrix_name))==NULL)
	{
		//Error
		return NULL;
	}
	return p->matrix;
}

/*
 *为ans分配或重新分配空间,if Ans not NULL will be cleared and malloc,Error
 */
Matrix* stor_createAns(int m, int n)
{
	if (ans != NULL)
		stor_freeMatrix(ans);
	return (stor_createMatrix(&ans, m, n));
}

/*
 *返回某一矩阵某一元素,m,n are scripts,Error
 */
double* stor_entry(Matrix *p, int m, int n)
{
	if (p == NULL)
	{
		return NULL;
	}
	if(m<0 || m>=p->m
		||n<0 || n>=p->n)
	{
		//Error越界
		return 0;
	}
	return p->pd[m]+n;
}

/*
 *将矩阵sour赋值给矩阵dest,Error
 */
Matrix* stor_assign(Matrix *dest, Matrix *sour)
{
	int i;
	int m = dest->m, n = dest->n;

	if (dest == NULL || sour == NULL)
	{
		return NULL;
	}
	if ((m != sour->m)||(n != sour->n))
	{
		//Errpr 矩阵不匹配
		printf("Error");
		return NULL;
	}
	for (i = 0; i < m; i++)
	{
		memcpy(dest->pd[i], sour->pd[i], n * sizeof(double));
	}
	return dest;
}

/*
 *根据已有的矩阵和相关信息产生子矩阵
 *m,n为子矩阵的长和宽
 *row 和 colum数组存放子矩阵为大矩阵的哪些行和列,最小为1行1列,自动排序，重复报错
 *生成的子矩阵放在新申请的子矩阵中
 *有错返回NULL
 *Error
 */
SubMatrix* stor_subMatrix(Matrix *sour, int m, int n, int *row, int *colum)
{
	int i,j;
	SubMatrix* p = NULL;
	if (sour == NULL)
	{
		return NULL;
	}
	p = (SubMatrix *)malloc(sizeof(SubMatrix));
	if (p == NULL)
	{
		//Error
		return NULL;
	}

	p->row = (int *)malloc(sizeof(int)*m);
	util_sort(row, 0, m - 1);
	for (i = 0; i < m-1; i++)
	{
		if (row[i] == row[i + 1])
		{
			//Error
			return NULL;
		}
	}
	if (p->row == NULL)
	{
		//Error
		return NULL;
	}
	memcpy(p->row, row, m * sizeof(int));

	p->colum = (int *)malloc(sizeof(int)*n);
	util_sort(colum, 0, n - 1);
	for (i = 0; i < n - 1; i++)
	{
		if (colum[i] == colum[i + 1])
		{
			//Error
			return NULL;
		}
	}
	if (p->colum == NULL)
	{
		//Error
		return NULL;
	}
	memcpy(p->colum, colum, n * sizeof(int));

	p->sour = sour;

	p->sub = NULL;
	if (!stor_createMatrix(&(p->sub), m, n))
	{
		//Error
		return NULL;
	}
	for(i = 0; i<m; i++)
	{
		for(j = 0; j<n; j++)
		{
			*stor_entry(p->sub, i, j) = *stor_entry(sour, row[i]-1, colum[j]-1);
		}
	}
	return p;
}

/*
 *将子矩阵的修改同步到大矩阵中,Error
 */
Matrix* stor_mergeSubMatrix(SubMatrix *subMatrix)
{
	int i,j;
	Matrix *sour = subMatrix->sour;

	if (subMatrix == NULL)
	{
		return NULL;
	}
	for (i = 0; i<subMatrix->sub->m; i++)
	{
		for (j = 0; j<subMatrix->sub->n; j++)
		{
			*stor_entry(sour, subMatrix->row[i]-1, subMatrix->colum[j]-1) = *stor_entry(subMatrix->sub, i, j);
		}
	}
	return sour;
}

/*
 *横着合并,结果将放到新建的矩阵中,Error
 */
Matrix* stor_mergeColum(Matrix *m1, Matrix *m2)
{
	int i, j;
	int m, n;
	Matrix* p = NULL;

	if (m1 == NULL || m2 == NULL)
	{
		return NULL;
	}
	if (m1->m != m2->m)
	{
		//Error
		return NULL;
	}
	m = m1->m;
	n = m1->n + m2->n;

	if (!stor_createMatrix(&p, m, n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i<m; i++)
	{
		for (j = 0; j<n; j++)
		{
			if (j < m1->n)
				*stor_entry(p,i,j) = *stor_entry(m1,i,j);
			else
				*stor_entry(p,i,j) = *stor_entry(m2,i,j-m1->n);
		}
	}
	return p;
}

/*
 *竖着合并，结果将放到新建的ans中,Error
 */
Matrix* stor_mergeRow(Matrix *m1, Matrix *m2)
{
	int m,n;
	int i,j;
	Matrix *p = NULL;

	if (m1 == NULL || m2 == NULL)
	{
		return NULL;
	}
	if (m1->n != m2->n)
	{
		//Error
		return NULL;
	}
	m = m1->m + m2->m;
	n = m1->n;
	if (!stor_createMatrix(&p, m, n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i<m; i++)
	{
		for (j = 0; j<n; j++)
		{
			if (i < m1->m)
				*stor_entry(p,i,j) = *stor_entry(m1,i,j);
			else
				*stor_entry(p,i,j) = *stor_entry(m2,i-m1->m,j);
		}
	}
	return p;
}

/*
*释放内存，NoError
*/
void stor_freeMatrix(Matrix *p)
{
	int i;
	if (p == NULL)
	{
		return;
	}
	for (i = 0; i < p->m; i++)
	{
		free(p->pd[i]);
	}
	free(p->pd);
	free(p);
}

/*
*释放内存，NoError
*/
static void freeMatrixNode(Matrix_Node *p)
{
	if (p == NULL)
	{
		return;
	}
	stor_freeMatrix(p->matrix);
	free(p->name);
	free(p);
}

/*
*释放内存，NoError
*/
void stor_freeSubMatrix(SubMatrix *p)
{
	if (p == NULL)
	{
		return;
	}
	free(p->colum);
	free(p->row);
	stor_freeMatrix(p->sub);
	free(p);
}

//test
void stor_print(Matrix *p)
{
	int i, j;
	for (i = 0; i < p->m; i++)
	{
		for (j = 0; j < p->n; j++)
		{
			printf("%.2lf  ", *stor_entry(p, i, j));
		}
		printf("\n");
	}
}
