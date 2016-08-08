#include "storage.h"
#include "universe.h"
#include "calculate.h"
#include "util.h"

/*
*返回的指针都是ans，如果要弄个新的矩阵要自己弄
*/

static double **ex1_mul(double** p1, double** p2, int m);
static long **ex0_mul(long** p1, long** p2, int m);
static void ex1(double** p, int m);
static void ex0(long** p, int m);
static void exchange_row(double *a, double *b, int n);
static void mul_row(double *a, double k, int n);
static void add_row(double *a1, double *a2, int k, int n);
static int rank(double** p, int m, int n);
static double dot11(double** p1, double** p2, int m, int n);
static double dot10(double** p1, long** p2, int m, int n);
static double dot01(long** p1, double** p2, int m, int n);
static double dot00(long** p1, long** p2, int m, int n);
static void rref(int m);
static void swap(int m, int max_row, int n);
static int feature_vector(double** temp, int n);
static int feature_value(double** temp, int n);
static void opposite1(double** p, int m, int n);
static void opposite0(long** p, int m, int n);

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
*Create a random Matrix, each entry is double between 0 to 1, Error return NULL
*/
Matrix* calc_rand_Matrix(int m, int n)
{
	int i, j;
	if (!stor_creatAns(m, n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i<m; i++)
	{
		for (j = 0; j<n; j++)
		{
			*stor_ansEntry(i,j) = rand()*1.0/RAND_MAX;
		}
	}
	return ans;
}

/*
*Create a zero Matrix, Error return NULL
*/
Matrix* calc_zeros_Matrix(int m, int n)
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
			*stor_ansEntry(i,j) = 0;
		}
	}
	return ans;
}

/*
*Create a eye Matrix, Error return NULL
*/
Matrix* calc_eye_Matrix(int n)
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
				*stor_ansEntry(i,j) = 0;
			else
				*stor_ansEntry(i,j) = 1;
		}
	}
	return ans;
}

/*
*Add two matrixs, Error
*/
Matrix* calc_add_Matrix(Matrix* p1, Matrix* p2)
{
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
	if (!stor_createAns(p1->m, p1->n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p1->m; i++)
		for (j = 0; j < p1->n; j++)
			*stor_ansEntry(i, j) = *stor_entry(p1, i, j)+ *stor_entry(p2, i, j);
	return ans;
}

/*
*Sub two matrixs, Error
*/
Matrix* calc_sub_Matrix(Matrix* p1, Matrix* p2) 
{
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
	
	if (!stor_createAns(p1->m, p1->n))
	{
		//Error
		return NULL;
	}
	for (i = 0; i < p1->m; i++)
		for (j = 0; j < p1->n; j++)
			*stor_ansEntry(i, j) = *stor_entry(p1, i, j) - *stor_entry(p2, i, j);

	return ans;
}

/*
*Mul two matrixs, Error
*/
Matrix* calc_mul_Matrix(Matrix* p1, Matrix* p2) {
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

	if (!stor_createAns(p1->m, p2->n))
	{
		//Error
		return NULL;
	}

	for (i = 0; i < p1->m; i++)
		for (j = 0; j < p2->n; j++)
		{
			*stor_ansEntry(i, j) = 0;
			for (k = 0; k < p1->n; k++)
				*stor_ansEntry(i, j) += *stor_entry(p1, i, k)* *stor_entry(p2, k, j);
		}
	return ans;
}

/*
*the reverse of matrix, Error
*Todo test
*/
Matrix* calc_reverse_Matrix(Matrix* p){
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

	if (!stor_createMatrix(t, n, n))
	{
		//Error
		return NULL;
	}
	stor_assign(t, p);

	if (!calc_eye_Matrix(n))
	{
		//Error
		return NULL;
	}

	for (i = 0; i < n; i++)
	{
		k = i;         //the first line that the i colum is not zero
		while (k < n && util_isZero(*stor_entry(t, k, i))) k++;
		if (k >= n)
		{
			//Error,非满秩矩阵
			return NULL;
		}
		if (k != i)
		{
			for (j = 0; j < n; j++)
			{
				util_swap(stor_entry(t, i, j), stor_entry(t, k, j));
				util_swap(stor_ansEntry(i, j), stor_ansEntry(k, j));
			}
		}
		//k is no use since
		temp = *stor_entry(t, i, i);
		for (j = 0; j < n; j++)
		{
			*stor_entry(t, i, j) /= temp;
			*stor_ansEntry(i, j) /= temp;
		}
		//the [i][i] is 1 now
		for (j = i + 1; j < n; j++)
		{
			if (!util_isZero(*stor_entry(t, j, i)))
			{
				temp2 = *stor_entry(t, j, i);
				for (k = 0; k < n; k++)
				{
					*stor_entry(t, j, k) -= temp2 * *stor_entry(t, i, k);
					*stor_ansEntry(j, k) -= temp2 * *stor_ansEntry(i, k);
				}
			}
		}
		//now there is only one 1 in colum i
	}
	stor_freeMatrix(t);
	return ans;
}

/*
* X * p2 = p1, p1/p2 = X,Error
*/
Matrix* calc_div_Matrix(Matrix* p1, Matrix* p2){
	Matrix *temp = NULL;

	if (p1 == NULL || p2 == NULL)
	{
		//Error
		return NULL;
	}
	if (!calc_reverse_Matrix(p2))
	{
		//Error
		return NULL;
	}
	if (!stor_createMatrix(temp, ans->m, ans->n))
	{
		//Error
		return NULL;
	}
	stor_assign(temp, ans);
	
	calc_mul_Matrix(p1, temp);
	stor_freeMatrix(temp);
	return ans;
}

void ex_Matrix(Matrix* p,int n){
	if (p == NULL);
	//矩阵不存在
	else if (p->m != p->n);
	//矩阵行列不相同
	else{
		if (stor_type(p) & 1){
			stor_createAns(0, p->m, p->n, 1);
			ex1(p->pd, p->m);
		}
		else{
			stor_createAns(0, p->m, p->n, 0);
			ex0(p->pl, p->m);
		}
	}
}

static void ex1(double** p, int m){
	int i, j, n;
	double** temp, **x;
	n = m - 1;
		temp = (double**)malloc(sizeof(double*)*m);
		x = (double**)malloc(sizeof(double*)*m);
		for (i = 0; i < m; i++){
		temp[i] = (double*)malloc(sizeof(double)*m);
		x[i] = (double*)malloc(sizeof(double)*m);
		}
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++)
			*(x[i] + j) = *(temp[i] + j) = *(p[i] + j);
	while (n > 0){
		if ((n % 2) == 1)
			temp = ex1_mul(temp, x, m);
		x = ex1_mul(x, x, m);
		n /= 2;
	}
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++)
			*(double*)stor_ansEntry(0, i, j) = *(temp[i] + j);
	free(temp);
	free(x);
}

static void ex0(long** p, int m){
	int i, j, n;
	long** temp, **x;
	n = m - 1;
	temp = (long**)malloc(sizeof(long*)*m);
	x = (long**)malloc(sizeof(long*)*m);
	for (i = 0; i < m; i++){
		temp[i] = (long*)malloc(sizeof(long)*m);
		x[i] = (long*)malloc(sizeof(double)*m);
	}
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++)
			*(x[i] + j) = *(temp[i] + j) = *(p[i] + j);
	while (n> 0){
		if ((n % 2) == 1)
			temp = ex0_mul(temp, x, m);
		x = ex0_mul(x, x, m);
		n /= 2;
	}
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++)
			*(long*)stor_ansEntry(0, i, j) = *(temp[i] + j);
	free(temp);
	free(x);
}

static double **ex1_mul(double** p1, double** p2, int m){
	int i, j, k;
	double** temp;
	temp = (double**)malloc(sizeof(double*)*m);
	for (i = 0; i < m; i++)
		temp[i] = (double*)malloc(sizeof(double)*m);
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++){
			*(temp[i] + j) = 0;
			for (k = 0; k < m; k++)
				*(temp[i] + j) += *(p1[i] + k)**(p2[k] + j);
		}
	free(p1);
	return temp;
}

static long **ex0_mul(long** p1, long** p2, int m){
	int i, j, k;
	long** temp;
	temp = (long**)malloc(sizeof(long*)*m);
	for (i = 0; i < m; i++)
		temp[i] = (long*)malloc(sizeof(long)*m);
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++){
			*(temp[i] + j) = 0;
			for (k = 0; k < m; k++)
				*(temp[i] + j) += *(p1[i] + k)**(p2[k] + j);
		}
	free(p1);
	return temp;
}

double sum_Matrix(Matrix* p){
	double sum = 0;
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else if (stor_type(p) & 1){
		for (i = 0; i < p->m; i++)
			for (j = 0; j < p->n; j++)
				sum += *(p->pd[i] + j);
		return sum;
	}
	else{
		for (i = 0; i < p->m; i++)
			for (j = 0; j < p->n; j++)
				sum += *(p->pl[i] + j);
		return sum;
	}
	return sum;
}

double max_Matrix(Matrix* p){
	double max;
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else if (stor_type(p) & 1){
		max = *(p->pd[0]);
		for (i = 0; i < p->m; i++)
			for (j = 0; j < p->n; j++)
				if (*(p->pl[i] + j) > max)
					max = *(p->pl[i] + j);
		return max;
	}
	else{
		max = *(p->pd[0]);
		for (i = 0; i < p->m; i++)
			for (j = 0; j < p->n; j++)
				if (*(p->pl[i] + j) > max)
					max = *(p->pl[i] + j);
		return max;
	}
	return 0;//never hapen maybe
}

double min_Matrix(Matrix* p){
	double min;
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else if (stor_type(p) & 1){
		min = *(p->pd[0]);
		for (i = 0; i < p->m; i++)
			for (j = 0; j < p->n; j++)
				if (*(p->pl[i] + j) < min)
					min = *(p->pl[i] + j);
		return min;
	}
	else{
		min = *(p->pd[0]);
		for (i = 0; i < p->m; i++)
			for (j = 0; j < p->n; j++)
				if (*(p->pl[i] + j) < min)
					min = *(p->pl[i] + j);
		return min;
	}
	return 0;//never happen maybe
}

void round_Matrix(Matrix* p){
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else{
		stor_createAns(0, p->m, p->n, 0);
		if (stor_type(p) & 1)
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(long*)stor_ansEntry(0, i, j) = (long)(*(p->pd[i] + j) + 0.5);
		else
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(long*)stor_ansEntry(0, i, j) = (long)*(p->pd[i] + j);
	}
}

void upper_Matrix(Matrix* p){
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else{
		stor_createAns(0, p->m, p->n, 0);
		if (stor_type(p) & 1)
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(long*)stor_ansEntry(0, i, j) = (long)ceil(*(p->pd[i] + j));
		else
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(long*)stor_ansEntry(0, i, j) = (long)*(p->pd[i] + j);
	}
}

void lower_Matrix(Matrix* p){
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else{
		stor_createAns(0, p->m, p->n, 0);
		if (stor_type(p) & 1)
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(long*)stor_ansEntry(0, i, j) = (long)floor(*(p->pd[i] + j));
		else
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(long*)stor_ansEntry(0, i, j) = *(p->pl[i] + j);
	}
}

void reverse_Matrix(Matrix* p){
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else{
		if (stor_type(p) & 1){
			stor_createAns(0, p->n, p->m, 1);
			for (i = 0; i < p->n; i++)
				for (j = 0; j < p->m; j++)
					*(double*)stor_ansEntry(0, i, j) = *(p->pd[j] + i);
		}
		else{
			stor_createAns(0, p->n, p->m, 0);
			for (i = 0; i < p->n; i++)
				for (j = 0; j < p->m; j++)
					*(double*)stor_ansEntry(0, i, j) = *(p->pl[j] + i);
		}
	}
}

double det_Matrix(Matrix* p){
	int i, j, c, c1;
	double** temp;
	double result = 0, t;
	if (p == NULL){
		//矩阵不存在
		return 0;
	}
	else if (p->m != p->n){
		//行列不相等，行列式不存在
		return 0;
	}
	else{
		temp = (double**)malloc(sizeof(double*)*p->m);
		for (i = 0; i < p->m; i++)
			temp[i] = (double*)malloc(sizeof(double)*p->m*2);
		if (stor_type(p) & 1)
			for (i = 0; i < p->m; i++)
				for (j = 0; j < 2 * p->m; j++)
					*(temp[i] + j) = *(p->pd[i] + j%p->m);
		else
			for (i = 0; i < p->m; i++)
				for (j = 0; j < 2 * p->m; j++)
					*(temp[i] + j) = *(p->pl[i] + j%p->m);
		for (c1 = 0; c1<p->m; c1++)
		{
			i = 0;
			j = c1;
			t = 1;
			for (c = 0; c<p->m; c++)
			{
				t *= *(temp[i] + j);
				i++;
				j++;
			}
			result += t;
		}
		for (c1 = 0; c1<p->m; c1++)
		{
			i = p->m - 1;
			j = c1;
			t = 1;
			for (c = 0; c<p->m; c++)
			{
				t *= *(temp[i] + j);
				i--;
				j++;
			}
			result -= t;
		}
		return result;
	}
}

double norm_Matrix(Matrix* p){
	double result = 0;
	int i, j;
	if (p == NULL){
		//矩阵不存在
		return 0;
	}
	else{
		if (stor_type(p) & 1)
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					result += *(p->pd[i] + j)**(p->pd[i] + j);
		else
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					result += *(p->pd[i] + j)**(p->pl[i] + j);
		return sqrt(result);
	}
}

int rank_Matrix(Matrix *p){
	int i, j;
	double** temp;

	if (p == NULL){
		//矩阵不存在
		return 0;
	}
	else{
		temp = (double**)malloc(sizeof(double*)*p->m);
		for (i = 0; i < p->n; i++)
			temp[i] = (double*)malloc(sizeof(double)*p->n);
		if (stor_type(p) & 1){
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(temp[i] + j) = *(p->pd[i] + j);
		}
		else{
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(temp[i] + j) = *(p->pl[i] + j);
		}
		return rank(temp, p->m, p->n);
	}
}

static void exchange_row(double *a, double *b, int n)
{
	int i;
	double t;
	for (i = 0; i<n; i++)
	{
		t = *(a + i);
		*(a + i) = *(b + i);
		*(b + i) = t;
	}
}

static void mul_row(double *a, double k, int n) {
	int i;
	for (i = 0; i < n; i++)
		*(a + i) *= k;
}

static void add_row(double *a1, double *a2, int k, int n) {
	int i;
	for (i = 0; i < n; i++)
		*(a1 + i) += *(a2 + i)*k;
}

static int rank(double** p,int m,int n){
	int i, t;
	int ri, ci, flag;
	for (ri = ci = 0; ci < n; ci++) {
		flag = 1;
		for (i = ri; i<m; i++)
			if (*(p[i] + ci) != 0) {
				if (i != ri)
					if (flag)
						exchange_row(p[ri] + ci, p[i] + ci, n - ci);
					else {
						t = *(p[i] + ci);
						mul_row(p[i] + ci, *(p[ri] + ci), n - ci);
						add_row(p[ri] + i, p[ri] + ci, -t, n - ci);
					}
					flag = 0;
			}
		if (!flag)
			ri++;
	}
	free(p);
	return ri;
}

double dot_Matrix(Matrix* p1, Matrix* p2){
	int p1_type, p2_type;
	if (p1 == NULL || p2 == NULL){
		//矩阵不存在
		return 0;
	}
	else if (p1->m != p2->m || p1->n != p2->n){
		//矩阵大小不相等
		return 0;
	}
	else{
		p1_type = stor_type(p1);
		p2_type = stor_type(p2);
		if ((p1_type & 1) || (p2_type & 1))
			if ((p1_type & 1) && (p2_type & 1))
				return dot11(p1->pd, p2->pd, p1->m, p1->n);
			else if (p1_type & 1)
				return dot10(p1->pd, p2->pl, p1->m, p1->n);
			else
				return dot01(p1->pl, p2->pd, p1->m, p1->n);
		else 
			return dot00(p1->pl, p2->pl, p1->m, p2->n);
	}
}//不知道具体什么意思

static double dot11(double** p1, double** p2, int m, int n){
	int i, j;
	double res = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			res += *(p1[j] + i)**(p2[j] + i);
	return res;
}

static double dot10(double** p1, long** p2, int m, int n){
	int i, j;
	double res = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			res += *(p1[j] + i)**(p2[j] + i);
	return res;
}

static double dot01(long** p1, double** p2, int m, int n){
	int i, j;
	double res = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			res += *(p1[j] + i)**(p2[j] + i);
	return res;
}

static double dot00(long** p1, long** p2, int m, int n){
	int i, j;
	double res = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			res += *(p1[j] + i)**(p2[j] + i);
	return res;
}

double angle_Matrix(Matrix* p1, Matrix* p2){
	if (p1 == NULL || p2 == NULL){
		//矩阵不存在
		return 0;
	}
	else if (p1->m != 1 || p2->m != 1){
		//存在无法表示为单一矢量的矩阵
		return 0;
	}
	else
		return dot_Matrix(p1, p2) / (norm_Matrix(p1)*norm_Matrix(p2));
}

void rref_Matrix(Matrix* p){
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else{
		stor_createAns(0, p->m, p->n, 1);
		if (stor_type(p) & 1)
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(double*)stor_ansEntry(0, i, j) = *(p->pd[i] + j);
		else
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(double*)stor_ansEntry(0, i, j) = *(p->pl[i] + j);
	}
	rref(p->m);
}

void rref(int m){
	int i, j, k, max_row;
	double temp;
	for (j = 0; j < m; j++){
		max_row = j;
		for (i = j; i < m; i++){
			if (fabs(*(double*)stor_ansEntry(0, i, j)) > fabs(*(double*)stor_ansEntry(0, max_row, j)))
				max_row = i;
			if (max_row != j)
				swap(j, max_row, m);
			for (i = j + 1; i < m; i++){
				temp = *(double*)stor_ansEntry(0, i, j) / *(double*)stor_ansEntry(0, j, j);
				for (k = j; k < m ; k++)
					*(double*)stor_ansEntry(0, i, k) -= *(double*)stor_ansEntry(0, j, k) * temp;
			}
		}
	}
}

static void swap(int m, int max_row, int n)
{
	int k;
	double temp;
	for (k = m; k <= n + 1; k++){
		temp = *(double*)stor_ansEntry(0, m, k);
		*(double*)stor_ansEntry(0, m, k) = *(double*)stor_ansEntry(0, max_row, k);
		*(double*)stor_ansEntry(0, max_row, k) = temp;
	}
}

void feature_vector_Matrix(Matrix* p){
	double **temp;
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else if (p->m != p->n);
	//矩阵行列不相等
	else{
		temp = (double**)malloc(sizeof(double*)*p->m);
		for (i = 0; i < p->m; i++)
			temp[i] = (double*)malloc(sizeof(double)*p->n);
		stor_createAns(0, p->m, p->n, 1);
		if (stor_type(p) & 1)
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(temp[i] + j) = *(p->pd[i] + j);
		else
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(temp[i] + j) = *(p->pl[i] + j);
		feature_vector(temp, p->m);
	}
}

static int feature_vector(double** temp, int n){
	double eps = 0.000001;
	int jt = 100;
	int i, j, p, q, u, w, l;
	double fm, cn, sn, omega, x, y, d;
	l = 1;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (i != j) 
				*(double*)stor_ansEntry(0, i, j) = 0;
			else 
				*(double*)stor_ansEntry(0, i, j) = 0;
	while (1 == 1)
	{
		fm = 0;
		for (i = 1; i < n; i++)
			for (j = 0; j < n; j++)
			{
				d = fabs(*(temp[i]+j));
				if ((i != j) && (d>fm))
					fm = d; p = i; q = j;
			}
		if (fm<eps)  return(1);
		if (l>jt)  return(-1);
		l = l + 1;
		x = -*(temp[p] + q); y = (*(temp[q] + q) - *(temp[p] + p)) / 2;
		omega = x / sqrt(x*x + y*y);
		if (y<0.0) omega = -omega;
		sn = 1.0 + sqrt(1.0 - omega*omega);
		sn = omega / sqrt(2.0*sn);
		cn = sqrt(1.0 - sn*sn);
		fm = *(temp[p] + p);
		*(temp[p] + p) = fm*cn*cn + *(temp[q] + q) * sn*sn + *(temp[p] + q) * omega;
		*(temp[p] + p) = fm*sn*sn + *(temp[q] + q) * cn*cn - *(temp[p] + q) * omega;
		*(temp[p] + q) = 0;
		*(temp[q] + p) = 0;
		for (j = 0; j < n; j++)
			if ((j != p) && (j != q))
			{
				u = p*n + j; w = q*n + j;
				fm = *(temp[p] + q);
				*(temp[p] + q) = fm*cn + *(temp[p] + p) * sn;
				*(temp[p] + p) = -fm*sn + *(temp[p] + p) * cn;
			}
		for (i = 0; i < n; i++)
			if ((i != p) && (i != q))
			{
				u = i*n + p; w = i*n + q;
				fm = *(temp[p] + q);
				*(temp[p] + q) = fm*cn + *(temp[p] + p) * sn;
				*(temp[p] + p) = -fm*sn + *(temp[p] + p) * cn;
			}
		for (i = 0; i < n; i++)
		{
			fm = *(double*)stor_ansEntry(0, i, p);
			*(double*)stor_ansEntry(0, i, p) = fm*cn + *(double*)stor_ansEntry(0, i, q) * sn;
			*(double*)stor_ansEntry(0, i, q) = -fm*sn + *(double*)stor_ansEntry(0, i, q) * cn;
		}
	}
}

void feature_value_Matrix(Matrix* p){
	double **temp;
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else if (p->m != p->n);
	//矩阵行列不相等
	else{
		temp = (double**)malloc(sizeof(double*)*p->m);
		for (i = 0; i < p->m; i++)
			temp[i] = (double*)malloc(sizeof(double)*p->n);
		stor_createAns(0, p->m, 1, 1);
		if (stor_type(p) & 1)
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(temp[i] + j) = *(p->pd[i] + j);
		else
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(temp[i] + j) = *(p->pl[i] + j);
		feature_vector(temp, p->m);
	}
}

static int feature_value(double** temp, int n){
	double eps = 0.000001;
	int jt = 100;
	int i, j, p, q, u, w, l;
	double fm, cn, sn, omega, x, y, d;
	l = 1;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (i != j)
				*(double*)stor_ansEntry(0, i, j) = 0;
			else
				*(double*)stor_ansEntry(0, i, j) = 0;
	while (1 == 1)
	{
		fm = 0;
		for (i = 1; i < n; i++)
			for (j = 0; j < n; j++)
			{
				d = fabs(*(temp[i] + j));
				if ((i != j) && (d>fm))
					fm = d; p = i; q = j;
			}
		if (fm<eps)  return(1);
		if (l>jt)  return(-1);
		l = l + 1;
		x = -*(temp[p] + q); y = (*(temp[q] + q) - *(temp[p] + p)) / 2;
		omega = x / sqrt(x*x + y*y);
		if (y<0.0) omega = -omega;
		sn = 1.0 + sqrt(1.0 - omega*omega);
		sn = omega / sqrt(2.0*sn);
		cn = sqrt(1.0 - sn*sn);
		fm = *(temp[p] + p);
		*(temp[p] + p) = fm*cn*cn + *(temp[q] + q) * sn*sn + *(temp[p] + q) * omega;
		*(temp[p] + p) = fm*sn*sn + *(temp[q] + q) * cn*cn - *(temp[p] + q) * omega;
		*(temp[p] + q) = 0;
		*(temp[q] + p) = 0;
		for (j = 0; j < n; j++)
			if ((j != p) && (j != q))
			{
				u = p*n + j; w = q*n + j;
				fm = *(temp[p] + q);
				*(temp[p] + q) = fm*cn + *(temp[p] + p) * sn;
				*(temp[p] + p) = -fm*sn + *(temp[p] + p) * cn;
			}
		for (i = 0; i < n; i++)
			if ((i != p) && (i != q))
			{
				u = i*n + p; w = i*n + q;
				fm = *(temp[p] + q);
				*(temp[p] + q) = fm*cn + *(temp[p] + p) * sn;
				*(temp[p] + p) = -fm*sn + *(temp[p] + p) * cn;
			}
		for (i = 0; i < n; i++)
			*(double*)stor_ansEntry(0, i, 0) = *(temp[i] + i);
	}
}

void opposite(Matrix* p){
	stor_createAns(0, p->m, p->n, 1);
	if (stor_type(p) & 1)
		opposite1(p->pd, p->m, p->n);
	else
		opposite0(p->pl, p->m, p->n);
}

static void opposite1(double** p, int m, int n){
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			*(double*)stor_ansEntry(0, i, j) = 1.0 / *(p[i] + j);
}

static void opposite0(long** p, int m, int n){
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			*(double*)stor_ansEntry(0, i, j) = 1.0 / *(p[i] + j);
}