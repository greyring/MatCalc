#include "util.h"

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