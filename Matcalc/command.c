#include "universe.h"
#include "command.h"
#include "storage.h"
#include "calculate.h"
#include "util.h"

static char buf[512];
static letter buf1[512];
static word buf2[256];
static word stack1[256];//��Ų�����
static word stack2[256];//���ָ��
static int sp1 = -1,sp2 = -1;//ָ��ջ��
static int flag[256] = {0};//���ں������õ������Լ����Ĵ������ĳһ�㱻��ע����ò㵯����Ҫ������һ��
static int flagn = 0;//���ڱ�ʾ����ѹ��Ķ���֮ǰ�ǲ������֣�����-�Ĵ���,flagnֻ�ڣ�;,[=(%/*^�Լ�.^����0������Ϊ1,�����ŵ�ʱ��ֱ���ж�һ�£���һ���ַ��������ⶼҪ����

/*
 *�ж��Ƿ�Ϊ����
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
 *����label�ж��ǲ��Ǿ���
 */
static int isMatrix(int label)
{
	if (label != 37 && label != 38 &&  label !=44 && label != 47)//Todo 37ʵ������ʧ
	{
		return 0;
	}
	else 
	{
		return 1;
	}
}

/*
*stack1[k]�ж��ǲ�����,�������򸡵�������1
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
	if (w.label == 39)//����
	{
		if (w.value.l <= 0)
			return 0;
		else
			return 1;
	}
	if (w.label == 44)//�Ӿ���
	{
		if (w.value.sm->sub->m == 1&& w.value.sm->sub->n == 1)
		{
			if (w.value.sm->sub->pd != NULL && w.value.sm->sub->pd[0][0]>0
				&& util_isLong(w.value.sm->sub->pd[0][0]))//Todo pdΪNULL���ж�
			{
				return 1;
			}
		}
		return 0;
	}
	return 0;//never happen
}

/*
 *�Ƚ�buf1���±�k��ʼ���ַ�����ǰ��λ�Ƿ��s2���Դ�Сд��ͬ
 *��ͬ����0������ͬ���ص�һ������ͬ�ľ����±��1��ʼ����buf��Ϊk
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
	if (isalnum(buf1[j+k].ch)||buf1[j+k].ch == '_')//����Ǹ���ʶ��
		return j+1;
	return 0;
}

/*
 *����buf2�е�k��word�����Ӧ��matrixָ��
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
 *����label������Ӧ�����ȼ���ջ�У����ȼ��ߵķ���ֵ��,�������������ȼ�
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
 *����label������Ӧ�����ȼ���Ҫ����ջ�У����ȼ��ߵķ���ֵ��,�������������ȼ�
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
 *Ϊbuf1�е�i��ch��ʼ���ַ���(�Կո�����������Ǹ�������������һ�����ţ�ȷ�����ھ����еĳɷ�
 *����һ�㲻�����ո�,Ŀǰlabel��49��scanner�ܲ����ĵ�41
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
//////////////////////////////////////////////////////////////////////��ʶ����ans������
	if(buf1[k].ch == 'a' && buf1[k+1].ch == 'n' && buf1[k+2].ch == 's' && 
		!isalnum(buf1[k+3].ch) && buf1[k+3].ch != '_')
	{
		w.label = 37;
		w.length = 3;
		stor_create(&(w.value.m), ans->m, ans->n);//Todo ����
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
	else if (isdigit(buf1[k].ch))//����������0λ��ͷ�����������ԣ��������������ʱ���.���һλ��ʼ����,��atof����
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
 *ȥ��ǰ��ո�\n�����м��������\nת��Ϊ��,�������Ϊ]��ת���������������
 *buf��buf1
 *NoError
 */
static void format0(void)
{
	char *p = buf;
	int i = 0;
	while(*p == ' '||*p == '\n')//\n��ʵ������
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
			while(*p == '\n'|| *p == ' ') p++;//���еĿո����
			if (*p != ']')//������к���]���������Ǹ�;����
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
	if (buf1[i].ch == ';')//���������ĩβΪ�ֺŵ����
	{
		uniFlag.show = 0;
		buf1[i].ch = 0;
	}
}


/*
 *������ո���һ���ո�
 *���ܶ�·����������
 *�г���
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
 *�����ո�ȥ�������������пո�,�������ڳ�������ո�ǰ�治��˫Ŀ���������������ǼӼ������ֲű���,���滻Ϊ��
  *û�г���
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
			if (buf2[i].label != 41)//�ո�
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
			if(buf2[i].label != 41)//���ǿո�
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
 *�����+��-�ϲ�
 *��.�������ϲ���ʵ��δ��ɣ���.��^�ϲ����������
 *û�г���һֱ����0
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
*sp2.label == 1,��������0,Error
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
		for (i = 0; i<(c - a) / b + 1; i++)//Todo �Ӿ����ʱ���õ������飬Ȼ�������ʱ���õ��Ǿ��󣬵�ʱ��Ҫת��
		{
			*stor_entry(temp, 0, i) = b*i + a;
		}
		stack1[sp1 - 2].label = 47;
		stack1[sp1 - 2].value.m = temp;
		temp = NULL;
		sp1 = sp1 - 2;
		sp2 = sp2 - 2;
	}//3����������
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
	}//������������
	return 0;
}

/*
 *�������˶��ŵĴ�����
 */
static int comma(void)
{
	Matrix *temp = NULL;

	if (stack2[sp2].label == 1)//:
	{
		colon();//Todo����Error
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
		if (stack1[sp1].label != 37 && stack1[sp1].label != 38)//����ans���ʶ��
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
 *�������˷ֺŵĴ�����
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
	if (sp1-3>0 && stack1[sp1-3].label == 0)//Todo С�����
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
		if (stack1[sp1].label != 37 && stack1[sp1].label != 38)//����ans���ʶ��
		{
			stor_freeMatrix(stack1[sp1].value.m);
		}
		if (stack1[sp1-2].label != 37 && stack1[sp1-2].label != 38)//����ans���ʶ��
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
 *��buf1�е�letterת��Ϊword
 *�г���
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
			//Error,����ʾ
			return 1;
		}
		buf2[j++] = w;
		i = i + w.length;
	}
	buf2[j].label = 0;
	return 0;
}

/*
*��stack1�е�k��ȡһ���������Ĳ�����Error
*�����һ���Ӿ����Ӿ���ᱻ�ͷŵ�
*��������0
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
		*l = (long)floor(stack1[k].value.sm->sub->pd[0][0] + 0.5);//�Ӿ���
		stor_freeSubMatrix(stack1[k].value.sm);
		return 0;
	}
	return 1;//never happen
}

/*
*��stack1�е�k��ȡһ�����������Error
*�����ͷŵ�
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
 *������
 */
static int pop(int prefer)//pop ����������һ���Ǳ��ʽһ���ǲ����������ڶ��ţ�
{
	int n = 1;//�����м�������
	long l1, l2;//�������
	double d1, d2;//�������
	Matrix *m1 = NULL, *m2 = NULL;//�������
	word temp;
	double tempd;
	int temp1[10000] = {0}, temp2[10000] = {0};
	char str[256];
	int i;

	if (sp2 == -1)
	{
		return 0;
	}//ջ��û������
	while (1)
	{
		if (sp2 == -1)//����������
		{
			if (prefer == -1)
			{
				return 0;
			}
			else
			{
				//Error
				return -1;//֮���ٴ���......
			}
		}
		else if (stack2[sp2].label == 8)//��������
		{
			if (prefer == 0)//�������ſ�ʼ����
			{
				if (flag[sp2] == 1)//������˼�������������ջ�������Ϊ����
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
							if (n == 1)//1������
							{
								if (stack2[sp2].label == 20)
									calc_rand(l1, l1);//Todo ����
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

							if (stack1[sp1].label == 44)//�������Ҫ���ͷ�
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

							ans = NULL;//Todo Ψ���ڼ���ǰ��ans=NULL����ʹ���������ans�����ͨ��pop����Ҳ����
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

							if (stack1[sp1].label == 44)//�������Ҫ���ͷ�
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

							if (stack1[sp1].label == 44)//�������Ҫ���ͷ�
							{
								stor_freeSubMatrix(stack1[sp1].value.sm);
							}
							else if (stack1[sp1].label == 47)
							{
								stor_freeMatrix(stack1[sp1].value.m);
							}

							if (stack1[sp1 - 1].label == 44)//�������Ҫ���ͷ�
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
						case 43://ȡ�Ӿ�����
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
							///////ǰ��Ѿ���������ŵ�������
							if (!(temp.value.sm = stor_subMatrix(stack2[sp2].value.m, l1, l2, temp1, temp2)))
							{
								//Error
								return stack2[sp2].offset;
							}

							if (stack1[sp1].label == 44)//�������Ҫ���ͷ�
							{
								stor_freeSubMatrix(stack1[sp1].value.sm);
							}
							else if (stack1[sp1].label == 47)
							{
								stor_freeMatrix(stack1[sp1].value.m);
							}

							if (stack1[sp1 - 1].label == 44)//�������Ҫ���ͷ�
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
					}//switch ����
					return 0;
				}//�����������
				else//�������Ǳ��ʽ��������
				{
					sp2--;
					return 0;
				}
			}//������
			else
			{
				//Error��û��������ֱ�ӵ�����������
				return stack2[sp2].offset;
			}
		}//�����ŵ���
		else if (prefer > preference1(stack2[sp2].label))//�������
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
		else if (stack2[sp2].label == 45)//��Ŀ����
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
				sp2--;//Todo ������Щsp1,sp2--������
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

				if (isNum(sp1 - 1))//������
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
				else if (isMatrix(stack1[sp1 - 1].label))//�������
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

					if (stack1[sp1 - 1].label == 44)//�������Ҫ���ͷ�
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

				if (isNum(sp1 - 1))//���˾���
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

					if (stack1[sp1].label == 44)//�������Ҫ���ͷ�
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
				else if (isMatrix(stack1[sp1 - 1].label))//����˾���
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

					if (stack1[sp1].label == 44)//�������Ҫ���ͷ�
					{
						stor_freeSubMatrix(stack1[sp1].value.sm);
					}
					else if (stack1[sp1].label == 47)
					{
						stor_freeMatrix(stack1[sp1].value.m);
					}

					if (stack1[sp1 - 1].label == 44)//�������Ҫ���ͷ�
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
			if (stack1[sp1 - 1].label == 39)//��������mod
			{
				l1 = stack1[sp1].value.l;
				sp1--;
				stack1[sp1].label = 39;
				stack1[sp1].value.l = l1 % l2;//Todo ������Ĵ���
				sp2--;
			}
			else if (isMatrix(stack1[sp1 - 1].label))//����mod��
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
				return stack1[sp1 - 1].offset;//Todo ����else���־���©��
			}
		}
		else if (stack2[sp2].label == 12)
		{
			if (isNum(sp1 - 1))//������
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
			else if (isMatrix(stack1[sp1 - 1].label))//�����
			{
				if (stack1[sp1 - 1].label == 44)
				{
					m1 = stack1[sp1 - 1].value.sm->sub;
				}
				else
				{
					m1 = stack1[sp1 - 1].value.m;
				}

				if (isNum(sp1))//�������
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
				else if (isMatrix(stack1[sp1].label))//���������
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
			if (isNum(sp1 - 1) && stack2[sp2].label == 42)//Todo Ҳ��Ӳ��ᷢ������Ϊ���ּӵ�ᱻ���ͳɸ�����
			{
				//Error
				return stack2[sp2].offset;
			}

			if (isNum(sp1 - 1))//���ĳ˷�
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
			else if (isMatrix(sp1 - 1))//��������ֳ˷�
			{
				if (stack1[sp1 - 1].label == 44)
				{
					m1 = stack1[sp1 - 1].value.sm->sub;
				}
				else
				{
					m1 = stack1[sp1 - 1].value.m;
				}

				if (stack2[sp2].label == 14)//����ĳ˷�
				{
					if (!calc_ex(m1, l2))//Todo ������ĵط��ǲ����У�,Ҫ�����
					{
						//Error
						return stack2[sp2].offset;
					}
				}
				else//��������ĳ˷�
				{
					if (!calc_everyEx(m1, l2))
					{
						//Error
						return stack2[sp2].offset;
					}
				}

				if (stack1[sp1 - 1].label == 44)//�������Ҫ���ͷ�
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
			if (stack1[sp1-1].label == 44)//�Ӿ���
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
				}//Todo ���offset
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
 *���ݴʷ�������Ӧ�ļ�����ж�
 *��ʱ���������͸��������⣬����Ҫ�õ�ʱ���ж�
 */
static int parser(void)
{
	int i,err;
	Matrix *temp = NULL;
	Matrix *oldAns = ans;
	ans = NULL;
    //��ֹ���ִ�����ans��ʧ//Todo ����ʱ�ǵûָ�ans//Todo ��ÿ�μ���ǰansŪΪNULL
	//ans��37������׶λ��У������Ѿ���ֵ����ȥ��
	i = 0;
	flagn = 0;
	while (buf2[i].label)
	{
		if (buf2[i].label >= 20 && buf2[i].label <=36)//����
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
		}//��������
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
		else if (buf2[i].label == 38)//��ʶ��
		{
			if (flagn == 1)
			{
				//Error
				return buf2[i].offset;
			}
			if (buf2[i+1].label == 8)//��ʶ������
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
			else//��ʶ��
			{
				sp1++;
				//stack1[sp1] = buf2[i];//Todo �޸���������
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
				else if(buf2[i].label == 10)//+��Ŀ
				{
					i++;
					flagn = 0;
				}
				else if(buf2[i].label == 9)//-��Ŀ
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
			}//flagn == 0����
			else
			{
				if (buf2[i].label == 8)//(
				{
					//Error
					return buf2[i].offset;
				}
				else 
				{
					if (err = pop(preference2(buf2[i].label)))//pop��Ҫ�����ж�
					{
						//Error
						return err;
					}
					sp2++;
					stack2[sp2] = buf2[i];
					flagn = 0;
					i++;
				}
			}//flagn ==1 ����
		}//�����ȼ������������
		else if (buf2[i].label == 1)//://Todo ��δ����Ӿ���ʱ�õ���:
		{
			if (flagn == 1)
			{
				if (err = pop(preference2(buf2[i].label)))//Todo �Ż�,��1����buf2[i].label
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
				stack1[sp1].label = 46;//ȫ����
				flagn = 0;
				i++;
			}
			else
			{
				//Error
				return buf2[i].offset;
			}
		}//:����
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
		}//'����
		else if (buf2[i].label == 16)
		{
			if (flagn == 0)
			{
				//Error
				return buf2[i].offset;
			}
			if (err = pop(3))//�Ȱ�,�����ȼ���һ�飬ʹ�����Ϊ�����Ļ������������ż�ֻ�У���:
			{
				//Error;
				return err;
			}
			if (err = pop(0))//pop��0��ʾһֱ����������
			{
				//Error;
				return err;
			}
			flagn = 1;
			i++;
		}//)����
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
		}//[����
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
			stack1[sp1 - 2].value.m = stack1[sp1 - 1].value.m;//Todo lengthӦ���ǲ�����ӵģ����һ��
			sp1 = sp1-2;
			flagn = 1;
			i++;
		}//]����
		else if (buf2[i].label == 3)//,
		{
			if (flagn == 0)
			{
				//Error
				return buf2[i].offset;
			}
			if (stack1[sp1-1].label == 0 || stack1[sp1-2].label == 0)//���ڶ��������
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
			else//�ں����зָ�����
			{
				if (err = pop(preference2(buf2[i].label)))
				{
					//Error
					return err;
				}
				if (stack2[sp2].label == 1)//:
				{
					colon();//Todo����Error
				}
				sp2++;

				stack2[sp2].label = buf2[i].label;
				stack2[sp2].length = buf2[i].length;
				stack2[sp2].offset = buf2[i].offset;

				flagn = 0;
				i++;
			}
		}//,����
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
	if (err = pop(-1))//-1��pop�б�ʾȫ������
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

	//��ӡ���
	stor_freeMatrix(oldAns);
	return 0;
}
		
/*
 *�����ָ���ѷ���buf��
 *û�д��󷵻�0���д���1
 */
int com_interpret(char * const buf)//�ȴ���readfrom read writeto, ��������и�ʽ���������������������ָ�uniflag
{
	int i, j;
	char str[256];
	int flag = 0;//��¼��һ����ѹ��ջ���ǲ�����
	word w;

	format0();
	w = scanner(0);
	if (w.label == 17||w.label == 19)//readfrom �� writeto
	{
		i = w.length;//��һ���ַ�λ��
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
			if (strlen(str) == 0)//û�и�����
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
		//Error����ʾ
		return 1;
	}

	if (letter2word())
	{
		//Error����ʾ
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
