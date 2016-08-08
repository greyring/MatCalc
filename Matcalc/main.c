#include "universe.h"
#include "storage.h"
#include "command.h"
#include "util.h"

FILE *fpin, *fpout;
int uni_flag;
char filein[256],fileout[256];
int main(void)
{
	int i,j;
	int row[2] = {1, 3};
	int colum[2] = {1, 3};

	fpin = stdin;
	fpout = stdout;
	srand((unsigned int)time(NULL));
	stor_ini();

	//strcpy(buf,"   _Matrix = [5  : .5 :       9]; \n");
	//strcpy(buf,"  ans   =  [ 3+1   ++  -0 --4  ,  3 --+-3    +-2]\n");
	strcpy(buf, "_MatrixA=[1.25 3,  4.76;5.28   3 1.44\n\n1 8,1\n]\n");
	com_interpret();
	//fputc('a',stdout);
	//printf("%s", buf);


	/*
	stor_create("cb",4,4,1);
	stor_create("cc",3,3,0);
	stor_create("ca",2,2,1);
	stor_createAns(1,2,2,0);
	for (i = 0; i<4; i++)
	{
		for (j = 0; j<4; j++)
		{
			*(long double *)stor_entry(stor_matrix("cb"),i,j) = i+j;
		}
	}
	stor_subMatrix(stor_matrix("cb"), 2, 2, row, colum);
	*(long double *)stor_ansEntry(1, 0, 0) = 1;
	stor_create("cb",4,4,0);
	stor_assignSubMatrix(stor_matrix("cb"),stor_ans(1),2,2,row, colum);*/


	return 0;
}
