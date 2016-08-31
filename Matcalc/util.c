#include "util.h"
#include "universe.h"

static void swap(int *a, int l, int r)
{
	int temp = a[l];
	a[l] = a[r];
	a[r] = temp;
}
static void insertSort(int *a, int l, int r)
{
	int temp;
	int i,j;
	for (i = l+1; i<=r; i++)
	{
		temp = a[i];
		j = i;
		while(a[j-1]>temp && j>l)
		{
			a[j] = a[j-1];
			j--;
		}
		a[j] = temp;
	}
}

/*
*l and r are scripts, NoError
*/
void util_sort(int *a, int l, int r)
{
	int mid = (l + r)/2;
	int i,j;

	if (r - l < 20)
	{
		insertSort(a, l, r);
		return ;
	}
	if (a[l]>a[r])
	{
		swap(a, l, r);
	}
	if (a[mid]<a[l])
	{
		swap(a, mid, l);
	}
	else if (a[mid]>a[r])
	{
		swap(a, mid, r);
	}
    swap(a, mid, r-1);
	i = l+1;
	j = r-2;
	while (i <= j)
	{
		while(a[i]<a[r-1]) i++;
		while(a[j]>a[r-1]) j--;
		if (i<j) swap(a, i, j);
	}
	swap(a, i, r-1);
	util_sort(a, l, i-1);
	util_sort(a, i+1, r);
	return;
}

/*
*isZero, 精度为float，NoError
*/
int util_isZero(double d)
{
	if (fabs(d) < FLT_EPSILON)
	{
		return 1;
	}
	return 0;
}

/*
*isLong, first round d, 精度为float的, NoError
*/
int util_isLong(double d)
{
	if (fabs(floor(d+0.5) - d) < FLT_EPSILON)
	{
		return 1;
	}
	return 0;
}

/*
*swap, NoError
*/
void util_swap(double *d1, double *d2)
{
	double temp;
	temp = *d1;
	*d1 = *d2;
	*d2 = temp;
}

/*
*may overflow,NoError
*/
void util_strcpy(char *dest, char *sour)
{
	memcpy(dest, sour, sizeof(char)*strlen(sour) + 1);
}

/*
* ex can be negative, NoError
*/
double util_ex(double db, int ex)
{
	double result;
	if (ex < 0)
	{
		db = (double)1.0 / db;
		ex = -ex;
	}
	result = 1;
	while (ex)
	{
		if (ex % 2)
		{
			result *= db;
		}
		db *= db;
		ex /= 2;
	}
	return result;
}

