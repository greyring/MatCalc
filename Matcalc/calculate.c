#include "storage.h"
#include "universe.h"
#include "calculate.h"
#include "util.h"

/*
* r is script, return the script in line r that is not zero
* if the line is all zero, return -1,NoError
*/
static int isZeroLine(Matrix *p, int r);

/*
*r1 and r2 are scripts,NoError
*/
static Matrix* swapRow(Matrix *p, int r1, int r2)
{
	int i;
	for (i = 0; i < p->n; i++)
	{
		util_swap(stor_entry(p, r1, i), stor_entry(p, r2, i));
	}
	return p;
}

/*
*c1 and c2 are scripts,NoError
*/
static Matrix* swapColum(Matrix *p, int c1, int c2)
{
	int i;
	for (i = 0; i < p->m; i++)
	{
		util_swap(stor_entry(p, i, c1), stor_entry(p, i, c2));
	}
	return p;
}

/*
* rd = rd+rs*mul, rd,rs are scripts,NoError
*/
static Matrix* addRow(Matrix *p, int rd, int rs, double mul)
{
	int i;
	for (i = 0; i < p->n; i++)
	{
		*stor_entry(p, rd, i) += mul * *stor_entry(p, rs, i);
	}
	return p;
}

/*
* cd = cd+cs*mul, cd,cs are scripts,NoError
*/
static Matrix* addColum(Matrix *p, int cd, int cs, double mul)
{
	int i;
	for (i = 0; i < p->m; i++)
	{
		*stor_entry(p, i,cd) += mul * *stor_entry(p, i, cs);
	}
	return p;
}

/*
* r is script,NoError
*/
static Matrix* mulRow(Matrix *p, int r, double mul)
{
	int i;
	for (i = 0; i < p->n; i++)
	{
		*stor_entry(p, r, i) *= mul;
	}
	return p;
}

/*
* c is script,NoError
*/
static Matrix* mulColum(Matrix *p, int c, double mul)
{
	int i;
	for (i = 0; i < p->m; i++)
	{
		*stor_entry(p, i, c) *= mul;
	}
	return p;
}

/*
*Create a random Matrix, each entry is double between 0 to 1, will release ans, Error return NULL
*/
Matrix* calc_rand(int m, int n)
{
	int i, j;
	if (!stor_createAns(m, n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i<m; i++)
	{
		for (j = 0; j<n; j++)
		{
			*stor_entry(ans, i,j) = rand()*1.0/RAND_MAX;
		}
	}
	return ans;
}

/*
*Create a zero Matrix, will release ans, Error return NULL
*/
Matrix* calc_zeros(int m, int n)
{
	int i, j;
	if (!stor_createAns(m, n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i<m; i++)
	{
		for (j = 0; j<n; j++)
		{
			*stor_entry(ans, i,j) = 0;
		}
	}
	return ans;
}

/*
*Create a eye Matrix, will release ans, Error return NULL
*/
Matrix* calc_eye(int n)
{
	int i, j;
	if (!stor_createAns(n, n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n; j++)
		{
			if (i != j)
				*stor_entry(ans, i,j) = 0;
			else
				*stor_entry(ans, i,j) = 1;
		}
	}
	return ans;
}

/*
*Add two matrixs, Error
*/
Matrix* calc_add(Matrix* p1, Matrix* p2)
{
	Matrix *oldAns = ans;
	int i, j;

	if (p1 == NULL || p2 == NULL)
	{
		//Error
		return NULL;
	}
	else if (p1->m != p2->m || p1->n != p2->n)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(p1->m, p1->n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p1->m; i++)
		for (j = 0; j < p1->n; j++)
			*stor_entry(ans, i, j) = *stor_entry(p1, i, j)+ *stor_entry(p2, i, j);
	stor_freeMatrix(oldAns);
	return ans;
}

/*
*Sub two matrixs, Error
*/
Matrix* calc_sub(Matrix* p1, Matrix* p2) 
{
	Matrix *oldAns = ans;
	int i, j;
	if (p1 == NULL || p2 == NULL)
	{
		//Error
		return NULL;
	}
	else if (p1->m != p2->m || p1->n != p2->n)
	{
		//Error
		return NULL;
	}
    ans = NULL;	
	if (!stor_createAns(p1->m, p1->n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p1->m; i++)
		for (j = 0; j < p1->n; j++)
			*stor_entry(ans, i, j) = *stor_entry(p1, i, j) - *stor_entry(p2, i, j);
	stor_freeMatrix(oldAns);

	return ans;
}

/*
*Mul two matrixs, Error
*/
Matrix* calc_mul(Matrix* p1, Matrix* p2) 
{
	Matrix *oldAns = ans;
	int i, j, k;
	if (p1 == NULL || p2 == NULL)
	{
		//Error
		return NULL;
	}
	else if (p1->n != p2->m)
	{
		//Error
		return NULL;
	}

	ans = NULL;
	if (!stor_createAns(p1->m, p2->n))
	{
		//Error
		return NULL;
	}

	for (i = 0; i < p1->m; i++)
		for (j = 0; j < p2->n; j++)
		{
			*stor_entry(ans, i, j) = 0;
			for (k = 0; k < p1->n; k++)
				*stor_entry(ans, i, j) += *stor_entry(p1, i, k)* *stor_entry(p2, k, j);
		}
	stor_freeMatrix(oldAns);
	return ans;
}

/*
*the inverse of matrix, Error
*/
Matrix* calc_inverse(Matrix* p)
{
	Matrix *oldAns = ans;
	int i, j, k;
	int n;
	double temp, temp2;
	Matrix *t = NULL;

	if (p == NULL){
		//Error
		return NULL;
	}
	else if (p->m != p->n){
		//Error
		return NULL;
	}
	n = p->m;

	if (!stor_createMatrix(&t, n, n))
	{
		//Error
		return NULL;
	}
	if (!stor_assign(t, p))
	{
		//Error
		return NULL;
	}

	ans = NULL;
	if (!calc_eye(n))
	{
		//Error
		return NULL;
	}

	for (i = 0; i < n; i++)
	{
		k = i;         //the first line that the i colum is not zero
		while (k < n && util_isZero(*stor_entry(t, i, k))) k++;//一定要是列增
		if (k >= n)
		{
			//Error,非满秩矩阵
			return NULL;
		}
		if (k != i)
		{
			swapColum(t, i, k);
			swapColum(ans, i, k);
		}
		//k is no use since
		temp = *stor_entry(t, i, i);
		if (util_isZero(temp))
		{
			//Error
			return NULL;
		}
		mulRow(t, i, (double)1.0 / temp);
		mulRow(ans, i, (double)1.0 / temp);
		//the [i][i] is 1 now
		for (j = i + 1; j < n; j++)
		{
			if (!util_isZero(*stor_entry(t, j, i)))
			{
				temp2 = *stor_entry(t, j, i);
				addRow(t, j, i, -temp2);
				addRow(ans, j, i, -temp2);
			}
		}
	}
	//现在是阶梯型,阶梯头是1
	for (i = n - 1; i >= 0; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if (!util_isZero(*stor_entry(t, j, i)))
			{
				temp2 = *stor_entry(t, j, i);
				addRow(t, j, i, -temp2);
				addRow(ans, j, i, -temp2);
			}
		}
	}
	stor_freeMatrix(t);
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* X * p2 = p1, p1/p2 = X, Error
*/
Matrix* calc_div(Matrix* p1, Matrix* p2)
{
	Matrix *oldAns = ans;

	if (p1 == NULL || p2 == NULL)
	{
		//Error
		return NULL;
	}

	ans = NULL;
	
	if (!calc_mul(p1, calc_inverse(p2)))
	{
		//Error
		return NULL;
	}
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* p % k,Error
*/
Matrix* calc_numMod(Matrix* p, long k)
{
	int i, j;
	Matrix *oldAns = ans;

	if (p == NULL)
	{
		//Error
		return NULL;
	}
	if (k == 0)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(p->m, p->n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p->m; i++)
	{
		for (j = 0; j < p->n; j++)
		{
			if (!util_isLong(*stor_entry(p, i, j)))
			{
				//Error
				stor_freeMatrix(ans);
				ans = oldAns;
				return NULL;
			}
			*stor_entry(ans, i, j) = (long)floor(*stor_entry(p, i, j) + 0.5) % k;
		}
	}
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* Error
*/
Matrix* calc_ex(Matrix* p,long ex)
{
	Matrix *oldAns = ans;
	Matrix *temp = NULL, *temp2 = NULL;
	int n;

	if (p == NULL)
	{
		//Error
		return NULL;
	}
	if (p->m != p->n)
	{
		//Error
		return NULL;
	}
	n = p->m;
	if (ex < 0)
	{
		ans = NULL;
		if (!(temp = calc_inverse(p)))
		{
			//Error
			return NULL;
		}
		ans = NULL;
		ex = -ex;
	}
	else
	{
		if (!stor_createMatrix(&temp, n, n))
		{
			//Error
			return NULL;
		}
		if (!stor_assign(temp, p))
		{
			//Error
			return NULL;
		}
		ans = NULL;
	}

	if (!calc_eye(n))
	{
		//Error
		return NULL;
	}
	while (ex)
	{
		if (ex % 2)
		{
			if (!calc_mul(temp, ans))
			{
				//Error
				return NULL;
			}
		}
		temp2 = ans;               //protect ans
		ans = NULL;                //protect ans
		if (!calc_mul(temp, temp))
		{
			//Error
			return NULL;
		}
		stor_freeMatrix(temp);
		temp = ans;
		ans = temp2;               //protect ans
		ex /= 2;
	}
	stor_freeMatrix(temp);
	stor_freeMatrix(oldAns);

	return ans;
}

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_sum(Matrix* p)
{
	Matrix *oldAns = ans;
	int i, j;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(1, 1))
	{
		//Error
		return NULL;
	}
	*stor_entry(ans, 0, 0) = 0;
	for (i = 0; i < p->m; i++)
		for (j = 0; j < p->n; j++)
			*stor_entry(ans, 0, 0) += *stor_entry(p, i, j);
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_max(Matrix* p)
{
	Matrix *oldAns = ans;
	int i, j;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(1, 1))
	{
		//Error
		return NULL;
	}
	*stor_entry(ans, 0, 0) = 0;
	for (i = 0; i < p->m; i++)
		for (j = 0; j < p->n; j++)
			if (*stor_entry(p, i, j)> *stor_entry(ans, 0, 0))
				*stor_entry(ans, 0, 0) = *stor_entry(p, i, j);
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_min(Matrix* p)
{
	Matrix *oldAns = ans;
	int i, j;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(1, 1))
	{
		//Error
		return NULL;
	}
	*stor_entry(ans, 0, 0) = *stor_entry(p, 0, 0);
	for (i = 0; i < p->m; i++)
		for (j = 0; j < p->n; j++)
			if (*stor_entry(p, i, j) < *stor_entry(ans, 0, 0))
				*stor_entry(ans, 0, 0) = *stor_entry(p, i, j);
	stor_freeMatrix(oldAns);
	return ans;
}

/*
*Error
*/
Matrix* calc_round(Matrix* p)
{
	Matrix *oldAns = ans;
	int i, j;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(p->m, p->n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p->m; i++)
		for (j = 0; j < p->n; j++)
			*stor_entry(ans, i, j) = (long)(*stor_entry(p, i, j)+0.5);
	stor_freeMatrix(oldAns);
	return ans;
}

/*
*Error
*/
Matrix* calc_upper(Matrix* p)
{
	Matrix *oldAns = ans;
	int i, j;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(p->m, p->n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p->m; i++)
		for (j = 0; j < p->n; j++)
			*stor_entry(ans, i, j) = ceil(*stor_entry(p, i, j));
	stor_freeMatrix(oldAns);
	return ans;
}

/*
*Error
*/
Matrix* calc_lower(Matrix* p)
{
	Matrix *oldAns = ans;
	int i, j;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(p->m, p->n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p->m; i++)
		for (j = 0; j < p->n; j++)
			*stor_entry(ans,i, j) = floor(*stor_entry(p, i, j));
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* M^T, Error
*/
Matrix* calc_trans(Matrix* p)
{
	Matrix *oldAns = ans;
	int i, j;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(p->n, p->m))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p->n; i++)
		for (j = 0; j < p->m; j++)
			*stor_entry(ans, i, j) = *stor_entry(p, j, i);
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_det(Matrix* p)
{
	Matrix *oldAns = ans;
	Matrix *temp = NULL;
	int i, j, k, n;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	if (p->m != p->n)
	{
		//Error
		return NULL;
	}
	n = p->m;
	if (!stor_createMatrix(&temp, n, n))
	{
		//Error
		return NULL;
	}
	if (!stor_assign(temp, p))
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(1, 1))
	{
		//Error
		return NULL;
	}
	*stor_entry(ans, 0, 0) = 1;
	for (i = 0; i < n; i++)
	{
		k = i;
		while (k < n && util_isZero(*stor_entry(temp, i, k))) k++;
		if (k >= n)
		{
			*stor_entry(ans, 0, 0) = 0;
			return ans;
		}
		if (i != k)
		{
			swapColum(temp, i, k);
		}
		*stor_entry(ans, 0, 0) *= *stor_entry(temp, i, i);
		if (util_isZero(*stor_entry(temp, i, i)))
		{
			//Error
			return NULL;
		}
		mulRow(temp, i, (double)1.0/ *stor_entry(temp, i, i));
		for (j = i + 1; j < n; j++)
		{
			if (!util_isZero(*stor_entry(temp, j, i)))
			{
				addRow(temp, j, i, -*stor_entry(temp, j, i));
			}
		}
	}
	stor_freeMatrix(temp);
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_norm(Matrix* p)
{
	if (!calc_dot(p, p))
	{
		//Error
		return NULL;
	}
	*stor_entry(ans, 0, 0) = sqrt(*stor_entry(ans, 0, 0));
	return ans;
}

/*
* return value(ans only 1*1) should be treated as int, Error
*/
Matrix* calc_rank(Matrix *p)
{
	Matrix *oldAns = ans;
	int i, j, k;
	int count = 0;

	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (p->m > p->n)
	{
		if (!calc_trans(p))
		{
			//Error
			return NULL;
		}
	}
	else
	{
		if (!stor_createAns(p->m, p->n))
		{
			//Error
			return NULL;
		}
		if (!stor_assign(ans, p))
		{
			//Error
			return NULL;
		}
	}
	if (ans == NULL)
	{
		//Error
		return NULL;
	}
	for (i = 0; i < ans->m; i++)
	{
		k = count;
		while (k < ans->n && util_isZero(*stor_entry(ans, i, k))) k++;
		if (k >= ans->n)
		{
			continue;
		}
		if (k != count)
		{
			swapColum(ans, count, k);
		}
		if (util_isZero(*stor_entry(ans, i, count)))
		{
			//Error
			return NULL;
		}
		mulRow(ans, i, (double)1.0 / *stor_entry(ans, i, count));
		if (!ans)
		{
			//Error
			return NULL;
		}
		for (j = i + 1; j < ans->m; j++)
		{
			addRow(ans, j, i, -*stor_entry(ans, j, count));
		}
		count++;
	}
	if (!stor_createAns(1, 1))
	{
		//Error
		return NULL;
	}
	*stor_entry(ans, 0, 0) = count;
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_dot(Matrix* p1, Matrix* p2)
{
	Matrix *oldAns = ans;
	int i;
	
	if (p1 == NULL || p2 == NULL)
	{
		//Error
		return 0;
	}
	if (p1->m ==1 && p2->m == 1 && p1->n == p2->n)
	{ 
		ans = NULL;
		if (!stor_createAns(1, 1))
		{
			//Error 
			return NULL;
		}
		*stor_entry(ans, 0, 0) = 0;
		for (i = 0; i < p1->n; i++)
		{
			*stor_entry(ans, 0, 0) += *stor_entry(p1, 0, i)* *stor_entry(p2, 0, i);
		}
		stor_freeMatrix(oldAns);
		return ans;
	}
	if (p1->n == 1 && p2->n == 1 && p1->m == p2->m)
	{
		ans = NULL;
		if (!stor_createAns(1, 1))
		{
			//Error 
			return NULL;
		}
		*stor_entry(ans, 0, 0) = 0;
		for (i = 0; i < p1->m; i++)
		{
			*stor_entry(ans, 0, 0) += *stor_entry(p1, i, 0)* *stor_entry(p2, i, 0);
		}
		stor_freeMatrix(oldAns);
		return ans;
	}
	return NULL;
}

/*
* return value(ans only 1*1) should be treated as angel(0~pi), Error
*/
Matrix* calc_angle(Matrix* p1, Matrix* p2)
{
	Matrix *oldAns = ans;
	double op1, op2, op3;
	if (p1 == NULL || p2 == NULL){
		//Error
		return NULL;
	}
	ans = NULL;
	if (!calc_dot(p1, p2))
	{
		//Error
		return NULL;
	}
	op1 = *stor_entry(ans, 0, 0);
	if (!calc_norm(p1))
	{
		//Error
		return NULL;
	}
	op2 = *stor_entry(ans, 0, 0);
	if (!calc_norm(p2))
	{
		//Error
		return NULL;
	}
	op3 = *stor_entry(ans, 0, 0);
	if (util_isZero(op2) || util_isZero(op3))
	{
		//Error
		return NULL;
	}
	if (!stor_createAns(1, 1))
	{
		//Error
		return NULL;
	}
	*stor_entry(ans, 0, 0) = acos(op1 / (op2*op3));
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* r is script, return the script in line r that is not zero
* if the line is all zero, return -1,NoError
*/
static int isZeroLine(Matrix *p, int r)
{
	int i;
	
	for (i = 0; i < p->n; i++)
	{
		if (!util_isZero(*stor_entry(p, r, i)))
		{
			return i;
		}
	}
	return -1;
}

/*
* 行最简式，Error
*/
Matrix* calc_rref(Matrix* p)
{
	Matrix *oldAns = ans;
	int i, j, k;
	int l = 0;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(p->m, p->n))
	{
		//Error
		return NULL;
	}
	if (!stor_assign(ans, p))
	{
		//Error
		return NULL;
	}
	if (!ans)
	{
		//Error
		return NULL;
	}
	for (i = 0; i < ans->m; i++)
	{
		k = isZeroLine(ans, i);
		if (k == -1)
		{
			continue;
		}
		if (util_isZero(*stor_entry(ans, i, k)))
		{
			//Error
			return NULL;
		}
		mulRow(ans, i, (double)1.0 / *stor_entry(ans, i, k));
		if (!ans)
		{
			//Error
			return NULL;
		}
		for (j = 0; j < ans->m; j++)
		{
			if (i != j && !util_isZero(*stor_entry(ans, j, k)))
			{
				addRow(ans, j, i, -*stor_entry(ans, j, k));
			}
		}
	}
	l = 0;
	for (i = 0; i < p->n; i++)
	{
		for (j = i; j < p->m; j++)
		{
			if (!util_isZero(*stor_entry(ans, j, i))) break;
		}
		if (j < p->m)
		{
			if (i != j)
				swapRow(ans, l, j);
			l++;
		}
	}
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* a*M, Error
*/
Matrix* calc_numMul(Matrix* p, double mul)
{
	Matrix *oldAns = ans;
	int i, j;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(p->m, p->n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p->m; i++)
	{
		for (j = 0; j < p->n; j++)
		{
			*stor_entry(ans, i, j) = *stor_entry(p, i, j) * mul;
		}
	}
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* M.^ex, Error
*/
Matrix* calc_everyEx(Matrix *p, int ex)
{
	Matrix *oldAns = ans;
	int i, j;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createAns(p->m, p->n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p->m; i++)
	{
		for (j = 0; j < p->n; j++)
		{
			if (ex < 0 && util_isZero(*stor_entry(p, i, j)))
			{
				//Error
				return NULL;
			}
			*stor_entry(ans, i, j) = util_ex(*stor_entry(p, i, j), ex);
		}
	}
	stor_freeMatrix(oldAns);
	return (ans);
}

//Todo eig

/*
* if is Upper triangle matrix, NoError
*/
static int isUpTrian(Matrix *p)
{
	int i, j;
	for (i = 0; i < p->m; i++)
	{
		for (j = 0; j < p->n; j++)
		{
			if (i > j && !util_isZero(*stor_entry(p, i, j))) return 0;
		}
	}
	return 1;
}

/*
* H*x will make [c+2~m] zero, c is script, Error
*/
static Matrix* householder(Matrix *v, int c)
{
	Matrix* oldAns = ans;
	Matrix* oldAns2;
	Matrix* temp = NULL;
	int i;
	double norm;
	int k;
	if (v == NULL)
	{
		//Error
		return NULL;
	}
	if (v->n != 1)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!stor_createMatrix(&temp, v->m, 1))
	{
		//Error
		return NULL;
	}
	if (!stor_assign(temp, v))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < c; i++)
	{
		*stor_entry(temp, i, 0) = 0;
	}
	if (!calc_norm(temp))//NULL
	{
		//Error
		return NULL;
	}
	norm = *stor_entry(ans, 0, 0);
	k = c;
	*stor_entry(temp, k, 0) += norm;
	if (!calc_norm(temp))
	{
		//Error
		return NULL;
	}
	norm = *stor_entry(ans, 0, 0);
	if (util_isZero(norm))
	{
		//Error
		return NULL;
	}
	if (!calc_numMul(calc_mul(temp, calc_trans(temp)),(double)2.0/(norm*norm)))
	{
		//Error
		return NULL;
	}
	oldAns2 = ans;
	ans = NULL;
	if (!calc_sub(calc_eye(temp->m), oldAns2))
	{
		//Error
		return NULL;
	}
	stor_freeMatrix(oldAns2);
	stor_freeMatrix(oldAns);
	return ans;
}

/*
* the eigvalue, 有可能在数学上出问题（虚数） Error
*/
Matrix* calc_eig(Matrix* p)
{
	Matrix *oldAns = ans, *oldAns2;
	Matrix *temp = NULL,*temp2 = NULL;
	Matrix *v = NULL;
	double r1;
	int i, j, k = 0;
	if (p == NULL)
	{
		//Error
		return NULL;
	}
	if (p->m != p->n)
	{
		//Error
		return NULL;
	}
	if (!stor_createMatrix(&v, p->m, 1))
	{
		//Error
		return NULL;
	}
	if (!stor_createMatrix(&temp, p->m, p->n))
	{
		//Error
		return NULL;
	}
	if (!stor_createMatrix(&temp2, p->m, p->n))
	{
		//Error
		return NULL;
	}
	if (!stor_assign(temp, p))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p->n - 2; i++)
	{
		for (j = 0; j < p->m; j++)
		{
			*stor_entry(v, j, 0) = *stor_entry(temp, j, i);
		}
		if (!householder(v, i + 1))
		{
			//Error
			return NULL;
		}
		oldAns2 = ans;
		ans = NULL;
		if (!calc_mul(calc_mul(oldAns2, temp), oldAns2))  //H*A*H
		{
			//Error
			return NULL;
		}
		stor_freeMatrix(temp);
		temp = ans;
		ans = NULL;
		stor_freeMatrix(oldAns2);
	}
	//temp is 拟三角阵
	while (k<=500 && !isUpTrian(temp))//最多500次迭代
	{
		k++;
		ans = NULL;
		if (!(temp2 = calc_eye(p->n)))
		{
			//Error
			return NULL;
		}
		ans = NULL;
		for (i = 0; i < p->n - 1; i++)
		{
			if (!calc_eye(p->n))
			{
				//Error
				return NULL;
			}
			r1 = sqrt(*stor_entry(temp, i, i) * *stor_entry(temp, i, i)
				+ *stor_entry(temp, i + 1, i)* *stor_entry(temp, i + 1, i));
			if (util_isZero(r1))
			{
				//Error
				return NULL;
			}
			*stor_entry(ans, i, i) = *stor_entry(temp, i, i) / r1;
			*stor_entry(ans, i + 1, i + 1) = *stor_entry(temp, i, i) / r1;
			*stor_entry(ans, i + 1, i) = -*stor_entry(temp, i + 1, i) / r1;
			*stor_entry(ans, i, i + 1) = *stor_entry(temp, i + 1, i) / r1;
			oldAns2 = ans;
			ans = NULL;
			if (!calc_mul(temp2, calc_trans(oldAns2)))//Q
			{
				//Error
				return NULL;
			}
			stor_freeMatrix(temp2);
			temp2 = ans;
			ans = NULL;
			if (!calc_mul(oldAns2, temp))//R
			{
				//Error
				return NULL;
			}
			stor_freeMatrix(temp);
			temp = ans;
			ans = NULL;
			stor_freeMatrix(oldAns2);
		}
		if (!calc_mul(temp, temp2))
		{
			//Error
			return NULL;
		}
		stor_freeMatrix(temp);
		temp = ans;
	}
	//优化迭代， 好像可以证明RQ一定是拟上三角阵
	for (i = 0; i < p->m; i++)
	{
		for (j = 0; j < p->n; j++)
		{
			if (i != j) *stor_entry(ans, i, j) = 0;
		}
	}
	return ans;
}

Matrix* calc_solve(Matrix *p)
{
	Matrix *oldAns = ans;
	Matrix *temp = NULL;
	int *one = NULL;
	int i, j, k, l, rank;

	if (p == NULL)
	{
		//Error
		return NULL;
	}
	ans = NULL;
	if (!calc_rref(p))
	{
		//Error
		return NULL;
	}
	temp = ans;
	ans = NULL;
	if (!(one = (int*)malloc(temp->m *sizeof(int))))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < temp->m; i++)
	{
		one[i] = -1;
	}
	for (i = 0; i < temp->m; i++)
	{
		j = isZeroLine(temp, i);
		if (j == temp->n - 1)
		{
			//No solution
			stor_freeMatrix(temp);
			stor_freeMatrix(oldAns);
			return NULL;
		}

		if (j == -1) break;
		one[i] = j;
	}
	if (!calc_zeros(temp->n - 1, temp->n - i))
	{
		//Error
		return NULL;
	}
	rank = i;//rank
	l = k = 0;
	for (i = 0; i < temp->n - 1; i++)
	{
		if (i != one[k])
		{
			for (j = 0; j < rank; j++)
			{
				*stor_entry(ans, one[j], l) = -*stor_entry(temp, j, i);
			}
			*stor_entry(ans, i, l) = 1;
			l++;
			if (l == temp->n - rank -1 )
			{
				break;
			}
		}
		else
		{
			k++;
		}
	}
	for (i = 0; i < rank; i++)
	{
		*stor_entry(ans, one[i], l) = *stor_entry(temp, i, temp->n-1);
	}
	stor_freeMatrix(temp);
	stor_freeMatrix(oldAns);
	return ans;
}

/*
*一般的乘法
*/
Matrix* calc_numEx(double d, long ex)
{
	if (!stor_createAns(1, 1))
	{
		//Error
		return NULL;
	}
	if (ex < 0 && util_isZero(d))
	{
		//Error
		return NULL;
	}
	*stor_entry(ans, 0, 0) = util_ex(d, ex);
	return ans;
}
