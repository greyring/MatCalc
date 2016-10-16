#include "io.h"
#include "util.h"
#include "universe.h"
#include "storage.h"
#include "command.h"

static int outMatrix(void)
{
	int i, j;
	
	if (!com_result.value.m)
	{
		//Error
		return 1;
	}
	printf("%s", "ans = [\n");
	for (i = 0; i < com_result.value.m->m; i++)
	{
		for (j = 0; j < com_result.value.m->n; j++)
		{
			if (util_isLong(*stor_entry(com_result.value.m, i, j)))
			{
				printf("%ld ", (long)(*stor_entry(com_result.value.m, i, j) + 0.5));
			}
			else
				printf("%.2lf ", *stor_entry(com_result.value.m, i, j));
		}
		printf("\n");
	}
	printf("]\n");
	return 0;
}

static int outSubMatrix(void)
{
	int i, j;
	if (!com_result.value.sm || !com_result.value.sm->sub)
	{
		//Error
		return 1;
	}
	if (com_result.value.sm->sub->m == 1 && com_result.value.sm->sub->n == 1)
	{
		if (util_isLong(*stor_entry(com_result.value.sm->sub, 0, 0)))
		{
			printf("ans = %ld\n", (long)(*stor_entry(com_result.value.sm->sub, 0, 0) + 0.5));
		}
		else
		{
			printf("ans = %.2lf\n", *stor_entry(com_result.value.sm->sub, 0, 0));
		}
	}
	else
	{
		printf("%s", "ans = [\n");
		for (i = 0; i < com_result.value.m->m; i++)
		{
			for (j = 0; j < com_result.value.m->n; j++)
			{
				if (util_isLong(*stor_entry(com_result.value.sm->sub, i, j)))
				{
					printf("%ld ", (long)(*stor_entry(com_result.value.sm->sub, i, j) + 0.5));
				}
				else
					printf("%.2lf ", *stor_entry(com_result.value.sm->sub, i, j));
			}
			printf("\n");
		}
		printf("]\n");
	}
	return 0;
}

static int outLong(void)
{
	printf("ans = %ld\n", com_result.value.l);
	return 0;
}

static int outDouble(void)
{
	printf("ans = %.2lf\n", com_result.value.d);
	return 0;
}

void io_output(void)
{
	if (uniFlag.show)
	{
		switch (com_result.label)
		{
		case 47:case 38:
				if (outMatrix())
				{
					//Error
				}
				break;
			case 44:
				if (outSubMatrix())
				{
					//Error
				}
				break;
			case 39:
				if (outLong())
				{
					//Error
				}
				break;
			case 40:
				if (outDouble())
				{
					//Error
				}
				break;
			default:
				//Error
				break;
		}
	}
}

/*
*只负责一条语句的输入
*/
void io_input(void)
{
	int i = 0;
	char ch;

	while (1)
	{
		if (i != 0)
		{
			switch (com_inputNotEnd())
			{
				case -1:
				{
					//Error
					return;
				}
				case 0:
					return;
				case 1:
					break;
				default:
					break;
			}
		}
		while ((ch = getchar()) != '\n')
		{
			buf[i] = ch;
			i++;
		}
		buf[i] = ch;
		i++;
		buf[i] = 0;
	}
}
