#include "universe.h"
#include "calculate.h"
#include "storage.h"
#include "command.h"
#include "util.h"

FILE *fpin, *fpout;
int uni_flag;
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
	//strcpy(buf, "_MatrixA=[1.25 3,  4.76;5.28   3 1.44\n\n1 8,1\n]\n");
	//com_interpret();
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
	*/
	/*
	t = calc_eye(3);
	t2 = calc_add(t, t);
	t = NULL;
	t = stor_createMatrix(t, 3, 3);
	*stor_entry(t, 1, 1) = 2;
	t2 = calc_add(ans, t);
	t2 = calc_sub(t2, t2);
	t2 = calc_sub(ans, t);
	*/

	return 0;
}
