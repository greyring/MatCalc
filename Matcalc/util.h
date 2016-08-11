#pragma once

/*
*l and r are scripts,NoError
*/
void util_sort(int *a, int l, int r);

/*
*isZero, NoError
*/
int util_isZero(double d);

/*
*isLong, NoError
*/
int util_isLong(double d);

/*
*swap, NoError
*/
void util_swap(double *d1, double *d2);

/*
*may overflow,NoError
*/
void util_strcpy(char *dest, char *sour);

/*
* ex can be negative, NoError
*/
double util_ex(double db, int ex);
