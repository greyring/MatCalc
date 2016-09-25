#include "universe.h"

/*
*Return help message, Error
*/
char* gethelp(char* cmd){
	if (strcmp(cmd, ":") == 0)
		return "示例：MatName = [N:x:M]\n表示产生一个行向量，元素依次为“ N, N+x, N+2x, …, M”，若只有[ N:M ]而不指定 x 表示 x = 1";
	if (strcmp(cmd, "rank") == 0)
		return "示例：MatName = rand(N,M)\n定义一个名为“ MatName”的 N*M 的随机矩阵，矩阵中每个元素为一个[0, 1]的随机数。若不指定 M 则表示 M = N。若不加“ MatName = ”则仅有ans 等于这个随机矩阵。";
	if (strcmp(cmd, "zeros") == 0)
		return "示例：MatName = zeros(N, M)\n定义一个名为“ MatName”的 N*N 的零矩阵。若不指定 M 则表示 M=N。若不加“ Matname = ”则仅有 ans 等于这个零矩阵。";
	if (strcmp(cmd, "eye") == 0)
		return "示例：MatName = eye(N)\n定义一个名为“ MatName”的 N*N 的单位矩阵，若不加“ Matname = ”则仅有 ans 等于这个零矩阵。";
	if (strcmp(cmd, "ans") == 0)
		return "示例：MatName = ans\n定义一个名为“ MatName”且等于 ans 的矩阵";
	if (strcmp(cmd, "Vector") == 0)
		return "示例：MatA(VectorA, VectorB)\n VectorA = [1, 5], VectorB = [2, 4]表示从 MatA 中取出第 1 行、第 5 行的第 2 列、第 4 列构成的子矩阵。可将子矩阵赋值给其它矩阵，也可将其它矩阵赋值给子矩阵来改变 MatA。";
	if (strcmp(cmd, "sum") == 0)
		return "示例：sum(MatA)\n对矩阵所有元素求和";
	if (strcmp(cmd, "max") == 0)
		return "示例：max(MatA)\n求出矩阵中的最大值";
	if (strcmp(cmd, "min") == 0)
		return "示例：min(MatA)\n求出矩阵中的最小值";
	if (strcmp(cmd, "round") == 0)
		return "示例：round(MatName)\n对矩阵元素进行四舍五入";
	if (strcmp(cmd, "upper") == 0)
		return "示例：upper(MatName)\n对矩阵元素进行向上取整";
	if (strcmp(cmd, "lower") == 0)
		return "示例：lower(MatName)\n对矩阵元素进行向下取整";
	if (strcmp(cmd, "^") == 0)
		return "示例：MatName^N\n计算矩阵的n次幂";
	if (strcmp(cmd, "'") == 0)
		return "示例：MatName'\n计算矩阵的转置";
	if (strcmp(cmd, "det") == 0)
		return "示例：det(MatName)\n计算矩阵的行列式";
	if (strcmp(cmd, "rank") == 0)
		return "示例：rank(MatName)\n计算矩阵的秩";
	if (strcmp(cmd, "rref") == 0)
		return "示例：rref(MatName)\n计算矩阵的最简式";
	if (strcmp(cmd, "reverse") == 0)
		return "示例：reverse(MatA)\n计算矩阵的逆";
	if (strcmp(cmd, "dot") == 0)
		return "示例：dot(MatA ,MatB)\n计算矩阵的夹角";
	if (strcmp(cmd, "norm") == 0)
		return "示例：norm(MatA)\n计算矩阵的模";
	if (strcmp(cmd, "angle") == 0)
		return "示例：angle(MatA ,MatB)\n计算矩阵的夹角";
	if (strcmp(cmd, "eig") == 0)
		return "示例：eig(MatA)\n[X D] = eig(MatA)\n第一条指令表示求 MatA 的特征值，第二条指令表示 X 为 MatA 所有特征向量构成的矩阵， D 为特征值构成的对角矩阵。";
	if (strcmp(cmd, "solve") == 0)
		return "示例：solve(MatA)\nMatA为方程组对应的增广矩阵，结果为若干个线性无关的列向量组成的矩阵";
	return NULL;
}
