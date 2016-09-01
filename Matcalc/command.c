#include "universe.h"
#include "command.h"
#include "storage.h"
#include "calculate.h"
#include "util.h"

static char buf[512];
static letter buf1[512];
static word buf2[256];
static word stack1[256];//存放操作数
static word stack2[256];//存放指令
static int sp1 = -1,sp2 = -1;//指向栈顶
static int flag[256] = {0};//用于函数调用弹出，以及：的处理，如果某一层被标注，则该层弹出后要弹出下一层
static int flagn = 0;//用于标示现在压入的东西之前是不是数字，用于-的处理,flagn只在：;,[=(%/*^以及.^后变成0，否则为1,正负号的时候直接判断一下，下一个字符除（【外都要弹出

/*
 *判断是否为符号
 */
static int isPunctuation(char ch)
{
	if (ch == '('||ch == ')'||ch == '%'
		||ch == '^'||ch == '*'||ch == '+'
		||ch == '-'||ch == '['||ch == ']'
		||ch == ','||ch == '.'||ch == '\''
		||ch == ';'||ch == ':'||ch == '/'
		||ch == '=')
		return 1;
	return 0;
}

/*
 *根据label判断是不是矩阵
 */
static int isMatrix(int label)
{
	if (label != 37 && label != 38 &&  label !=44 && label != 47)//Todo 37实际已消失
	{
		return 0;
	}
	else 
	{
		return 1;
	}
}

/*
*stack1[k]判断是不是数,是整数或浮点数返回1
*/
static int isNum(int k)
{
	if (stack1[k].label == 39 || stack1[k].label == 40)
	{
		return 1;
	}
	
	return 0;
}

static int isPInteger(word w)
{
	if (w.label != 39 && w.label != 44)
	{
		return 0;
	}
	if (w.label == 39)//整数
	{
		if (w.value.l <= 0)
			return 0;
		else
			return 1;
	}
	if (w.label == 44)//子矩阵
	{
		if (w.value.sm->sub->m == 1&& w.value.sm->sub->n == 1)
		{
			if (w.value.sm->sub->pd != NULL && w.value.sm->sub->pd[0][0]>0
				&& util_isLong(w.value.sm->sub->pd[0][0]))//Todo pd为NULL的判断
			{
				return 1;
			}
		}
		return 0;
	}
	return 0;//never happen
}

/*
 *比较buf1从下标k开始的字符串的前几位是否和s2忽略大小写相同
 *相同返回0，不相同返回第一个不相同的绝对下标从1开始，在buf中为k
 */
static int cmp(int k, char *s2)
{
	int j;
	for (j = 0; (unsigned)j < strlen(s2); j++)
	{
		if (isupper(buf1[j+k].ch))
		{
			if (buf1[j+k].ch-'A'+'a' != s2[j])
			{
				return j+1;
			}
		}
		else
		{
			if (buf1[j+k].ch != s2[j])
			{
				return j+1;
			}
		}
	}
	if (isalnum(buf1[j+k].ch)||buf1[j+k].ch == '_')//如果是个标识符
		return j+1;
	return 0;
}

/*
 *根据buf2中第k个word获得相应的matrix指针
 */
static Matrix* getMatrix(int k)
{
	char str[256];
	int i;
	for (i = 0; i<buf2[k].length; i++)
	{
		str[i] = buf1[buf2[k].offset + i].ch;
	}
	str[i] = 0;
	return stor_matrix(str);
}

/*
 *根据label返回相应的优先级在栈中，优先级高的返回值大,函数单独有优先级
 */
static int preference1(int label)
{
	switch(label)
	{
		case 1:case 3:return 2;
		case 7:return 4;
		case 8:return 2;
		case 9:case 10:return 6;
		case 11:case 12:case 13:return 8;
		case 14:case 42:case 45:return 10;
		default:return 2;
	}
	return 0;//never happen
}

/*
 *根据label返回相应的优先级将要放入栈中，优先级高的返回值大,函数单独有优先级
 */
static int preference2(int label)
{
	switch(label)
	{
		case 1:case 3:return 3;
		case 7:return 11;
		case 8:return 11;
		case 9:case 10:return 5;
		case 11:case 12:case 13:return 7;
		case 14:case 42:case 45:return 11;
		default:return 11;
	}
	return 0;//never happen
}

/*
 *为buf1中第i个ch开始的字符串(以空格结束，或者是浮点数，或者是一个符号）确定它在句子中的成分
 *长度一般不包括空格,目前label到49，scanner能产生的到41
 */
static word scanner(int k)
{
	word w;
	int i = 0;//offset
	double num = 0;
	int flag = 0;
	char str[256];

	w.label = 0;
	w.length = 1;
	w.offset = k;

	if (buf1[k].ch == ' ')
	{
		w.label = 41;
		return w;
	}

	switch(buf1[k].ch)
	{
		case ':': w.label = 1; return w;
		case ';': w.label = 2; return w;
		case ',': w.label = 3; return w;
		case '.': w.label = 4; return w;
		case '[': w.label = 5; return w;
		case ']': w.label = 6; return w;
		case '=': w.label = 7; return w;
		case '(': w.label = 8; return w;
		case '-': w.label = 9; return w;
		case '+': w.label = 10; return w;
		case '%': w.label = 11; return w;
		case '/': w.label = 12; return w;
		case '*': w.label = 13; return w;
		case '^': w.label = 14; return w;
		case '\'':w.label = 15; return w;
		case ')': w.label = 16; return w;

		default: break;
	}
	if (!cmp(k, "readfrom"))
	{
		w.label = 17;
		w.length = 8;
	}
	else if(!cmp(k, "read"))
	{
		w.label = 18;
		w.length = 4;
	}
	else if(!cmp(k, "writeto"))
	{
		w.label = 19;
		w.length = 7;
	}
	else if(!cmp(k, "rand"))
	{
		w.label = 20;
		w.length = 4;
	}
	else if(!cmp(k, "zeros"))
	{
		w.label = 21;
		w.length = 5;
	}
	else if(!cmp(k, "eye"))
	{
		w.label = 22;
		w.length = 3;
	}
	else if(!cmp(k, "sum"))
	{
		w.label = 23;
		w.length = 3;
	}
	else if(!cmp(k, "max"))
	{
		w.label = 24;
		w.length = 3;
	}
	else if(!cmp(k, "min"))
	{
		w.label = 25;
		w.length = 4;
	}
	else if(!cmp(k, "round"))
	{
		w.label = 26;
		w.length = 5;
	}
	else if(!cmp(k, "upper"))
	{
		w.label = 27;
		w.length = 5;
	}
	else if(!cmp(k, "lower"))
	{
		w.label = 28;
		w.length = 5;
	}
	else if(!cmp(k, "reverse"))
	{
		w.label = 29;
		w.length = 6;
	}
	else if(!cmp(k, "rref"))
	{
		w.label = 30;
		w.length = 4;
	}
	else if(!cmp(k, "det"))
	{
		w.label = 31;
		w.length = 3;
	}
	else if(!cmp(k, "rank"))
	{
		w.label = 32;
		w.length = 4;
	}
	else if(!cmp(k, "dot"))
	{
		w.label = 33;
		w.length = 3;
	}
	else if(!cmp(k, "norm"))
	{
		w.label = 34;
		w.length = 4;
	}
	else if(!cmp(k, "angle"))
	{
		w.label = 35;
		w.length = 5;
	}
	else if(!cmp(k, "eig"))
	{
		w.label = 36;
		w.length = 3;
	}

	if (w.label != 0)
	{
		return w;
	}
//////////////////////////////////////////////////////////////////////标识符或ans或数字
	if(buf1[k].ch == 'a' && buf1[k+1].ch == 'n' && buf1[k+2].ch == 's' && 
		!isalnum(buf1[k+3].ch) && buf1[k+3].ch != '_')
	{
		w.label = 37;
		w.length = 3;
		stor_create(&(w.value.m), ans->m, ans->n);//Todo 出错
		stor_assign(w.value.m, ans);
		return w;
	}

	else if (buf1[k].ch == '_' || isalpha(buf1[k].ch))
	{
		i = 1;
		while(isalnum(buf1[i + k].ch) || buf1[i + k].ch == '_')
		{
			i++;
		}
		w.label = 38;
		w.length = i;
		return w;
	}
	else if (isdigit(buf1[k].ch))//整数不能以0位开头，浮点数可以，但浮点数处理的时候从.后第一位开始处理,用atof函数
	{
		i = 0;
		flag = 0;//if has dot
		while (isdigit(buf1[k + i].ch) || buf1[k + i].ch == '.')
		{
			if (buf1[k + i].ch == '.')
			{
				if (flag == 1)
				{
					//Error
					w.label = 0;
					w.offset = k + i;
					return w;
				}
				flag = 1;
			}
			str[i] = buf1[k + i].ch;
		}
		str[i] = 0;

		if (buf1[i+k].ch !=' ' && buf1[i+k].ch !='%' && buf1[i+k].ch !='^' && buf1[i+k].ch !='*' 
			&& buf1[i+k].ch !=')' && buf1[i+k].ch !='-' && buf1[i+k].ch !='+' && buf1[i+k].ch !=']' 
			&& buf1[i+k].ch !=',' && buf1[i+k].ch !=';' && buf1[i+k].ch !=':'&& buf1[i+k].ch != '/' 
			&& buf1[i+k].ch != 0)
		{
			//Error
			printf("suppose no digit\n");
			w.label = 0;
			w.offset = i + k;
			return w;
		}
		num = atof(str);
		if (util_isLong(num))
		{
			w.label = 39;
			w.length = i;
			w.value.l = (long)floor(num + 0.5);
			return w;
		}
		w.label = 40;
		w.length = i;
		w.value.d = (long)floor(num + 0.5);
		return w;
	}
	//Error
	printf("unknown word");
	w.label = 0;
	return w;
}

/*
 *去除前后空格\n，将中间连续多个\n转化为；,如果后面为]则不转化，处理输出抑制
 *buf到buf1
 *NoError
 */
static void format0(void)
{
	char *p = buf;
	int i = 0;
	while(*p == ' '||*p == '\n')//\n其实不可能
	{
		p++;
	}
	while(*p)
	{
		buf1[i].offset = (p - buf)/sizeof(char);
		if (*p != '\n')
		{
			buf1[i].ch = *p;
			p++;
			i++;
		}
		else
		{
			buf1[i].ch = ';';
			while(*p == '\n'|| *p == ' ') p++;//整行的空格忽略
			if (*p != ']')//如果换行后是]，把最后的那个;消掉
			{
				i++;
			}
		}
	}

	i--;
	while(buf1[i].ch == ' ')
	{
		buf1[i--].ch = 0;
	}
	if (buf1[i].ch == ';')//处理如果最末尾为分号的情况
	{
		uniFlag.show = 0;
		buf1[i].ch = 0;
	}
}


/*
 *将多个空格变成一个空格
 *不能对路径这样化简
 *有出错
 */
static int format1(void)
{
	int i = 0, j = 0;

	if (isPunctuation(buf1[i].ch))
	{
		//Error
		return 1;
	}
	else
	{
		i++;
		j++;
	}
	while(buf1[i].ch)
	{
		if (buf1[i].ch != ' ')
		{
			buf1[j].ch = buf1[i].ch;
			buf1[j++].offset = buf1[i++].offset;
		}
		else
		{
			buf1[j].ch = ' ';
			buf1[j++].offset = buf1[i++].offset;
			while( buf1[i].ch == ' ') i++;
		}
	}
	buf1[j].ch = 0;
	buf1[j].offset = -1;
	return 0;
}

/*
 *削减空格，去掉方括号外所有空格,方括号内除非这个空格前面不是双目运算符，后面跟的是加减或数字才保留,并替换为，
  *没有出错
 */
static int format2(void)
{
	int i, j;
	int flag = 0;
	i = j = 0;
	while(buf2[i].label)
	{
		if (flag == 0)
		{
			if (buf2[i].label != 41)//空格
			{
				buf2[j].label = buf2[i].label;
				buf2[j].length = buf2[i].length;
				buf2[j].offset = buf2[i].offset;
				buf2[j].value = buf2[i].value;
				if (buf2[i].label == 5)
				{
					flag++;
				}
				i++;
				j++;
			}
			else
			{
				i++;
			}
		}
		else 
		{
			if(buf2[i].label != 41)//不是空格
			{
				buf2[j].label = buf2[i].label;
				buf2[j].length = buf2[i].length;
				buf2[j].offset = buf2[i].offset;
				buf2[j].value = buf2[i].value;
				if (buf2[i].label == 6)//]
				{
					flag--;
				}
				i++;
				j++;
			}
			else
			{
				if ((buf2[i+1].label == 8 || buf2[i+1].label == 9 || buf2[i+1].label == 10 
					|| (buf2[i+1].label >=20 && buf2[i+1].label<=40))
					&& (buf2[i-1].label == 4 ||buf2[i-1].label == 15 || buf2[i-1].label == 16 ||buf2[i-1].label == 38
					||buf2[i-1].label == 15||buf2[i-1].label == 39||buf2[i-1].label == 40))
				{
					buf2[j].label = 3;
					buf2[j].length = 1;
					buf2[j].offset = buf2[i].offset;
					buf2[j].value = buf2[i].value;
					j++;
				}
				i++;
			}
		}
	}
	buf2[j].label = 0;
	return 0;
}

/*
 *将多个+和-合并
 *将.和整数合并成实数未完成，将.和^合并成新运算符
 *没有出错，一直返回0
 */
static int format3(void)
{
	int i = 0;
	int j = 0;
	int temp;
	int k;
	int l;
	char str[256];

	while(buf2[i].label)
	{
		if(buf2[i].label != 10 && buf2[i].label != 9)
		{
			if(buf2[i].label == 4 && buf2[i+1].label == 39)
			{
				str[0] = '0';
				str[1] = '.';
				for (l = 0; l<buf2[i+1].length; l++)
				{
					str[l + 2] = buf1[l + buf2[i+1].offset].ch;
				}
				str[l+2] = 0;
				buf2[j].label = 40;
				buf2[j].length = buf2[i+1].length + 1;
				buf2[j].offset = buf2[i].offset;
				buf2[j].value.d = atof(str);
				j++;
				i = i+2;
			}
			else if(buf2[i].label == 4 && buf2[i+1].label == 14)//.^
			{
				buf2[j].label = 42;
				buf2[j].offset = buf2[i].offset;
				buf2[j].length = buf2[i].length;
				buf2[j].value = buf2[i].value;
				j++;
				i = i+2;
			}
			else
			{
				buf2[j].label = buf2[i].label;
				buf2[j].offset = buf2[i].offset;
				buf2[j].length = buf2[i].length;
				buf2[j].value = buf2[i].value;
				i++;
				j++;
			}
		}//not +-
		else
		{
			k = 1;
			temp = i;
			while(buf2[i].label == 10 || buf2[i].label == 9)
			{
				if (buf2[i].label == 9) k = -k;
				i++;
			}
			if (k == 1)
			{
				buf2[j].label = 10;
			}
			else
			{
				buf2[j].label = 9;
			}
			buf2[j].offset = buf2[temp].offset;
			buf2[j].length = 1;
			j++;
		}
	}
	buf2[j].label = 0;
	return 0;
}

/*
*sp2.label == 1,正常返回0,Error
*/
static int colon(void)
{
	int a, b, c;
	int i;
	Matrix *temp = NULL;

	if (stack1[sp1].label != 39)
	{
		//Error
		return stack1[sp1].offset;
	}
	if (stack1[sp1 - 1].label != 39)
	{
		//Error
		return stack1[sp1 - 1].offset;
	}
	if (stack2[sp2 - 1].label == 1)//:
	{
		if (stack1[sp1 - 2].label != 39)
		{
			//Error
			return stack1[sp1 - 2].offset;
		}
		a = stack1[sp1 - 2].value.l;
		b = stack1[sp1 - 1].value.l;
		c = stack1[sp1].value.l;
		if (((c - a) / b)*b != (c - a))
		{
			//Error
			return stack1[sp1 - 2].offset;
		}
		stor_createMatrix(&temp, 1, (c - a) / b + 1);
		for (i = 0; i<(c - a) / b + 1; i++)//Todo 子矩阵的时候，用的是数组，然而处理的时候用的是矩阵，到时候要转换
		{
			*stor_entry(temp, 0, i) = b*i + a;
		}
		stack1[sp1 - 2].label = 47;
		stack1[sp1 - 2].value.m = temp;
		temp = NULL;
		sp1 = sp1 - 2;
		sp2 = sp2 - 2;
	}//3个参数结束
	else
	{
		a = stack1[sp1 - 1].value.l;
		b = stack1[sp1].value.l;
		stor_createMatrix(&temp, 1, b - a + 1);
		for (i = 0; i<(b - a) + 1; i++)
		{
			*stor_entry(temp, 0, i) = a + i;
		}
		stack1[sp1 - 1].label = 47;
		stack1[sp1 - 1].value.m = temp;
		temp = NULL;
		sp1--;
		sp2--;
	}//两个参数结束
	return 0;
}

/*
 *当遇到了逗号的处理方法
 */
static int comma(void)
{
	Matrix *temp = NULL;

	if (stack2[sp2].label == 1)//:
	{
		colon();//Todo处理Error
	}

	if (!isMatrix(stack1[sp1].label))
	{
		if (stack1[sp1].label == 39 || stack1[sp1].label == 40)//trans number to matrix
		{
			stack1[sp1].label = 47;
			stor_createMatrix(&temp, 1,1);
			*stor_entry(temp,0,0) = stack1[sp1].value.l;
			stack1[sp1].value.m = temp;
			temp = NULL;
		}
		else
		{
			//Error
			return stack1[sp1].offset;
		}
	}

	if (stack1[sp1-1].label==0)//only one number
	{
		return 0;
	}
	else
	{
		if (!isMatrix(stack1[sp1-1].label))
		{
			//Error
			return stack1[sp1-1].offset;
		}
		if (!(temp = stor_mergeColum(stack1[sp1 - 1].value.m, stack1[sp1].value.m)))//Merge
		{
			//Error
			return stack1[sp1 - 1].offset;
		}
		if (stack1[sp1].label != 37 && stack1[sp1].label != 38)//不是ans或标识符
		{
			stor_freeMatrix(stack1[sp1].value.m);
		}
		if (stack1[sp1-1].label != 37 && stack1[sp1-1].label != 38)
		{
			stor_freeMatrix(stack1[sp1-1].value.m);
		}
		stack1[sp1-1].label = 47;
		stack1[sp1-1].value.m = temp;
		temp = NULL;
		sp1--;
	}
	return 0;
}

/*
 *当遇到了分号的处理方法
 */
static int semicolon(void)
{
	int err;
	Matrix *temp = NULL;

	if (err = comma())
	{
		return err;
	}
	if (!isMatrix(stack1[sp1].label))
	{
		//Error
		return stack1[sp1].offset;
	}
	if (stack1[sp1-1].label != 0)
	{
		//Error
		return stack1[sp1-1].offset;
	}
	if (sp1-3>0 && stack1[sp1-3].label == 0)//Todo 小心溢出
	{
		if (!isMatrix(stack1[sp1-2].label))
		{
			//Error
			return stack1[sp1-2].offset;
		}
		if (!(temp = stor_mergeRow(stack1[sp1 - 2].value.m, stack1[sp1].value.m)))
		{
			//Error
			return stack1[sp1 - 2].offset;
		}
		if (stack1[sp1].label != 37 && stack1[sp1].label != 38)//不是ans或标识符
		{
			stor_freeMatrix(stack1[sp1].value.m);
		}
		if (stack1[sp1-2].label != 37 && stack1[sp1-2].label != 38)//不是ans或标识符
		{
			stor_freeMatrix(stack1[sp1].value.m);
		}
		stack1[sp1-2].label = 47;
		stack1[sp1-2].value.m = temp;
		temp = NULL;
		sp1 = sp1-2;
	}
	sp1++;
	stack1[sp1].label = 0;
	return 0;
}

/*
 *将buf1中的letter转换为word
 *有出错
 */
static int letter2word(void)
{
	int i = 0;
	int j = 0;
	word w;

	while(buf1[i].ch)
	{
		w = scanner(i);
		if (w.label == 0)
		{
			//Error,不显示
			return 1;
		}
		buf2[j++] = w;
		i = i + w.length;
	}
	buf2[j].label = 0;
	return 0;
}

/*
*从stack1中第k个取一个正整数的参数，Error
*如果是一个子矩阵，子矩阵会被释放掉
*正常返回0
*/
static int getPIP(long *l, int k)
{
	if (!isPInteger(stack1[k]))
	{
		//Error
		return 1;
	}
	if (stack1[k].label == 39)
	{
		*l = stack1[sp1].value.l;
		return 0;
	}
	else
	{
		*l = (long)floor(stack1[k].value.sm->sub->pd[0][0] + 0.5);//子矩阵
		stor_freeSubMatrix(stack1[k].value.sm);
		return 0;
	}
	return 1;//never happen
}

/*
*从stack1中第k个取一个矩阵出来，Error
*不会释放掉
*/
static int getMP(Matrix *m, int k)
{
	if (!isMatrix(stack1[k].label))
	{
		//Error
		return 1;
	}
	if (stack1[k].label == 44)
	{
		m = stack1[k].value.sm->sub;
	}
	else
	{
		m = stack1[k].value.m;
	}
	return 0;
}

/*
 *处理弹出
 */
static int pop(int prefer)//pop 对于右括号一种是表达式一种是参数，区别在逗号？
{
	int n = 1;//计算有几个参数
	long l1, l2;//保存参数
	double d1, d2;//保存参数
	Matrix *m1 = NULL, *m2 = NULL;//保存参数
	word temp;
	double tempd;
	int temp1[10000] = {0}, temp2[10000] = {0};
	char str[256];
	int i;

	if (sp2 == -1)
	{
		return 0;
	}//栈里没有内容
	while (1)
	{
		if (sp2 == -1)//弹出到底了
		{
			if (prefer == -1)
			{
				return 0;
			}
			else
			{
				//Error
				return -1;//之后再处理......
			}
		}
		else if (stack2[sp2].label == 8)//到左括号
		{
			if (prefer == 0)//从右括号开始弹出
			{
				if (flag[sp2] == 1)//被标记了继续弹出，表明栈顶运算符为函数
				{
					sp2--;
					switch(stack2[sp2].label)
					{
						case 20:case 21://rand,zeros
						{
							if (getPIP(&l1, sp1))
							{
								//Error
								return stack1[sp1].offset;
							}
							if (n == 1)//1个参数
							{
								if (stack2[sp2].label == 20)
									calc_rand(l1, l1);//Todo 出错
								else calc_zeros(l1, l1);
								stack1[sp1].label = 47;
								stack1[sp1].value.m = ans;
								ans = NULL;
								break;
							}
							else if (n == 2)
							{
								if (getPIP(&l2, sp1 - 1))
								{
									//Error
									return stack1[sp1 - 1].offset;
								}
								if (stack2[sp2].label == 20)
									calc_rand(l2, l1);
								else calc_zeros(l2, l1);
								sp1--;
								stack1[sp1].label = 47;
								stack1[sp1].value.m = ans;
								ans = NULL;
								break;
							}
							sp2--;
							break;
						}
						case 22://eye
						{
							if (n > 1)
							{
								//Error
								return stack2[sp2].offset;
							}

							if (getPIP(&l1, sp1))
							{
								//Error
								return stack1[sp1].offset;
							}

							if (!calc_eye(l1))
							{
								//Error
								return stack2[sp2].offset;
							}
							stack1[sp1].label = 47;
							stack1[sp1].value.m = ans;
							ans = NULL;
							sp2--;
							break;
						}
						case 23: case 24: case 25:case 31: case 32:case 34://sum,max,min,det,rank,norm
						{
							if (n > 1)
							{
								//Error
								return stack2[sp2].offset;
							}
							if (getMP(m1, sp1))
							{
								//Error
								return stack1[sp1].offset;
							}
							switch (stack2[sp2].label)
							{
								case 23:  calc_sum(m1); break;
								case 24:  calc_max(m1); break;
								case 25:  calc_min(m1); break;
								case 31:  calc_det(m1); break;
								case 32:  calc_rank(m1); break;
								case 34:  calc_norm(m1); break;
							}

							if (stack1[sp1].label == 44)//这矩阵需要被释放
							{
								stor_freeSubMatrix(stack1[sp1].value.sm);
							}
							else if (stack1[sp1].label == 47)
							{
								stor_freeMatrix(stack1[sp1].value.m);
							}

							if (util_isLong(*stor_entry(ans, 0, 0)))
							{
								stack1[sp1].label = 39;
								stack1[sp1].value.l = (long)floor(*stor_entry(ans, 0, 0) + 0.5);
							}
							else
							{
								stack1[sp1].label = 40;
								stack1[sp1].value.d = *stor_entry(ans, 0, 0);
							}

							ans = NULL;//Todo 唯独在计算前将ans=NULL才能使结果保存在ans里，不过通过pop到底也可以
							sp2--;
							break;
						}
						case 26: case 27: case 28:case 29:case 30:case 36://round,upper,lower,reverse,rref,feature
						{
							if (n > 1)
							{
								//Error
								return stack2[sp2].offset;
							}
							if (getMP(m1, sp1))
							{
								//Error
								return stack1[sp1].offset;
							}
							if (stack2[sp2].label == 26)
								calc_round(m1);
							else if (stack2[sp2].label == 27)
								calc_upper(m1);
							else if (stack2[sp2].label == 28)
								calc_lower(m1);
							else if (stack2[sp2].label == 29)
								calc_inverse(m1);
							else if (stack2[sp2].label == 30)
								calc_rref(m1);
							else if (stack2[sp2].label == 36)
								calc_eig(m1);

							if (stack1[sp1].label == 44)//这矩阵需要被释放
							{
								stor_freeSubMatrix(stack1[sp1].value.sm);
							}
							else if (stack1[sp1].label == 47)
							{
								stor_freeMatrix(stack1[sp1].value.m);
							}

							stack1[sp1].label = 47;
							stack1[sp1].value.m = ans;
							ans = NULL;
							sp2--;
							break;
						}
						case 33:case 35://dot,angle
						{
							if (n != 2)
							{
								//Error
								return stack2[sp2].offset;
							}

							if (getMP(m2, sp1))
							{
								//Error
								return stack1[sp1].offset;
							}
							if (getMP(m1, sp1 - 1))
							{
								//Error
								return stack1[sp1 - 1].offset;
							}
							if (stack2[sp2].label == 33)
								calc_dot(m1, m2);
							else calc_angle(m1, m2);

							if (stack1[sp1].label == 44)//这矩阵需要被释放
							{
								stor_freeSubMatrix(stack1[sp1].value.sm);
							}
							else if (stack1[sp1].label == 47)
							{
								stor_freeMatrix(stack1[sp1].value.m);
							}

							if (stack1[sp1 - 1].label == 44)//这矩阵需要被释放
							{
								stor_freeSubMatrix(stack1[sp1 - 1].value.sm);
							}
							else if (stack1[sp1 - 1].label == 47)
							{
								stor_freeMatrix(stack1[sp1 - 1].value.m);
							}

							sp1--;
							if (util_isLong(*stor_entry(ans, 0, 0)))
							{
								stack1[sp1].label = 39;
								stack1[sp1].value.l = (long)floor(*stor_entry(ans, 0, 0) + 0.5);
							}
							else
							{
								stack1[sp1].label = 40;
								stack1[sp1].value.d = *stor_entry(ans, 0, 0);
							}
							ans = NULL;
							sp2--;
							break;
						}
						case 43://取子矩阵函数
						{
							if (n != 2)
							{
								//Error
								return stack2[sp2].offset;
							}
							if (stack1[sp1].label != 46
								&& !(isMatrix(stack1[sp1].label) && stack1[sp1].value.m->m == 1))
							{
								//Error
								return stack1[sp1].offset;
							}
							if (stack1[sp1 - 1].label != 46
								&& !(isMatrix(stack1[sp1 - 1].label) && stack1[sp1 - 1].value.m->m == 1))
							{
								//Error
								return stack1[sp1 - 1].offset;
							}

							if (stack1[sp1].label == 46)
							{
								l2 = stack2[sp2].value.m->n;
for (i = 0; i < l1; i++)
{
	temp2[i] = i + 1;
}
							}
							else
							{
								l2 = stack1[sp1].value.m->n;
								for (i = 0; i < stack1[sp1].value.m->n; i++)
								{
									if (!util_isLong(*stor_entry(stack1[sp1].value.m, 0, i)))
									{
										//Error
										return stack1[sp1].offset;
									}
									temp2[i] = (long)floor(*stor_entry(stack1[sp1].value.m, 0, i) + 0.5);
								}
							}

							if (stack1[sp1 - 1].label == 46)
							{
								l1 = stack2[sp2].value.m->m;
								for (i = 0; i < l1; i++)
								{
									temp1[i] = i + 1;
								}
							}
							else
							{
								l1 = stack1[sp1].value.m->m;
								for (i = 0; i < l1; i++)
								{
									if (!util_isLong(*stor_entry(stack1[sp1].value.m, 0, i)))
									{
										//Error
										return stack1[sp1].offset;
									}
									temp1[i] = (long)floor(*stor_entry(stack1[sp1].value.m, 0, i) + 0.5);
								}
							}
							///////前面把矩阵里的数放到数组里
							if (!(temp.value.sm = stor_subMatrix(stack2[sp2].value.m, l1, l2, temp1, temp2)))
							{
								//Error
								return stack2[sp2].offset;
							}

							if (stack1[sp1].label == 44)//这矩阵需要被释放
							{
								stor_freeSubMatrix(stack1[sp1].value.sm);
							}
							else if (stack1[sp1].label == 47)
							{
								stor_freeMatrix(stack1[sp1].value.m);
							}

							if (stack1[sp1 - 1].label == 44)//这矩阵需要被释放
							{
								stor_freeSubMatrix(stack1[sp1 - 1].value.sm);
							}
							else if (stack1[sp1 - 1].label == 47)
							{
								stor_freeMatrix(stack1[sp1 - 1].value.m);
							}

							sp1--;
							stack1[sp1].label = 44;
							stack1[sp1].value.sm = temp.value.sm;
							sp2--;
							break;
						}
						default:
							//Error
							return -1;
					}//switch 结束
					return 0;
				}//函数处理结束
				else//遇到的是表达式的左括号
				{
					sp2--;
					return 0;
				}
			}//右括号
			else
			{
				//Error，没有右括号直接弹到了左括号
				return stack2[sp2].offset;
			}
		}//左括号弹出
		else if (prefer > preference1(stack2[sp2].label))//弹出完毕
		{
			return 0;
		}
		else if (stack2[sp2].label == 9 || stack2[sp2].label == 10)//-,+
		{
			if (isNum(sp1) && isNum(sp1 - 1))
			{
				if (stack1[sp1].label == 39)
				{
					d2 = stack1[sp1].value.l;
				}
				else
				{
					d2 = stack1[sp1].value.d;
				}

				if (stack1[sp1-1].label == 39)
				{
					d1 = stack1[sp1-1].value.l;
				}
				else
				{
					d1 = stack1[sp1-1].value.d;
				}

				if (stack2[sp2].label == 9)
				{
					tempd = d1 - d2;
				}
				else if (stack2[sp2].label == 10)
				{
					tempd = d1 + d2;
				}
				sp1--;
				if (util_isLong(tempd))
				{
					stack1[sp1].label = 39;
					stack1[sp1].value.l = (long)floor(tempd + 0.5);
				}
				else
				{
					stack1[sp1].label = 40;
					stack1[sp1].value.d = tempd;
				}
				sp2--;
			}
			else if (isMatrix(stack1[sp1].label) && isMatrix(stack1[sp1 - 1].label))
			{
				if (stack1[sp1].label == 44)
				{
					m2 = stack1[sp1].value.sm->sub;
				}
				else
				{
					m2 = stack1[sp1].value.m;
				}

				if (stack1[sp1-1].label == 44)
				{
					m1 = stack1[sp1-1].value.sm->sub;
				}
				else
				{
					m1 = stack1[sp1-1].value.m;
				}

				if (!calc_sub(m1, m2))
				{
					//Error
					return stack2[sp2].offset;
				}
				sp1--;
				stack1[sp1].label = 47;
				stack1[sp1].value.m = ans;
				ans = NULL;
				sp2--;
			}
			else
			{
				//Error
				return stack2[sp2].offset;
			}
		}
		else if (stack2[sp2].label == 45)//单目负号
		{
			if (isNum(sp1))
			{
				if (stack1[sp1].label == 39)
				{
					stack1[sp1].value.l = -stack1[sp1].value.l;
				}
				else
				{
					stack1[sp1].value.d = -stack1[sp1].value.d;
				}
				sp2--;//Todo 可能有些sp1,sp2--忘记了
			}
			else if (isMatrix(stack1[sp1].label))
			{
				if (stack1[sp1].label == 44)
				{
					m1 = stack1[sp1].value.sm->sub;
				}
				else
				{
					m1 = stack1[sp1].value.m;
				}

				if (!calc_numMul(m1, -1))
				{
					//Error
					return stack2[sp2].offset;
				}

				if (stack1[sp1].label == 44)
				{
					stor_freeSubMatrix(stack1[sp1].value.sm);
				}
				else if (stack1[sp1].label == 47)
				{
					stor_freeMatrix(stack1[sp1].value.m);
				}

				stack1[sp1].label = 47;
				stack1[sp1].value.m = ans;
				ans = NULL;
				sp2--;
			}
			else
			{
				//Error
				return stack1[sp1].offset;
			}
		}
		else if (stack2[sp2].label == 13)//*
		{
			if (isNum(sp1))
			{
				if (stack1[sp1].label == 39)
				{
					d2 = stack1[sp1].value.l;
				}
				else if (stack1[sp1].label == 40)
				{
					d2 = stack1[sp1].value.d;
				}

				if (isNum(sp1 - 1))//数乘数
				{
					if (stack1[sp1-1].label == 39)
					{
						d1 = stack1[sp1-1].value.l;
					}
					else if (stack1[sp1-1].label == 40)
					{
						d1 = stack1[sp1-1].value.d;
					}
					tempd = d1*d2;
					sp1--;
					if (util_isLong(tempd))
					{
						stack1[sp1].label = 39;
						stack1[sp1].value.l = (long)floor(tempd + 0.5);
					}
					else
					{
						stack1[sp1].label = 40;
						stack1[sp1].value.d = tempd;
					}
					sp2--;
				}
				else if (isMatrix(stack1[sp1 - 1].label))//矩阵乘数
				{
					if (stack1[sp1 - 1].label == 44)
					{
						m1 = stack1[sp1 - 1].value.sm->sub;
					}
					else
					{
						m1 = stack1[sp1 - 1].value.m;
					}

					if (calc_numMul(m1, d2))
					{
						//Error
						return stack2[sp2].offset;
					}

					if (stack1[sp1 - 1].label == 44)//这矩阵需要被释放
					{
						stor_freeSubMatrix(stack1[sp1 - 1].value.sm);
					}
					else if (stack1[sp1 - 1].label == 47)
					{
						stor_freeMatrix(stack1[sp1 - 1].value.m);
					}
					sp1--;
					stack1[sp1].label = 47;
					stack1[sp1].value.m = ans;
					ans = NULL;
					sp2--;
				}
				else
				{
					//Error
					return stack1[sp1 - 1].offset;
				}
			}
			else if (isMatrix(stack1[sp1].label))
			{
				if (stack1[sp1].label == 44)
				{
					m2 = stack1[sp1].value.sm->sub;
				}
				else
				{
					m2 = stack1[sp1].value.m;
				}

				if (isNum(sp1 - 1))//数乘矩阵
				{
					if (stack1[sp1 - 1].label == 39)
					{
						d1 = stack1[sp1 - 1].value.l;
					}
					else if (stack1[sp1 - 1].label == 40)
					{
						d1 = stack1[sp1 - 1].value.d;
					}

					if (calc_numMul(m2, d1))
					{
						//Error
						return stack2[sp2].offset;
					}

					if (stack1[sp1].label == 44)//这矩阵需要被释放
					{
						stor_freeSubMatrix(stack1[sp1].value.sm);
					}
					else if (stack1[sp1].label == 47)
					{
						stor_freeMatrix(stack1[sp1].value.m);
					}
					sp1--;
					stack1[sp1].label = 47;
					stack1[sp1].value.m = ans;
					ans = NULL;
					sp2--;
				}
				else if (isMatrix(stack1[sp1 - 1].label))//矩阵乘矩阵
				{
					if (stack1[sp1 - 1].label == 44)
					{
						m1 = stack1[sp1 - 1].value.sm->sub;
					}
					else
					{
						m1 = stack1[sp1 - 1].value.m;
					}

					if (calc_mul(m1, m2))
					{
						//Error
						return stack2[sp2].offset;
					}

					if (stack1[sp1].label == 44)//这矩阵需要被释放
					{
						stor_freeSubMatrix(stack1[sp1].value.sm);
					}
					else if (stack1[sp1].label == 47)
					{
						stor_freeMatrix(stack1[sp1].value.m);
					}

					if (stack1[sp1 - 1].label == 44)//这矩阵需要被释放
					{
						stor_freeSubMatrix(stack1[sp1 - 1].value.sm);
					}
					else if (stack1[sp1 - 1].label == 47)
					{
						stor_freeMatrix(stack1[sp1 - 1].value.m);
					}
					sp1--;
					stack1[sp1].label = 47;
					stack1[sp1].value.m = ans;
					ans = NULL;
					sp2--;
				}
				else
				{
					//Error
					return stack1[sp1 - 1].offset;
				}
			}
			else
			{
				//Error
				return stack1[sp1].offset;
			}
		}
		else if (stack2[sp2].label == 11)// %
		{
			if (stack1[sp1].label != 39)
			{
				//Error
				return stack1[sp1].offset;
			}
			l2 = stack1[sp1].value.l;
			if (stack1[sp1 - 1].label == 39)//数和数相mod
			{
				l1 = stack1[sp1].value.l;
				sp1--;
				stack1[sp1].label = 39;
				stack1[sp1].value.l = l1 % l2;//Todo 除以零的错误
				sp2--;
			}
			else if (isMatrix(stack1[sp1 - 1].label))//矩阵mod数
			{
				if (stack1[sp1 - 1].label == 44)
				{
					m1 = stack1[sp1 - 1].value.sm->sub;
				}
				else
				{
					m1 = stack1[sp1 - 1].value.m;
				}

				if (!calc_numMod(m1, l2))
				{
					//Error
					return stack2[sp2].offset;
				}
				
				if (stack1[sp1 - 1].label == 44)
				{
					stor_freeSubMatrix(stack1[sp1 - 1].value.sm);
				}
				else if (stack1[sp1 - 1].label == 47)
				{
					stor_freeMatrix(stack1[sp1 - 1].value.m);
				}

				sp1--;
				stack1[sp1].label = 47;
				stack1[sp1].value.m = ans;
				ans = NULL;
				sp2--;
			}
			else
			{
				//Error
				return stack1[sp1 - 1].offset;//Todo 最后的else部分经常漏掉
			}
		}
		else if (stack2[sp2].label == 12)
		{
			if (isNum(sp1 - 1))//数除数
			{
				if (stack1[sp1 - 1].label == 39)
				{
					d1 = stack1[sp1 - 1].value.l;
				}
				else
				{
					d1 = stack1[sp1 - 1].value.d;
				}

				if (!isNum(sp1))
				{
					//Error
					return stack1[sp1].offset;
				}
				if (stack1[sp1].label == 39)
				{
					d2 = stack1[sp1].value.l;
				}
				else
				{
					d2 = stack1[sp1].value.d;
				}
				tempd = d1 / d2;

				sp1--;
				if (util_isLong(tempd))
				{
					stack1[sp1].label = 39;
					stack1[sp1].value.l = (long)floor(tempd + 0.5);
				}
				else
				{
					stack1[sp1].label = 40;
					stack1[sp1].value.d = tempd;
				}
				sp2--;
			}
			else if (isMatrix(stack1[sp1 - 1].label))//矩阵除
			{
				if (stack1[sp1 - 1].label == 44)
				{
					m1 = stack1[sp1 - 1].value.sm->sub;
				}
				else
				{
					m1 = stack1[sp1 - 1].value.m;
				}

				if (isNum(sp1))//矩阵除数
				{
					if (stack1[sp1].label == 39)
					{
						d2 = stack1[sp1].value.l;
					}
					else
					{
						d2 = stack1[sp1].value.d;
					}

					if (!calc_numMul(m1, (double)1.0 / d2))
					{
						//Error
						return stack2[sp2].offset;
					}

					if (stack1[sp1 - 1].label == 44)
					{
						stor_freeSubMatrix(stack1[sp1 - 1].value.sm);
					}
					else if (stack1[sp1 - 1].label == 47)
					{
						stor_freeMatrix(stack1[sp1 - 1].value.m);
					}

					sp1--;
					stack1[sp1].label = 47;
					stack1[sp1].value.m = ans;
					ans = NULL;
					sp2--;
				}
				else if (isMatrix(stack1[sp1].label))//矩阵除矩阵
				{
					if (stack1[sp1].label == 44)
					{
						m2 = stack1[sp1].value.sm->sub;
					}
					else
					{
						m2 = stack2[sp1].value.m;
					}

					if (!calc_div(m1, m2))
					{
						//Error
						return stack2[sp2].offset;
					}

					if (stack1[sp1 - 1].label == 44)
					{
						stor_freeSubMatrix(stack1[sp1 - 1].value.sm);
					}
					else if (stack1[sp1 - 1].label == 47)
					{
						stor_freeMatrix(stack1[sp1 - 1].value.m);
					}

					if (stack1[sp1].label == 44)
					{
						stor_freeSubMatrix(stack1[sp1].value.sm);
					}
					else if (stack1[sp1].label == 47)
					{
						stor_freeMatrix(stack1[sp1].value.m);
					}

					sp1--;
					stack1[sp1].label = 47;
					stack1[sp1].value.m = ans;
					ans = NULL;
					sp2--;
				}
				else
				{
					//Error
					return stack1[sp1].offset;
				}
				
			}
			else
			{
				//Error
				return stack1[sp1 - 1].offset;
			}
		}
		else if (stack2[sp2].label == 14 || stack2[sp2].label == 42)//^, .^
		{
			if (stack1[sp1].label != 39)
			{
				//Error
				return stack1[sp1].offset;
			}
			l2 = stack1[sp1].value.l;
			if (isNum(sp1 - 1) && stack2[sp2].label == 42)//Todo 也许从不会发生，因为数字加点会被解释成浮点数
			{
				//Error
				return stack2[sp2].offset;
			}

			if (isNum(sp1 - 1))//数的乘方
			{
				if (stack1[sp1 - 1].label == 39)
				{
					d1 = stack1[sp1 - 1].value.l;
				}
				else
				{
					d1 = stack1[sp1 - 1].value.d;
				}

				if (!calc_numEx(d1, l2))
				{
					//Error
					return stack2[sp2].offset;
				}
				
				tempd = *stor_entry(ans, 0, 0);
				sp1--;
				if (util_isLong(tempd))
				{
					stack1[sp1].label = 39;
					stack1[sp1].value.l = (long)floor(tempd + 0.5);
				}
				else
				{
					stack1[sp1].label = 40;
					stack1[sp1].value.d = tempd;
				}
				sp2--;
			}
			else if (isMatrix(sp1 - 1))//矩阵的两种乘方
			{
				if (stack1[sp1 - 1].label == 44)
				{
					m1 = stack1[sp1 - 1].value.sm->sub;
				}
				else
				{
					m1 = stack1[sp1 - 1].value.m;
				}

				if (stack2[sp2].label == 14)//矩阵的乘方
				{
					if (!calc_ex(m1, l2))//Todo 检查出错的地方是不是有！,要分情况
					{
						//Error
						return stack2[sp2].offset;
					}
				}
				else//矩阵各数的乘方
				{
					if (!calc_everyEx(m1, l2))
					{
						//Error
						return stack2[sp2].offset;
					}
				}

				if (stack1[sp1 - 1].label == 44)//这矩阵需要被释放
				{
					stor_freeSubMatrix(stack1[sp1 - 1].value.sm);
				}
				else if (stack1[sp1 - 1].label == 47)
				{
					stor_freeMatrix(stack1[sp1 - 1].value.m);
				}

				sp1--;
				stack1[sp1].label = 47;
				stack1[sp1].value.m = ans;
				ans = NULL;
				sp2--;
			}
			else
			{
				//Error
				return stack1[sp1 - 1].offset;
			}

		}
		else if (stack2[sp2].label == 7)//=
		{
			if (stack1[sp1-1].label != 44 && stack1[sp1-1].label != 38)
			{
				//Error
				return stack1[sp1-1].offset;
			}
			if (!isMatrix(stack1[sp1].label))
			{
				//Error
				return stack1[sp1].offset;
			}
			if (getMP(m1, sp1))
			{
				//Error
				return stack1[sp1].offset;
			}
			if (stack1[sp1-1].label == 44)//子矩阵
			{
				if (!stor_assign(stack1[sp1 - 1].value.sm->sub, m1))
				{
					//Error
					return stack2[sp2].offset;
				}
				stor_mergeSubMatrix(stack1[sp1 - 1].value.sm);
			}
			else
			{
				if (stack1[sp1 - 1].value.m == NULL)
				{
					for (i = 0; i<stack1[sp1 - 1].length; i++)
					{
						str[i] = buf1[stack1[sp1 - 1].offset + i].ch;
					}
					str[i] = 0;
					if (stor_create(str, m1->m, m1->n))
					{
						//Error
						return stack1[sp1].offset;
					}
				}//Todo 检查offset
				if (!stor_assign(stack1[sp1 - 1].value.m, m1))
				{
					//Error
					return stack2[sp2].offset;
				}
			}
			sp2--;
			sp1--;
		}
	}
}

/*
 *根据词法进行相应的计算和判断
 *及时处理整数和浮点数问题，不在要用的时候判断
 */
static int parser(void)
{
	int i,err;
	Matrix *temp = NULL;
	Matrix *oldAns = ans;
	ans = NULL;
    //防止出现错误导致ans丢失//Todo 出错时记得恢复ans//Todo 在每次计算前ans弄为NULL
	//ans的37在这个阶段还有，但是已经将值带进去了
	i = 0;
	flagn = 0;
	while (buf2[i].label)
	{
		if (buf2[i].label >= 20 && buf2[i].label <=36)//函数
		{
			if (flagn == 1)
			{
				//Error
				return buf2[i].offset;
			}
			//if (buf2[i+1].label != 16) Question
			if (buf2[i+1].label != 8)
			{
				//Error
				return buf2[i].offset;
			}
			sp2++;
			stack2[sp2] = buf2[i];
			sp2++; i++;
			stack2[sp2] = buf2[i];
			flag[sp2] = 1;
			flagn = 0;
			i++;
		}//函数结束
		else if (buf2[i].label == 37 || buf2[i].label == 39 || buf2[i].label == 40)
		{
			if (flagn == 1)
			{
				//Error
				return buf2[i].offset;
			}
			sp1++;
			if (buf2[i].label != 37)
			{
				stack1[sp1].label = buf2[i].label;
			}
			else
			{
				stack1[sp1].label = 47;
			}
			stack1[sp1].length = buf2[i].length;
			stack1[sp1].offset = buf2[i].offset;
			stack1[sp1].value.m = buf2[i].value.m;
			flagn = 1;
			i++;
		}
		else if (buf2[i].label == 38)//标识符
		{
			if (flagn == 1)
			{
				//Error
				return buf2[i].offset;
			}
			if (buf2[i+1].label == 8)//标识符函数
			{
				sp2++;
				stack2[sp2] = buf2[i];
				stack2[sp2].label = 43;
				stack2[sp2].value.m = getMatrix(i);
				sp2++;
				i++;
				stack2[sp2] = buf2[i];
				flag[sp2] = 1;
				flagn = 0;
				i++;
			}
			else//标识符
			{
				sp1++;
				//stack1[sp1] = buf2[i];//Todo 修改类似情形
				stack1[sp1].label = buf2[i].label;
				stack1[sp1].length = buf2[i].length;
				stack1[sp1].offset = buf2[i].offset;
				stack1[sp1].value.m = getMatrix(i);
				flagn = 1;
				i++;
			}
		}
		else if ((buf2[i].label >=7 && buf2[i].label <= 14 )
			||buf2[i].label == 42)
		{
			if (flagn == 0)
			{
				if(buf2[i].label == 8)//(
				{
					sp2++;
					stack2[sp2] = buf2[i];
					flagn = 0;
					i++;
				}
				else if(buf2[i].label == 10)//+单目
				{
					i++;
					flagn = 0;
				}
				else if(buf2[i].label == 9)//-单目
				{
					sp2++;
					stack2[sp2]=buf2[i];
					stack2[sp2].label = 45;
					flagn = 0;
					i++;
				}
				else
				{
					//Error
					return buf2[i].offset;
				}
			}//flagn == 0结束
			else
			{
				if (buf2[i].label == 8)//(
				{
					//Error
					return buf2[i].offset;
				}
				else 
				{
					if (err = pop(preference2(buf2[i].label)))//pop里要进行判定
					{
						//Error
						return err;
					}
					sp2++;
					stack2[sp2] = buf2[i];
					flagn = 0;
					i++;
				}
			}//flagn ==1 结束
		}//有优先级的运算符结束
		else if (buf2[i].label == 1)//://Todo 如何处理子矩阵时用到的:
		{
			if (flagn == 1)
			{
				if (err = pop(preference2(buf2[i].label)))//Todo 优化,用1代替buf2[i].label
				{
					//Error
					return err;
				}
				sp2++;
				stack2[sp2] = buf2[i];
				flagn = 0;
				i++;
			}
			else if (stack2[sp2].label == 3)//,
			{
				sp1++;
				stack1[sp1] = buf2[i];
				stack1[sp1].label = 46;//全行列
				flagn = 0;
				i++;
			}
			else
			{
				//Error
				return buf2[i].offset;
			}
		}//:结束
		else if(buf2[i].label == 15)//'
		{
			if (flagn == 0)
			{
				//Error
				return buf2[i].offset;
			}
			if(!isMatrix(stack1[sp1].label))
			{
				//Error
				return buf2[i].offset;
			}
			else
			{
				calc_trans(stack1[sp1].value.m);
				stack1[sp1].label = 47;
				stack1[sp1].offset = stack1[sp1].offset;
				stack1[sp1].length = 0;
				stack1[sp1].value.m = ans;
				ans = NULL;
				flagn = 1;
				i++;
			}
		}//'结束
		else if (buf2[i].label == 16)
		{
			if (flagn == 0)
			{
				//Error
				return buf2[i].offset;
			}
			if (err = pop(3))//先按,的优先级弹一遍，使得如果为函数的话，左右两括号间只有，和:
			{
				//Error;
				return err;
			}
			if (err = pop(0))//pop中0表示一直弹到左括号
			{
				//Error;
				return err;
			}
			flagn = 1;
			i++;
		}//)结束
		else if (buf2[i].label == 5)//[
		{
			if (flagn == 1)
			{
				//Error
				return buf2[i].offset;
			}
			sp1++;
			stack1[sp1].label = 0;
			stack1[sp1].offset = buf2[i].offset;
			flagn = 0;
			i++;
		}//[结束
		else if (buf2[i].label == 6)//]
		{
			if (flagn == 0)
			{
				//Error
				return buf2[i].offset;
			}
			if (err = semicolon())
			{
				//Error
				return err;
			}
			//stack1[sp1 - 2] = stack1[sp1 - 1];
			stack1[sp1 - 2].label = stack1[sp1 - 1].label;
			stack1[sp1 - 2].length = stack1[sp1 - 1].length;
			stack1[sp1 - 2].offset = stack1[sp1 - 1].offset;
			stack1[sp1 - 2].value.m = stack1[sp1 - 1].value.m;//Todo length应该是不断相加的，检查一下
			sp1 = sp1-2;
			flagn = 1;
			i++;
		}//]结束
		else if (buf2[i].label == 3)//,
		{
			if (flagn == 0)
			{
				//Error
				return buf2[i].offset;
			}
			if (stack1[sp1-1].label == 0 || stack1[sp1-2].label == 0)//是在定义矩阵中
			{
				if (err = pop(preference2(buf2[i].label)))
				{
					//Error
					return err;
				}
				if (err = comma())
				{
					//Error
					return err;
				}
				flagn = 0;
				i++;
			}
			else//在函数中分隔参数
			{
				if (err = pop(preference2(buf2[i].label)))
				{
					//Error
					return err;
				}
				if (stack2[sp2].label == 1)//:
				{
					colon();//Todo处理Error
				}
				sp2++;

				stack2[sp2].label = buf2[i].label;
				stack2[sp2].length = buf2[i].length;
				stack2[sp2].offset = buf2[i].offset;

				flagn = 0;
				i++;
			}
		}//,结束
		else if (buf2[i].label == 2)//;
		{
			if (flagn == 0)
			{
				//Error
				return buf2[i].offset;
			}
			semicolon();
			flagn = 0;
			i++;
		}
	}
	if (err = pop(-1))//-1在pop中表示全部弹出
	{
		//Error
		return err;
	}

	if (stack1[sp1].label == 44)
	{
		ans = stack1[sp1].value.sm->sub;
	}
	else if (stack1[sp1].label == 47 || stack1[sp1].label == 38)
	{
		ans = stack1[sp1].value.m;
	}
	else if (stack1[sp1].label == 39)
	{
		stor_createAns(1, 1);//Error
		*stor_entry(ans, 0, 0) = stack1[sp1].value.l;
	}
	else if (stack1[sp1].label == 40)
	{
		stor_createAns(1, 1);//Error
		*stor_entry(ans, 0, 0) = stack1[sp1].value.d;
	}
	else
	{
		//Error
		return stack1[sp1].offset;
	}
	sp1--;

	//打印结果
	stor_freeMatrix(oldAns);
	return 0;
}
		
/*
 *读入的指令已放在buf中
 *没有错误返回0，有错返回1
 */
int com_interpret(char * const buf)//先处理readfrom read writeto, 对命令进行格式化，处理命令，处理错误，最后恢复uniflag
{
	int i, j;
	char str[256];
	int flag = 0;//记录上一个被压入栈的是不是数
	word w;

	format0();
	w = scanner(0);
	if (w.label == 17||w.label == 19)//readfrom 或 writeto
	{
		i = w.length;//第一个字符位置
		while(buf1[i].ch == ' ') i++;
		j = 0;
		while(buf1[i].ch)
		{
			str[j++] = buf1[i++].ch;
		}
		str[j] = 0;
		if (w.label == 17)
		{
			if (uniFlag.in == 1)
			{
				if (fclose(fpin))
				{
					//Error
					return 1;
				}
			}
			if(fopen_s(fpin, str, "r"))
			{
				//Error
				return 1;
			}
			util_strcpy(filein, str);
			uniFlag.in = 1;
		}
		else
		{
			if (uniFlag.out = 1)
			{
				if (fclose(fpout))
				{
					//Error
					return 1;
				}
			}
			if (strlen(str) == 0)//没有跟参数
			{
				fileout[0] = 0;
				uniFlag.out = 0;
				return 0;
			}
			if (fopen_s(fpout, str, "a"))
			{
				//Error
				return 1;
			}
			util_strcpy(fileout, str);
			uniFlag.out = 1;
		}
		return 0;
	}

	if (format1())
	{
		//Error不显示
		return 1;
	}

	if (letter2word())
	{
		//Error不显示
		return 1;
	}

	format2();
	format3();
	parser();

	/*while (buf1[i].ch)
	{
		printf("%c",buf1[i].ch);
		i++;
	}
	printf("\n");
	format1();

	i = 0;
	while (buf1[i].ch)
	{
		printf("%c",buf1[i].ch);
		i++;
	}*/
	uniFlag.show = 1;
	return 0;
}
