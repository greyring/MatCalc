/*
只能根据浮点数的精度来判断矩阵中的元素是不是整数
*/
#include "universe.h"
#include "storage.h"
#include "command.h"
#include "util.h"
#include "io.h"
#include "calculate.h"

FILE *fpin, *fpout;
char filein[256],fileout[256];
int main(void)
{
	//int i,j;
	int row[2] = {1, 3};
	int colum[2] = {1, 3};
	//SubMatrix *p;
	Matrix *t = NULL, *t2 = NULL, *t3= NULL;

	fpin = stdin;
	fpout = stdout;
	srand((unsigned int)time(NULL));
	stor_ini();

	//strcpy(buf,"   _Matrix = [5  : .5 :       9]; \n");
	//strcpy(buf,"  ans   =  [ 3+1   ++  -0 --4  ,  3 --+-3    +-2]\n");
	//util_strcpy(buf, "_MatrixA=[1.25 3,  4.76;5.28   3 1.44\n\n1 8,1\n]\n");
	//util_strcpy(buf, "a = [1,2,3;2,3,4;3,4,5]\n");
	//com_interpret();
	//util_strcpy(buf, "a\n");
	//com_interpret();
	//util_strcpy(buf, "a([1],[2]) = 1\n");
	//com_interpret();
	uniFlag.in = 0;
	uniFlag.out = 0;
	while (1)
	{
		uniFlag.show = 1;
		io_input();
		if (com_interpret()==-1)
		{
			io_output();//Todo不是所有都输出矩阵的
		}
		else
		{
			printf("Error\n");
		}
	}
	//fputc('a',stdout);
	//printf("%s", buf);


	/*
	stor_create("cb",4,4);
	stor_create("cc",1,1);
	stor_create("ca",1,1);
	stor_createMatrix(t, 0, 0);
	stor_createAns(4,4);
	for (i = 0; i<4; i++)
	{
		for (j = 0; j<4; j++)
		{
			*stor_entry(stor_matrix("cb"),i,j) = i+j;
		}
	}
	*stor_entry(stor_matrix("cc"), 0, 0) = 0;
	*stor_entry(stor_matrix("ca"), 0, 0) = 1;
	p = stor_subMatrix(stor_matrix("cb"), 2, 2, row, colum);
	*stor_entry(p->sub, 0, 0) = 5;
	stor_mergeSubMatrix(p);
	stor_assign(ans, stor_matrix("cb"));
	t = stor_mergeColum(stor_matrix("ca"), stor_matrix("cc"));
	t = stor_mergeRow(stor_matrix("ca"), stor_matrix("cc"));
	stor_create("ca", 2, 2);
	t2 = stor_createMatrix(t2, 3, 3);
	*/
	/*
	t = calc_rand(3, 3);
	t2 = calc_zeros(3, 3);
	t = calc_eye(3);
	
	t = calc_eye(3);
	t2 = calc_add(t, t);
	t = NULL;
	t = stor_createMatrix(&t, 3, 3);
	*stor_entry(t, 1, 1) = 2;
	t2 = calc_add(ans, t);
	t2 = calc_sub(t2, t2);
	t2 = calc_sub(ans, t);
	*/
	/*
	stor_createMatrix(&t, 2, 3);
	stor_createMatrix(&t2, 2, 3);
	*stor_entry(t, 0, 0) = 3;
	*stor_entry(t, 0, 1) = 2;
	*stor_entry(t, 0, 2) = 1;
	*stor_entry(t, 1, 0) = 4;
	*stor_entry(t, 1, 1) = 5;
	*stor_entry(t, 1, 2) = 6;
	*stor_entry(t2, 0, 0) = 2;
	*stor_entry(t2, 0, 1) = 2;
	*stor_entry(t2, 0, 2) = 2;
	*stor_entry(t2, 1, 0) = 1;
	*stor_entry(t2, 1, 1) = 2;
	*stor_entry(t2, 1, 2) = 3;
	calc_add(t, t2);
	calc_numMul(ans, -2);
	t3 = calc_mul(t, t2);
	calc_trans(t2);
	calc_mul(ans, t);
	t = ans;
	ans = NULL;
	calc_sum(t);
	calc_max(t);
	calc_min(t);
	calc_rand(2, 2);
	stor_freeMatrix(t);
	t = ans;
	ans = NULL;
	calc_round(t);
	calc_upper(t);
	calc_lower(t);
	stor_freeMatrix(t);
	
	stor_createMatrix(&t, 3, 3);
	*stor_entry(t, 0, 0) = 1;
	*stor_entry(t, 0, 1) = -1;
	*stor_entry(t, 0, 2) = 3;
	*stor_entry(t, 1, 0) = 2;
	*stor_entry(t, 1, 1) = -1;
	*stor_entry(t, 1, 2) = 4;
	*stor_entry(t, 2, 0) = -1;
	*stor_entry(t, 2, 1) = 2;
	*stor_entry(t, 2, 2) = -4;
	calc_inverse(t);
	// -4 2 -1;4 -1 2;3 -1 1
	t2 = ans;
	ans = NULL;
	calc_div(calc_eye(3), t2);
	stor_freeMatrix(t2);
	stor_freeMatrix(t);

	calc_eye(3);
	calc_numMul(ans, 2);
	t = ans;
	ans = NULL;
	calc_ex(t, 6);
	calc_everyEx(t, 6);
	stor_freeMatrix(t);

	stor_createMatrix(&t, 3, 3);
	*stor_entry(t, 0, 0) = 4;
	*stor_entry(t, 0, 1) = 2;
	*stor_entry(t, 0, 2) = 1;
	*stor_entry(t, 1, 0) = 3;
	*stor_entry(t, 1, 1) = 0;
	*stor_entry(t, 1, 2) = 2;
	*stor_entry(t, 2, 0) = 2;
	*stor_entry(t, 2, 1) = -1;
	*stor_entry(t, 2, 2) = 2;
	calc_det(t);
	calc_rank(t);
	*/
    /*
	stor_freeMatrix(t);
	stor_createMatrix(&t, 4, 3);
	*stor_entry(t, 0, 0) = 1;
	*stor_entry(t, 0, 1) = 9;
	*stor_entry(t, 0, 2) = -2;
	*stor_entry(t, 1, 0) = 2;
	*stor_entry(t, 1, 1) = 100;
	*stor_entry(t, 1, 2) = -4;
	*stor_entry(t, 2, 0) = -1;
	*stor_entry(t, 2, 1) = 10;
	*stor_entry(t, 2, 2) = 2;
	*stor_entry(t, 3, 0) = 4;
	*stor_entry(t, 3, 1) = 4;
	*stor_entry(t, 3, 2) = -8;
	calc_rank(t);
	calc_add(calc_zeros(4, 3), t);
	calc_rref(ans);
	stor_print(ans);
	stor_freeMatrix(t);
	*/
	/*
	stor_createMatrix(&t, 1, 4);
	*stor_entry(t, 0, 0) = 2;
	*stor_entry(t, 0, 1) = 1;
	*stor_entry(t, 0, 2) = 3;
	*stor_entry(t, 0, 3) = 2;
	stor_createMatrix(&t2, 1, 4);
	*stor_entry(t2, 0, 0) = 1;
	*stor_entry(t2, 0, 1) = 2;
	*stor_entry(t2, 0, 2) = -2;
	*stor_entry(t2, 0, 3) = 1;
	calc_dot(t, t2);
	calc_norm(t);
	calc_angle(t, t2);
	*/
	/*
	stor_freeMatrix(t);
	stor_freeMatrix(t2);
	stor_createMatrix(&t, 3, 3);
	*stor_entry(t, 0, 0) = 5;
	*stor_entry(t, 0, 1) = -3;
	*stor_entry(t, 0, 2) = 2;
	*stor_entry(t, 1, 0) = 6;
	*stor_entry(t, 1, 1) = -4;
	*stor_entry(t, 1, 2) = 4;
	*stor_entry(t, 2, 0) = 4;
	*stor_entry(t, 2, 1) = -4;
	*stor_entry(t, 2, 2) = 5;
	calc_eig(t);
	stor_print(ans);
	*/
	/*
	stor_createMatrix(&t, 3, 5);
	*stor_entry(t, 0, 0) = 1;
	*stor_entry(t, 0, 1) = 1;
	*stor_entry(t, 0, 2) = -1;
	*stor_entry(t, 0, 3) = -1;
	*stor_entry(t, 0, 4) = 0;
	*stor_entry(t, 1, 0) = 2;
	*stor_entry(t, 1, 1) = -5;
	*stor_entry(t, 1, 2) = 3;
	*stor_entry(t, 1, 3) = 2;
	*stor_entry(t, 1, 4) = 0;
	*stor_entry(t, 2, 0) = 7;
	*stor_entry(t, 2, 1) = -7;
	*stor_entry(t, 2, 2) = 3;
	*stor_entry(t, 2, 3) = 1;
	*stor_entry(t, 2, 4) = 0;
	calc_solve(t);
	stor_print(ans);
	*/
	return 0;
}
