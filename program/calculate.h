#pragma once
#define _CALCULATE_H_

#include "universe.h"
#include "util.h"
#include "storage.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void rand_Matrix(int m, int n);
void zeros_Matrix(int m, int n);
void eye_Matrix(int n);
void add_Matrix(Matrix* p1, Matrix* p2);
void sub_Matrix(Matrix* p1, Matrix* p2);
void mul_Matrix(Matrix* p1, Matrix* p2);
int inverse_Matrix(Matrix* p);
void div_Matrix(Matrix* p1, Matrix* p2);
void ex_Matrix(Matrix* p, int n);
long double sum_Matrix(Matrix* p);
long double max_Matrix(Matrix* p);
long double min_Matrix(Matrix* p);
void round_Matrix(Matrix* p);
void upper_Matrix(Matrix* p);
void lower_Matrix(Matrix* p);
void reverse_Matrix(Matrix* p);
long double det_Matrix(Matrix* p);
long double norm_Matrix(Matrix* p);
int rank_Matrix(Matrix* p);
long double dot_Matrix(Matrix* p1, Matrix* p2);
long double angle_Matrix(Matrix* p1, Matrix* p2);
void rref_Matrix(Matrix* p);
void feature_value_Matrix(Matrix* p);
void opposite(Matrix* p);
