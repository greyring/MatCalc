#include "storage.h"
#include "universe.h"
#include "calculate.h"

static void add11(long double** p1, long double** p2, int m, int n);
static void add10(long double** p1, long** p2, int m, int n);
static void add01(long** p1, long double** p2, int m, int n);
static void add00(long** p1, long** p2, int m, int n);
static void sub11(long double** p1, long double** p2, int m, int n);
static void sub10(long double** p1, long** p2, int m, int n);
static void sub01(long** p1, long double** p2, int m, int n);
static void sub00(long** p1, long** p2, int m, int n);
static void mul11(long double** p1, long double** p2, int m, int n, int l);
static void mul10(long double** p1, long** p2, int m, int n, int l);
static void mul01(long** p1, long double** p2, int m, int n, int l);
static void mul00(long** p1, long** p2, int m, int n, int l);
static int inverse0(long double** p, int m, int n);
static int inverse1(long** p, int m, int n);
static long double **ex1_mul(long double** p1, long double** p2, int m);
static long **ex0_mul(long** p1, long** p2, int m);
static void ex1(long double** p, int m);
static void ex0(long** p, int m);
static void exchange_row(long double *a, long double *b, int n);
static void mul_row(long double *a, long double k, int n);
static void add_row(long double *a1, long double *a2, int k, int n);
static int rank(long double** p, int m, int n);
static long double dot11(long double** p1, long double** p2, int m, int n);
static long double dot10(long double** p1, long** p2, int m, int n);
static long double dot01(long** p1, long double** p2, int m, int n);
static long double dot00(long** p1, long** p2, int m, int n);
static void rref(int m);
static void swap(int m, int max_row, int n);
static int feature_vector(long double** temp, int n);
static int feature_value(long double** temp, int n);
static void opposite1(long double** p, int m, int n);
static void opposite0(long** p, int m, int n);


void rand_Matrix(int m, int n)
{
	int i, j;
	stor_createAns(0, m, n, 1);
	for (i = 0; i<m; i++)
	{
		for (j = 0; j<n; j++)
		{
			*(long double*)stor_ansEntry(0,i,j) = rand()*1.0/RAND_MAX;
		}
	}
}

void zeros_Matrix(int m, int n)
{
	int i, j;
	stor_createAns(0, m, n, 0);
	for (i = 0; i<m; i++)
	{
		for (j = 0; j<n; j++)
		{
			*(long*)stor_ansEntry(0,i,j) = 0;
		}
	}
}

void eye_Matrix(int n)
{
	int i, j;
	stor_createAns(0, n, n, 0);
	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n; j++)
		{
			if (i != j)
				*(long*)stor_ansEntry(0,i,j) = 0;
			else
				*(long*)stor_ansEntry(0,i,j) = 1;
		}
	}
}

void add_Matrix(Matrix* p1, Matrix* p2) {
	int p1_type, p2_type;
	if (p1 == NULL || p2 == NULL);
	//矩阵不存在
	else if (p1->m != p2->m || p1->n != p2->n);
	//行列不相同
	else {
		p1_type = stor_type(p1);
		p2_type = stor_type(p2);
		if ((p1_type & 1) || (p2_type & 1)) {
			stor_createAns(0, p1->m, p1->n, 1);
			if ((p1_type & 1) && (p2_type & 1))
				add11(p1->pd, p2->pd, p1->m, p1->n);
			else if (p1_type & 1)
				add10(p1->pd, p2->pl, p1->m, p1->n);
			else
				add01(p1->pl, p2->pd, p1->m, p1->n);
		}
		else {
			stor_createAns(0, p1->m, p1->n, 0);
			add00(p1->pl, p2->pl, p1->m, p1->n);
		}
	}
}

static void add11(long double** p1, long double** p2,int m,int n) {
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++);
	*(long double*)stor_ansEntry(0, i, j) = *(p1[i] + j) + *(p2[i] + j);
}

static void add10(long double** p1, long** p2, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++);
	*(long double*)stor_ansEntry(0, i, j) = *(p1[i] + j) + *(p2[i] + j);
}

static void add01(long** p1, long double** p2, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++);
	*(long double*)stor_ansEntry(0, i, j) = *(p1[i] + j) + *(p2[i] + j);
}

static void add00(long** p1, long** p2, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++);
	*(long*)stor_ansEntry(0, i, j) = *(p1[i] + j) + *(p2[i] + j);
}

void sub_Matrix(Matrix* p1, Matrix* p2) {
	int p1_type, p2_type;
	if (p1 == NULL || p2 == NULL);
	//矩阵不存在
	else if (p1->m != p2->m || p1->n != p2->n);
	//行列不相同
	else {
		p1_type = stor_type(p1);
		p2_type = stor_type(p2);
		if ((p1_type & 1) || (p2_type & 1)) {
			stor_createAns(0, p1->m, p1->n, 1);
			if ((p1_type & 1) && (p2_type & 1))
				sub11(p1->pd, p2->pd, p1->m, p1->n);
			else if (p1_type & 1)
				sub10(p1->pd, p2->pl, p1->m, p1->n);
			else
				sub01(p1->pl, p2->pd, p1->m, p1->n);
		}
		else {
			stor_createAns(0, p1->m, p1->n, 0);
			sub00(p1->pl, p2->pl, p1->m, p1->n);
		}
	}
}

static void sub11(long double** p1, long double** p2, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++);
	*(long double*)stor_ansEntry(0, i, j) = *(p1[i] + j) - *(p2[i] + j);
}

static void sub10(long double** p1, long** p2, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++);
	*(long double*)stor_ansEntry(0, i, j) = *(p1[i] + j) - *(p2[i] + j);
}

static void sub01(long** p1, long double** p2, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++);
	*(long double*)stor_ansEntry(0, i, j) = *(p1[i] + j) - *(p2[i] + j);
}

static void sub00(long** p1, long** p2, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++);
	*(long*)stor_ansEntry(0, i, j) = *(p1[i] + j) - *(p2[i] + j);
}

void mul_Matrix(Matrix* p1, Matrix* p2) {
	int p1_type, p2_type;
	if (p1 == NULL || p2 == NULL);
	//矩阵不存在
	else if (p1->n != p2->m);
	//p1行不等于p2列
	else {
		p1_type = stor_type(p1);
		p2_type = stor_type(p2);
		if ((p1_type & 1) || (p2_type & 1)) {
			stor_createAns(0, p1->m, p2->n, 1);
			if ((p1_type & 1) && (p2_type & 1))
				mul11(p1->pd, p2->pd, p1->m, p2->n, p1->n);
			else if (p1_type & 1)
				mul10(p1->pd, p2->pl, p1->m, p2->n, p1->n);
			else
				mul01(p1->pl, p2->pd, p1->m, p2->n, p1->n);
		}
		else {
			stor_createAns(0, p1->m, p2->n, 0);
			mul00(p1->pl, p2->pl, p1->m, p2->n, p1->n);
		}
	}
}

static void mul11(long double** p1, long double** p2, int m, int n, int l) {
	int i, j, k;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++) {
			*(long double*)stor_ansEntry(0, i, j) = 0;
			for (k = 0; k < l; k++)
				*(long double*)stor_ansEntry(0, i, j) += *(p1[i] + k)**(p2[k] + j);
		}
}

static void mul10(long double** p1, long** p2, int m, int n, int l) {
	int i, j, k;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++) {
			*(long double*)stor_ansEntry(0, i, j) = 0;
			for (k = 0; k < l; k++)
				*(long double*)stor_ansEntry(0, i, j) += *(p1[i] + k)**(p2[k] + j);
		}
}

static void mul01(long** p1, long double** p2, int m, int n, int l) {
	int i, j, k;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++) {
			*(long double*)stor_ansEntry(0, i, j) = 0;
			for (k = 0; k < l; k++)
				*(long double*)stor_ansEntry(0, i, j) += *(p1[i] + k)**(p2[k] + j);
		}
}

static void mul00(long** p1, long** p2, int m, int n, int l) {
	int i, j, k;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++) {
			*(long*)stor_ansEntry(0, i, j) = 0;
			for (k = 0; k < l; k++)
				*(long*)stor_ansEntry(0, i, j) += *(p1[i] + k)**(p2[k] + j);
		}
}

int inverse_Matrix(Matrix* p){
	if (p == NULL){
		//节点不存在
		return 1;
	}
	else if (p->m != p->n){
		//矩阵行列不相同，不存在逆
		return 1;
	}
	else{
		stor_createAns(0, p->m, p->n, 0);
		if (stor_type(p) & 1)
			return inverse0(p->pd, p->m, p->n);
		else
			return inverse1(p->pl, p->m, p->n);
	}
}

static int inverse0(long double** p, int m, int n){
	int i, j, k;
	long double max, temp;
	long double **t;
	t = (long double**)malloc(sizeof(long double*)*m);
	for (i = 0; i < m; i++)
		t[i] = (long double*)malloc(sizeof(long double)*n);

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			*(t[i] + j) = *(p[i] + j);
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			*(long double*)stor_ansEntry(0, i, j) = (i == j) ? (long double)1 : 0;
	for (i = 0; i < m; i++){
		max = *(t[i] + i);
		k = i;
		for (j = i + 1; j < n; j++)
			if (fabs(*(t[i] + j)) > fabs(max)){
				max = *(t[i] + j);
				k = j;
			}
	}
	if (k != i)
		for (j = 0; j < n; j++){
			temp = *(long double*)stor_ansEntry(0, i, j);
			*(long double*)stor_ansEntry(0, i, j) = *(long double*)stor_ansEntry(0, k, j);
			*(long double*)stor_ansEntry(0, k, j) = temp;
		}
	if (*(t[i] + i) == 0){
		//不是满秩矩阵
		return 1;
	}
	else{
		temp = *(t[i] + i);
		for (j = 0; j < n; j++){
			*(t[i] + j) = *(t[i] + j)*temp;
			*(long double*)stor_ansEntry(0, i, j) = *(long double*)stor_ansEntry(0, i, j) / temp;
		}
		for (j = 0; j < n; j++){
			if (j != i){
				temp = *(t[j] + i);
				for (k = 0; k < n; k++){
					*(t[j] + k) = *(t[j] + k) - *(t[i] + k)*temp;
					*(long double*)stor_ansEntry(0, j, k) = *(long double*)stor_ansEntry(0, j, k) - *(long double*)stor_ansEntry(0, i, k)*temp;
				}
			}
		}
		return 0;
	}
	free(t);
}

static int inverse1(long** p, int m, int n){
	int i, j, k;
	long double max, temp;
	long double **t;
	t = (long double**)malloc(sizeof(long double*)*m);
	for (i = 0; i < m; i++)
		t[i] = (long double*)malloc(sizeof(long double)*n);

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			*(t[i] + j) = *(p[i] + j);
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			*(long double*)stor_ansEntry(0, i, j) = (i == j) ? (long double)1 : 0;
	for (i = 0; i < m; i++){
		max = *(t[i] + i);
		k = i;
		for (j = i + 1; j < n; j++)
			if (fabs(*(t[i] + j)) > fabs(max)){
				max = *(t[i] + j);
				k = j;
			}
	}
	if (k != i)
		for (j = 0; j < n; j++){
			temp = *(long double*)stor_ansEntry(0, i, j);
			*(long double*)stor_ansEntry(0, i, j) = *(long double*)stor_ansEntry(0, k, j);
			*(long double*)stor_ansEntry(0, k, j) = temp;
		}
	if (*(t[i] + i) == 0){
		//不是满秩矩阵
		return 1;
	}
	else{
		temp = *(t[i] + i);
		for (j = 0; j < n; j++){
			*(t[i] + j) = *(t[i] + j)*temp;
			*(long double*)stor_ansEntry(0, i, j) = *(long double*)stor_ansEntry(0, i, j) / temp;
		}
		for (j = 0; j < n; j++){
			if (j != i){
				temp = *(t[j] + i);
				for (k = 0; k < n; k++){
					*(t[j] + k) = *(t[j] + k) - *(t[i] + k)*temp;
					*(long double*)stor_ansEntry(0, j, k) = *(long double*)stor_ansEntry(0, j, k) - *(long double*)stor_ansEntry(0, i, k)*temp;
				}
			}
		}
		return 0;
	}
	free(t);
}

void div_Matrix(Matrix* p1, Matrix* p2){
	Matrix *temp;
	int i, j;
	if (p1 == NULL || p2 == NULL);
	//矩阵不存在
	else{
		if (inverse_Matrix(p2) == 1);
			//不存在p2的逆矩阵
		else{
			temp = (Matrix *)malloc(sizeof(Matrix));
			temp->pd = (long double**)malloc(sizeof(long double*)*p1->m);
			for (i = 0; i < p1->m; i++)
				temp->pd[i] = (long double*)malloc(sizeof(long double)*p1->n);
			for (i = 0; i < p1->m; i++)
				for (j = 0; j < p1->n; j++)
					*(temp->pd[i] + j) = *(long double*)stor_ansEntry(0, i, j);
			stor_createAns(0, p1->m, p1->n, 0);
			if (stor_type(p1) & 1)
				mul11(p1->pd, temp->pd, p1->m, p1->n, p1->n);
			else
				mul01(p1->pl, temp->pd, p1->m, p1->n, p1->n);
			free(temp->pd);
			free(temp);
		}
	}
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

static void ex1(long double** p, int m){
	int i, j, n;
	long double** temp, **x;
	n = m - 1;
		temp = (long double**)malloc(sizeof(long double*)*m);
		x = (long double**)malloc(sizeof(long double*)*m);
		for (i = 0; i < m; i++){
		temp[i] = (long double*)malloc(sizeof(long double)*m);
		x[i] = (long double*)malloc(sizeof(long double)*m);
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
			*(long double*)stor_ansEntry(0, i, j) = *(temp[i] + j);
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
		x[i] = (long*)malloc(sizeof(long double)*m);
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

static long double **ex1_mul(long double** p1, long double** p2, int m){
	int i, j, k;
	long double** temp;
	temp = (long double**)malloc(sizeof(long double*)*m);
	for (i = 0; i < m; i++)
		temp[i] = (long double*)malloc(sizeof(long double)*m);
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

long double sum_Matrix(Matrix* p){
	long double sum = 0;
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

long double max_Matrix(Matrix* p){
	long double max;
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

long double min_Matrix(Matrix* p){
	long double min;
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
					*(long double*)stor_ansEntry(0, i, j) = *(p->pd[j] + i);
		}
		else{
			stor_createAns(0, p->n, p->m, 0);
			for (i = 0; i < p->n; i++)
				for (j = 0; j < p->m; j++)
					*(long double*)stor_ansEntry(0, i, j) = *(p->pl[j] + i);
		}
	}
}

long double det_Matrix(Matrix* p){
	int i, j, c, c1;
	long double** temp;
	long double result = 0, t;
	if (p == NULL){
		//矩阵不存在
		return 0;
	}
	else if (p->m != p->n){
		//行列不相等，行列式不存在
		return 0;
	}
	else{
		temp = (long double**)malloc(sizeof(long double*)*p->m);
		for (i = 0; i < p->m; i++)
			temp[i] = (long double*)malloc(sizeof(long double)*p->m*2);
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

long double norm_Matrix(Matrix* p){
	long double result = 0;
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
	long double** temp;

	if (p == NULL){
		//矩阵不存在
		return 0;
	}
	else{
		temp = (long double**)malloc(sizeof(long double*)*p->m);
		for (i = 0; i < p->n; i++)
			temp[i] = (long double*)malloc(sizeof(long double)*p->n);
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

static void exchange_row(long double *a, long double *b, int n)
{
	int i;
	long double t;
	for (i = 0; i<n; i++)
	{
		t = *(a + i);
		*(a + i) = *(b + i);
		*(b + i) = t;
	}
}

static void mul_row(long double *a, long double k, int n) {
	int i;
	for (i = 0; i < n; i++)
		*(a + i) *= k;
}

static void add_row(long double *a1, long double *a2, int k, int n) {
	int i;
	for (i = 0; i < n; i++)
		*(a1 + i) += *(a2 + i)*k;
}

static int rank(long double** p,int m,int n){
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

long double dot_Matrix(Matrix* p1, Matrix* p2){
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

static long double dot11(long double** p1, long double** p2, int m, int n){
	int i, j;
	long double res = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			res += *(p1[j] + i)**(p2[j] + i);
	return res;
}

static long double dot10(long double** p1, long** p2, int m, int n){
	int i, j;
	long double res = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			res += *(p1[j] + i)**(p2[j] + i);
	return res;
}

static long double dot01(long** p1, long double** p2, int m, int n){
	int i, j;
	long double res = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			res += *(p1[j] + i)**(p2[j] + i);
	return res;
}

static long double dot00(long** p1, long** p2, int m, int n){
	int i, j;
	long double res = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			res += *(p1[j] + i)**(p2[j] + i);
	return res;
}

long double angle_Matrix(Matrix* p1, Matrix* p2){
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
					*(long double*)stor_ansEntry(0, i, j) = *(p->pd[i] + j);
		else
			for (i = 0; i < p->m; i++)
				for (j = 0; j < p->n; j++)
					*(long double*)stor_ansEntry(0, i, j) = *(p->pl[i] + j);
	}
	rref(p->m);
}

void rref(int m){
	int i, j, k, max_row;
	long double temp;
	for (j = 0; j < m; j++){
		max_row = j;
		for (i = j; i < m; i++){
			if (fabs(*(long double*)stor_ansEntry(0, i, j)) > fabs(*(long double*)stor_ansEntry(0, max_row, j)))
				max_row = i;
			if (max_row != j)
				swap(j, max_row, m);
			for (i = j + 1; i < m; i++){
				temp = *(long double*)stor_ansEntry(0, i, j) / *(long double*)stor_ansEntry(0, j, j);
				for (k = j; k < m ; k++)
					*(long double*)stor_ansEntry(0, i, k) -= *(long double*)stor_ansEntry(0, j, k) * temp;
			}
		}
	}
}

static void swap(int m, int max_row, int n)
{
	int k;
	long double temp;
	for (k = m; k <= n + 1; k++){
		temp = *(long double*)stor_ansEntry(0, m, k);
		*(long double*)stor_ansEntry(0, m, k) = *(long double*)stor_ansEntry(0, max_row, k);
		*(long double*)stor_ansEntry(0, max_row, k) = temp;
	}
}

void feature_vector_Matrix(Matrix* p){
	long double **temp;
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else if (p->m != p->n);
	//矩阵行列不相等
	else{
		temp = (long double**)malloc(sizeof(long double*)*p->m);
		for (i = 0; i < p->m; i++)
			temp[i] = (long double*)malloc(sizeof(long double)*p->n);
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

static int feature_vector(long double** temp, int n){
	long double eps = 0.000001;
	int jt = 100;
	int i, j, p, q, u, w, l;
	long double fm, cn, sn, omega, x, y, d;
	l = 1;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (i != j) 
				*(long double*)stor_ansEntry(0, i, j) = 0;
			else 
				*(long double*)stor_ansEntry(0, i, j) = 0;
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
			fm = *(long double*)stor_ansEntry(0, i, p);
			*(long double*)stor_ansEntry(0, i, p) = fm*cn + *(long double*)stor_ansEntry(0, i, q) * sn;
			*(long double*)stor_ansEntry(0, i, q) = -fm*sn + *(long double*)stor_ansEntry(0, i, q) * cn;
		}
	}
}

void feature_value_Matrix(Matrix* p){
	long double **temp;
	int i, j;
	if (p == NULL);
	//矩阵不存在
	else if (p->m != p->n);
	//矩阵行列不相等
	else{
		temp = (long double**)malloc(sizeof(long double*)*p->m);
		for (i = 0; i < p->m; i++)
			temp[i] = (long double*)malloc(sizeof(long double)*p->n);
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

static int feature_value(long double** temp, int n){
	long double eps = 0.000001;
	int jt = 100;
	int i, j, p, q, u, w, l;
	long double fm, cn, sn, omega, x, y, d;
	l = 1;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (i != j)
				*(long double*)stor_ansEntry(0, i, j) = 0;
			else
				*(long double*)stor_ansEntry(0, i, j) = 0;
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
			*(long double*)stor_ansEntry(0, i, 0) = *(temp[i] + i);
	}
}

void opposite(Matrix* p){
	stor_createAns(0, p->m, p->n, 1);
	if (stor_type(p) & 1)
		opposite1(p->pd, p->m, p->n);
	else
		opposite0(p->pl, p->m, p->n);
}

static void opposite1(long double** p, int m, int n){
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			*(long double*)stor_ansEntry(0, i, j) = 1.0 / *(p[i] + j);
}

static void opposite0(long** p, int m, int n){
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			*(long double*)stor_ansEntry(0, i, j) = 1.0 / *(p[i] + j);
}