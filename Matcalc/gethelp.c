#include "universe.h"

/*
*Return help message, Error
*/
char* gethelp(char* cmd){
	if (strcmp(cmd, ":") == 0)
		return "ʾ����MatName = [N:x:M]\n��ʾ����һ����������Ԫ������Ϊ�� N, N+x, N+2x, ��, M������ֻ��[ N:M ]����ָ�� x ��ʾ x = 1";
	if (strcmp(cmd, "rank") == 0)
		return "ʾ����MatName = rand(N,M)\n����һ����Ϊ�� MatName���� N*M ��������󣬾�����ÿ��Ԫ��Ϊһ��[0, 1]�������������ָ�� M ���ʾ M = N�������ӡ� MatName = �������ans ��������������";
	if (strcmp(cmd, "zeros") == 0)
		return "ʾ����MatName = zeros(N, M)\n����һ����Ϊ�� MatName���� N*N �����������ָ�� M ���ʾ M=N�������ӡ� Matname = ������� ans ������������";
	if (strcmp(cmd, "eye") == 0)
		return "ʾ����MatName = eye(N)\n����һ����Ϊ�� MatName���� N*N �ĵ�λ���������ӡ� Matname = ������� ans ������������";
	if (strcmp(cmd, "ans") == 0)
		return "ʾ����MatName = ans\n����һ����Ϊ�� MatName���ҵ��� ans �ľ���";
	if (strcmp(cmd, "Vector") == 0)
		return "ʾ����MatA(VectorA, VectorB)\n VectorA = [1, 5], VectorB = [2, 4]��ʾ�� MatA ��ȡ���� 1 �С��� 5 �еĵ� 2 �С��� 4 �й��ɵ��Ӿ��󡣿ɽ��Ӿ���ֵ����������Ҳ�ɽ���������ֵ���Ӿ������ı� MatA��";
	if (strcmp(cmd, "sum") == 0)
		return "ʾ����sum(MatA)\n�Ծ�������Ԫ�����";
	if (strcmp(cmd, "max") == 0)
		return "ʾ����max(MatA)\n��������е����ֵ";
	if (strcmp(cmd, "min") == 0)
		return "ʾ����min(MatA)\n��������е���Сֵ";
	if (strcmp(cmd, "round") == 0)
		return "ʾ����round(MatName)\n�Ծ���Ԫ�ؽ�����������";
	if (strcmp(cmd, "upper") == 0)
		return "ʾ����upper(MatName)\n�Ծ���Ԫ�ؽ�������ȡ��";
	if (strcmp(cmd, "lower") == 0)
		return "ʾ����lower(MatName)\n�Ծ���Ԫ�ؽ�������ȡ��";
	if (strcmp(cmd, "^") == 0)
		return "ʾ����MatName^N\n��������n����";
	if (strcmp(cmd, "'") == 0)
		return "ʾ����MatName'\n��������ת��";
	if (strcmp(cmd, "det") == 0)
		return "ʾ����det(MatName)\n������������ʽ";
	if (strcmp(cmd, "rank") == 0)
		return "ʾ����rank(MatName)\n����������";
	if (strcmp(cmd, "rref") == 0)
		return "ʾ����rref(MatName)\n�����������ʽ";
	if (strcmp(cmd, "reverse") == 0)
		return "ʾ����reverse(MatA)\n����������";
	if (strcmp(cmd, "dot") == 0)
		return "ʾ����dot(MatA ,MatB)\n�������ļн�";
	if (strcmp(cmd, "norm") == 0)
		return "ʾ����norm(MatA)\n��������ģ";
	if (strcmp(cmd, "angle") == 0)
		return "ʾ����angle(MatA ,MatB)\n�������ļн�";
	if (strcmp(cmd, "eig") == 0)
		return "ʾ����eig(MatA)\n[X D] = eig(MatA)\n��һ��ָ���ʾ�� MatA ������ֵ���ڶ���ָ���ʾ X Ϊ MatA ���������������ɵľ��� D Ϊ����ֵ���ɵĶԽǾ���";
	if (strcmp(cmd, "solve") == 0)
		return "ʾ����solve(MatA)\nMatAΪ�������Ӧ��������󣬽��Ϊ���ɸ������޹ص���������ɵľ���";
	return NULL;
}
