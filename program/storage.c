#include "universe.h"
#include "storage.h"
/*
 *ans����ͨ��������create
 *ansһ�鶼��Ϊ�����洢�ռ�
 */
/*
 *�����ľ��������ͨ�����ֲ���
 */
static Matrix_Node *matrixHeadNode = NULL;
static Matrix *ans[1] = {NULL};//Todo �������õ�

/*
 *Ϊ�������ռ�,pָ���matrixΪNULL
 */
Matrix* stor_createMatrix(Matrix *p, int m, int n)
{
	int i;
	
	if (p == NULL)
	{
		p = (Matrix*)malloc(sizeof(Matrix));
		p->pd = NULL;
		p->pl = NULL;
	}
	p->m = m;
	p->n = n;
	if ((type & 1) == 1)//ʡ����undxpeced
	{
		p->pl = NULL;
		p->pd = (long double**)malloc(sizeof(long double*)*m);
		if (p->pd == NULL)
		{
			//Error
		}
		for (i = 0; i<m; i++)
		{
			p->pd[i] = (long double *)malloc(sizeof(long double)*n);
			if (p->pd[i] == NULL)
			{
				//Error
				return NULL;
			}
		}
	}
	else
	{
		p->pd = NULL;
		p->pl = (long **)malloc(sizeof(long *)*m);
		if (p->pl == NULL)
		{
			//Error
		}
		for (i = 0; i<m; i++)
		{
			p->pl[i] = (long *)malloc(sizeof(long)*n);
			if (p->pl[i] == NULL)
			{
				//Error
				return NULL;
			}
		}
	}
	return p;
}

/*
 *�������Ʒ�����Ӧ����ڵ�
 */
static Matrix_Node* getMatrix_Node(char* matrix_name)
{
	Matrix_Node *p = matrixHeadNode;
	int flag;
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
 *��ʼ��,����ans��ansex��m��n��ֵ����Ϊ��createAns�л���丳ֵ����p��ΪNULL�����
 *
 */
//���Թ�
void stor_ini(void)
{
	matrixHeadNode = (Matrix_Node *)malloc(sizeof(Matrix_Node));
	matrixHeadNode->matrix = NULL;
	matrixHeadNode->left = NULL;
	matrixHeadNode->right = NULL;

	ans[0] = (Matrix *)malloc(sizeof(Matrix));
	ans[0]->pd = NULL;
	ans[0]->pl = NULL;
}

/*
 *���ؾ����һЩ����
 */
//todo �����õ�
int stor_type(Matrix *p)
{
	int type = 0;
	if (p->pd != NULL)
		type |= 1;
	return type;
}

/*
 *����һ���µľ���ڵ�,���½�ans,����Ѵ��������½����ڴ����
 */
//���Թ�
Matrix* stor_create(char* matrix_name, int m, int n, int type)
{
	Matrix_Node *p = NULL;
	int flag;

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
		strcpy(p->name, matrix_name);
		p->left = NULL;
		p->right = NULL;
		p->matrix = (Matrix *)malloc(sizeof(Matrix));
		if (p->matrix == NULL)
		{
			//Error
			return NULL;
		}
		return stor_createMatrix(p->matrix, m, n, type);
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
					strcpy(p->name, matrix_name);
					p->left = NULL;
					p->right = NULL;
					p->matrix = (Matrix *)malloc(sizeof(Matrix));
					if (p->matrix == NULL)
					{
						//Error
						return NULL;
					}
					return stor_createMatrix(p->matrix, m, n, type);
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
					strcpy(p->name, matrix_name);
					p->left = NULL;
					p->right = NULL;
					p->matrix = (Matrix *)malloc(sizeof(Matrix));
					if (p->matrix == NULL)
					{
						//Error
						return NULL;
					}
					return stor_createMatrix(p->matrix, m, n, type);
				}
				else
				{
					p = p->left;
				}
			}
		}
	}
	//has been created before
	if ((p->matrix->m == m)&&(p->matrix->n == n)&&((stor_type(p->matrix)&1) == type))
	{
			return p->matrix;
	}
	else
	{
		free(p->matrix->pd);
		free(p->matrix->pl);
		return stor_createMatrix(p->matrix, m, n, type);
	}
	return NULL;//never happen
}

/*
 *�������Ʒ�����Ӧ����
 */
//���Թ�
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
 *Ϊans��ansex��������·���ռ�
 */
//���Թ�
Matrix* stor_createAns(int label, int m, int n, int type)
{
	if (label >0)
	{
		//Error
		return NULL;
	}
	if (ans[label]->m == m && ans[label]->n == n && (stor_type(ans[label])&1) == type)
	{
		return ans[label];
	}
	free(ans[label]->pl);
	free(ans[label]->pd);
	return stor_createMatrix(ans[label], m, m, type);
}

/*
 *����ĳһ����ĳһԪ�ص�ָ��,�ɵ��÷�ͨ��type�ж�Ϊint����long double
 */
//���Թ�
void* stor_entry(Matrix *p, int m, int n)
{
	if(m<0 || m>p->m
		||n<0 || n>p->n)
	{
		//ErrorԽ��
		return NULL;
	}
	if ((stor_type(p)&1) == 1)
		return p->pd[m]+n;
	else
		return p->pl[m]+n;
}

/*
 *����ans��ansex����ĳһԪ�ص�ָ��,�ɵ��÷�ͨ��type�ж�Ϊint����long double
 */
//���Թ�
void* stor_ansEntry(int label, int m, int n)
{
	if(label > 0)
	{
		//Error
		return NULL;
	}

	if(m<0 || m>ans[label]->m
		||n<0 || n>ans[label]->n)
	{
		//ErrorԽ��
		return NULL;
	}
	if ((stor_type(ans[label])&1) == 1)
		return ans[label]->pd[m]+n;
	else
		return ans[label]->pl[m]+n;
}

//�¼���
Matrix* stor_ans(int label)
{
	return ans[0];
}

/*
 *������sour��ֵ������dest
 *����ͬtype��������
 */
//���Թ�
Matrix* stor_assign(Matrix *dest, Matrix *sour)
{
	int m = dest->m, n = dest->n;
	int dt, st;

	dt = stor_type(dest);
	st = stor_type(sour);
	if ((m != sour->m)||(n != sour->n))
	{
		//Errpr ����ƥ��
		printf("Error");
		return NULL;
	}
	if (dt == st)
	{
		if ((dt & 1) == 1)
			memcpy(dest->pd, sour->pd, m*n*sizeof(long double));
		else
			memcpy(dest->pl, sour->pl, m*n*sizeof(long));
	}
	else
	{
		if ((dt & 1) == 1)//ת������
		{
			free(dest->pd);
			free(dest);
			dest = stor_createMatrix(dest, m, n, 0);
			memcpy(dest->pl, sour->pl, sizeof(long)*m*n);
		}
		else
		{
			free(dest->pl);
			free(dest);
			dest = stor_createMatrix(dest, m, n, 1);
			memcpy(dest->pl, sour->pl, m*n*sizeof(long));
		}
	}
	return dest;
}

/*
 *�������еľ���������Ϣ�����Ӿ���
 *m,nΪ�Ӿ���ĳ��Ϳ�
 *row �� colum�������Ӿ���Ϊ��������Щ�к���,��СΪ1��1��
 *���ɵ��Ӿ������ansex��
 *�д���1���޴���0
 *������Ҫȥ���ظ����еĴ���,������Ҫ����������
 */
Matrix* stor_subMatrix(Matrix *sour, int m, int n, int *row, int *colum)
{
	int i,j;
	Matrix* p = NULL;
	p = stor_createMatrix(p,m,n,stor_type(sour)&1);
	if ((stor_type(ans[1]) & 1) == 0)//������unexpected
	{
		for(i = 0; i<m; i++)
		{
			for(j = 0; j<n; j++)
			{
				*(long *)stor_entry(p, i, j) = *(long *)stor_entry(sour, row[i]-1, colum[j]-1);
			}
		}
	}
	else
	{
		for(i = 0; i<m; i++)
		{
			for(j = 0; j<n; j++)
			{
				*(long double*)stor_entry(p, i, j) = *(long double*)stor_entry(sour, row[i]-1, colum[j]-1);
			}
		}
	}
	return p;
}

/*
 *���Ӿ�����޸�ͬ�����������,����һЩ�����ж�
 */
Matrix* stor_assignSubMatrix(Matrix *dest, Matrix *sour, int m, int n, int *row, int *colum)
{
	int i,j;
	int dt, st;
	dt = stor_type(dest);
	st = stor_type(sour);

	if(dt ==0 && st == 1)//��long�ͱ��long double��
	{
		dest->pd = (long double **)malloc(sizeof(long double *)*dest->m);
		if (dest->pd == NULL)
		{
			//Error
		}
		for (i = 0; i<dest->m; i++)
		{
			dest->pd[i] = (long double *)malloc(sizeof(long double)*dest->n);
			if (dest->pd[i] == NULL)
			{
				//Error
			}
			for (j = 0; j<dest->n; j++)
			{
				*(long double *)stor_entry(dest,i,j) = *(long *)stor_entry(dest,i,j);
			}
		}
		free(dest->pl);
		dest->pl = NULL;
	}

	dt = stor_type(dest);
	if ((st & 1) == 0)//ʡ����unexpectedError
	{
		if((dt & 1) == 1)//ʡ����unexpectedError
		{
			for (i = 0; i<m; i++)
			{
				for (j = 0; j<n; j++)
				{
					*(long double *)stor_entry(dest,row[i]-1,colum[j]-1) = *(long *)stor_entry(sour, i, j);
				}
			}
		}
		else
		{
			for (i = 0; i<m; i++)
			{
				for (j = 0; j<n; j++)
				{
					*(long *)stor_entry(dest, row[i]-1, colum[j]-1) = *(long *)stor_entry(sour, i, j);
				}
			}
		}
	}
	else
	{
		for (i = 0; i<m; i++)
		{
			for (j = 0; j<n; j++)
			{
				*(long double *)stor_entry(dest, row[i]-1, colum[j]-1) = *(long double *)stor_entry(sour, i, j);
			}
		}
	}
	return dest;
}

/*
 *���źϲ�,������ŵ��½��ľ�����
 */
Matrix* stor_mergeColum(Matrix *m1, Matrix *m2)
{
	int i, j;
	int t1, t2;
	int m, n;
	Matrix* p = NULL;

	if (m1->m != m2->m)
	{
		//Error
		return NULL;
	}
	t1 = stor_type(m1)&1;
	t2 = stor_type(m2)&1;
	m = m1->m;
	n = m1->n + m2->n;
	if (t1 == 0 && t2 == 0)
	{
		p = stor_createMatrix(p,m,n,0);
		for (i = 0; i<m; i++)
		{
			for (j = 0; j<n; j++)
			{
				if (j < m1->n)
					*(long *)stor_entry(p,i,j) = *(long *)stor_entry(m1,i,j);
				else
					*(long *)stor_entry(p,i,j) = *(long *)stor_entry(m2,i,j-m1->n);
			}
		}
		return p;
	}
	if (t1 == 0 && t2 == 1)
	{
		p = stor_createMatrix(p, m, n, 1);
		for (i = 0; i<m; i++)
		{
			for (j = 0; j<n; j++)
			{
				if (j < m1->n)
					*(long double *)stor_entry(p,i,j) = *(long *)stor_entry(m1,i,j);
				else
					*(long double *)stor_entry(p,i,j) = *(long double *)stor_entry(m2,i,j-m1->n);
			}
		}
		return p;
	}
	if (t1 == 1 && t2 == 0)
	{
		p = stor_createMatrix(p, m, n, 1);
		for (i = 0; i<m; i++)
		{
			for (j = 0; j<n; j++)
			{
				if (j < m1->n)
					*(long double *)stor_entry(p,i,j) = *(long double *)stor_entry(m1,i,j);
				else
					*(long double *)stor_entry(p,i,j) = *(long *)stor_entry(m2,i,j-m1->n);
			}
		}
		return p;
	}
	if (t1 == 1 && t2 == 1)
	{
		p = stor_createMatrix(p, m, n, 1);
		for (i = 0; i<m; i++)
		{
			for (j = 0; j<n; j++)
			{
				if (j < m1->n)
					*(long double *)stor_entry(p,i,j) = *(long double *)stor_entry(m1,i,j);
				else
					*(long double *)stor_entry(p,i,j) = *(long double *)stor_entry(m2,i,j-m1->n);
			}
		}
		return p;
	}
	return NULL;//never happen
}

/*
 *���źϲ���������ŵ��½���ans��
 */
Matrix* stor_mergeRow(Matrix *m1, Matrix *m2)
{
	int t1, t2;
	int m,n;
	int i,j;
	Matrix *p = NULL;

	if (m1->n != m2->n)
	{
		//Error
		return NULL;
	}
	t1 = stor_type(m1);
	t2 = stor_type(m2);
	m = m1->m + m2->m;
	n = m1->n;
	if (t1 == 0 && t2 == 0)
	{
		p = stor_createMatrix(p,m,n,0);
		for (i = 0; i<m; i++)
		{
			for (j = 0; j<n; j++)
			{
				if (i < m1->m)
					*(long *)stor_entry(p,i,j) = *(long *)stor_entry(m1,i,j);
				else
					*(long *)stor_entry(p,i,j) = *(long *)stor_entry(m2,i-m1->m,j);
			}
		}
		return p;
	}
	if (t1 == 1 && t2 == 0)
	{
		p = stor_createMatrix(p,m,n,1);
		for (i = 0; i<m; i++)
		{
			for (j = 0; j<n; j++)
			{
				if (i < m1->m)
					*(long double *)stor_entry(p,i,j) = *(long double *)stor_entry(m1,i,j);
				else
					*(long double *)stor_entry(p,i,j) = *(long *)stor_entry(m2,i-m1->m,j);
			}
		}
		return p;
	}
	if (t1 == 0 && t2 == 1)
	{
		p = stor_createMatrix(p,m,n,1);
		for (i = 0; i<m; i++)
		{
			for (j = 0; j<n; j++)
			{
				if (i < m1->m)
					*(long double *)stor_entry(p,i,j) = *(long *)stor_entry(m1,i,j);
				else
					*(long double *)stor_entry(p,i,j) = *(long double *)stor_entry(m2,i-m1->m,j);
			}
		}
		return p;
	}
	if (t1 == 1 && t2 == 1)
	{
		p = stor_createMatrix(p,m,n,1);
		for (i = 0; i<m; i++)
		{
			for (j = 0; j<n; j++)
			{
				if (i < m1->m)
					*(long double *)stor_entry(p,i,j) = *(long double *)stor_entry(m1,i,j);
				else
					*(long double *)stor_entry(p,i,j) = *(long double *)stor_entry(m2,i-m1->m,j);
			}
		}
		return p;
	}
	return NULL;//never happen
}
