#pragma once

/*
*Create a random Matrix, each entry is double between 0 to 1, will release ans, Error return NULL
*/
Matrix* calc_rand(int m, int n);

/*
*Create a zero Matrix, will release ans, Error return NULL
*/
Matrix* calc_zeros(int m, int n);

/*
*Create a eye Matrix, will release ans, Error return NULL
*/
Matrix* calc_eye(int n);

/*
*Add two matrixs, Error
*/
Matrix* calc_add(Matrix* p1, Matrix* p2);

/*
*Sub two matrixs, Error
*/
Matrix* calc_sub(Matrix* p1, Matrix* p2);

/*
*Mul two matrixs, Error
*/
Matrix* calc_mul(Matrix* p1, Matrix* p2);

/*
*the reverse of matrix, Error
*Todo test
*/
Matrix* calc_inverse(Matrix* p);

/*
* X * p2 = p1, p1/p2 = X, Error
*/
Matrix* calc_div(Matrix* p1, Matrix* p2);

/*
* Error
*/
Matrix* calc_ex(Matrix* p, int ex);

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_sum(Matrix* p);

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_max(Matrix* p);

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_min(Matrix* p);

/*
*Error
*/
Matrix* calc_round(Matrix* p);

/*
*Error
*/
Matrix* calc_upper(Matrix* p);

/*
*Error
*/
Matrix* calc_lower(Matrix* p);

/*
* M^T, Error
*/
Matrix* calc_trans(Matrix* p);

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_det(Matrix* p);

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_norm(Matrix* p);

/*
* return value(ans only 1*1) should be treated as int, Error
*/
Matrix* calc_rank(Matrix *p);

/*
* return value(ans only 1*1) should be treated as double, Error
*/
Matrix* calc_dot(Matrix* p1, Matrix* p2);

/*
* return value(ans only 1*1) should be treated as angel(0~pi), Error
*/
Matrix* calc_angle(Matrix* p1, Matrix* p2);

/*
* 行最简式，Error
*/
Matrix* calc_rref(Matrix* p);

/*
* a*M, Error
*/
Matrix* calc_numMul(Matrix* p, double mul);

/*
* M.^ex, Error
*/
Matrix* calc_everyEx(Matrix *p, int ex);

/*
* the eigvalue, 有可能在数学上出问题（虚数） Error
*/
Matrix* calc_eig(Matrix* p);

Matrix* calc_solve(Matrix *p);
