#include "universe.h"
#include "command.h"
#include "storage.h"
#include "calculate.h"

char buf [256];
static letter buf1[512];
static word buf2[256];
static word stack1[256];//��Ų�����
static word stack2[256];//���ָ��
static int sp1 = -1,sp2 = -1;//ָ��ջ��
static int flag[256] = {0};//���ں������õ������Լ����Ĵ������ĳһ�㱻��ע����ò㵯����Ҫ������һ��
static int flagn = 0;//���ڱ�ʾ����ѹ��Ķ���֮ǰ�ǲ������֣�����-�Ĵ���,flagnֻ�ڣ�;,[=(%/*^�Լ�.^����0������Ϊ1,�����ŵ�ʱ��ֱ���ж�һ�£���һ���ַ��������ⶼҪ����
static int flagc = 0;//ǰһ���ǲ��Ƕ���

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
	if (label != 37 && label != 38 &&  label !=44 && label != 47)
	{
		return 0;
	}
	else 
	{
		return 1;
	}
}

static int isPInteger(word w)
{
	if (w.label != 39 && w.label != 44)
	{
		return 0;
	}
	if (w.label == 39)//����
	{
		if (w.value.valuel <= 0)
			return 0;
		else
			return 1;
	}
	if (w.label == 44)//�Ӿ���
	{
		if (w.value.matrix->m == 1&& w.value.matrix->n == 1)
		{
			if (w.value.matrix->pl != NULL && w.value.matrix->pl[0][0]>0)
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
	for (j = 0; j < strlen(s2); j++)
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
 *���ַ���ת��Ϊlong double
 */
static long double a2f(char *s)
{
	int i = 0;
	long double sum = 0;
	int flag = 0;
	long double exp;
	while(s[i])
	{
		if (s[i] == '.')
		{
			flag = 1;
			exp = 0.1;
		}
		else
		{
			if (flag == 0)//û����С����
			{
				sum = sum*10 + s[i]-'0';
			}
			else
			{
				sum = sum + (s[i] - '0') * exp;
				exp = exp/10;
			}
		}
		i++;
	}
	return sum;
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
 *Ϊbuf1�е�i����ʼ���ַ���(�Կո�����������Ǹ�������������һ�����ţ�ȷ�����ھ����еĳɷ�
 *����һ�㲻�����ո�,Ŀǰlabel��42��scanner�ܲ����ĵ�41
 */
static word scanner(int k)
{
	word w;
	int i = 0,j;
	long numl = 0;
	long double numd = 0;
	int flag;
	char str[256];

	w.label = 0;
	w.length = 1;
	w.offset = k;
	w.value.valued = 0;//��value��Ķ�����0

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
//////////////////////////////////////////////////////////////////////
	if(buf1[k].ch == 'a' && buf1[k+1].ch == 'n' && buf1[k+2].ch == 's' && 
		!isalnum(buf1[k+3].ch) && buf1[k+3].ch != '_')
	{
		w.label = 37;
		w.length = 3;
		return w;
	}

	else if (buf1[i + k].ch == '_' || isalpha(buf1[i + k].ch))
	{
		while(isalnum(buf1[i + k].ch) || buf1[i + k].ch == '_')
		{
			i++;
		}
		w.label = 38;
		w.length = i;
		return w;
	}
	else if (isdigit(buf1[i + k].ch))//����������0λ��ͷ�����������ԣ��������������ʱ���.���һλ��ʼ����,��atof����
	{
		j = 0;
		numd = 0;
		numl = 0;
		if (buf1[i + k].ch == '0')
		{
			if (buf1[i + k + 1].ch == '.')
			{
				flag = 1;
				i = i+2;//ʹָ��.���һλ������Ҫ��С�����ֽ��д���
			}
			else
			{
				if (buf1[i + k +1].ch == ' ')
				{
					w.label = 39;
					w.value.valuel = 0;
					w.length = 1;
					return w;
				}
				//Error
				printf("illeagle num\n");
				w.label = 0;
				return w;
			}
		}
		else//����0��ͷ
		{
			flag = 0;//�ȹ���Ϊlong��
			while(isdigit(buf1[i + k].ch))
			{
				str[j++] = buf1[i + k].ch;
				if (flag == 0)
				{
					numd = numd * 10 + buf1[i + k].ch - '0';
					numl = numl * 10 + buf1[i + k].ch - '0';
					if (fabs(numd - numl)> DBL_EPSILON)
					{
						flag = 1;
					}
				}
				i++;
			}
			if (buf1[i+k].ch !=' ' && buf1[i+k].ch !='%' && buf1[i+k].ch !='^' && buf1[i+k].ch !='*' 
				&& buf1[i+k].ch !=')' && buf1[i+k].ch !='-' && buf1[i+k].ch !='+' && buf1[i+k].ch !=']' 
				&& buf1[i+k].ch !=',' && buf1[i+k].ch !=';' && buf1[i+k].ch !=':' && buf1[i+k].ch !='.'
				&& buf1[i+k].ch != 0)
			{
				//Error
				printf("suppose no digit\n");
				w.label = 0;
				return w;
			}
			if (buf1[i + k].ch != '.')
			{
				if (flag == 0)
				{
					w.label = 39;
					w.length = i;
					w.value.valuel = numl;
				}
				else
				{
					w.label = 40;
					w.length = i;
					w.value.valued = numd;
				}
				return w;
			}
			flag = 1;
			i = i+1;//ʹָ��.���һλ������Ҫ��С�����ִ���
		}//��������������������ʵ��������i+kΪС���㣬str���ѱ�����������
		str[j++] = '.';
		while (isdigit(buf1[i + k].ch))
		{
			str[j++] = buf1[i+k].ch;
			i++;
		}
		str[j] = 0;
		if (buf1[i+k].ch !=' ' && buf1[i+k].ch !='%' && buf1[i+k].ch !='^' && buf1[i+k].ch !='*' 
				&& buf1[i+k].ch !=')' && buf1[i+k].ch !='-' && buf1[i+k].ch !='+' && buf1[i+k].ch !=']' 
				&& buf1[i+k].ch !=',' && buf1[i+k].ch !=';' && buf1[i+k].ch !=':' && buf1[i+k].ch !='.'
				&& buf1[i+k].ch != 0)
		{
			//Error
			printf("suppose no digit\n");
			w.label = 0;
			return w;
		}
		w.label = 40;
		w.length = i;
		w.value.valued = a2f(str);
		return w;
	}
	//Error
	printf("unknown word");
	w.label = 0;
	return w;
}

/*
 *ȥ��ǰ��ո�\n�����м��������\nת��Ϊ��,�������Ϊ]��ת��
 *buf��buf1
 *û�г���
 */
static int format0(void)
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
			if (*p != ']')
			{
				i++;
			}
		}
	}
    i--;
	buf1[i].ch = 0;
	i--;
	while(buf1[i].ch == ' ')
	{
		buf1[i--].ch = 0;
	}
	return 0;
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
				str[0] = '.';
				for (l = 0; l<buf2[i+1].length; l++)
				{
					str[l + 1] = buf1[l + buf2[i+1].offset].ch;
				}
				str[l+1] = 0;
				buf2[j].label = 40;
				buf2[j].length = buf2[i+1].length + 1;
				buf2[j].offset = buf2[i].offset;
				buf2[j].value.valued = a2f(str);
				j++;
				i = i+2;
			}
			else if(buf2[i].label == 4 && buf2[i+1].label == 14)//.^
			{
				buf2[j].label = 42;
				buf2[j].offset = buf2[i].offset;
				buf2[j].length = buf2[i].length;
				buf2[j++].value = buf2[i].value;
				i = i+2;
			}
			else
			{
				buf2[j].label = buf2[i].label;
				buf2[j].offset = buf2[i].offset;
				buf2[j].length = buf2[i].length;
				buf2[j++].value = buf2[i++].value;
			}
		}
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
			buf2[j].length = 1;//��Ϊ�������
			buf2[j++].value.valued = 0;
		}
	}
	buf2[j].label = 0;
	return 0;
}

/*
 *�������˶��ŵĴ�����
 */
static int comma(void)
{
	long a, b, c;
	Matrix *temp = NULL;
	int i;

	if (stack2[sp2].label == 1)//:
	{
		if (stack1[sp1].label != 39)
		{
			//Error
			return stack1[sp1].offset;
		}
		if (stack1[sp1-1].label != 39)
		{
			//Error
			return stack1[sp1-1].offset;
		}
		if (stack2[sp2-1].label == 1)//��
		{
			if (stack1[sp1-2].label != 39)
			{
				//Error
				return stack1[sp1-2].offset;
			}
			a = stack1[sp1-2].value.valuel;
			b = stack1[sp1-1].value.valuel;
			c = stack1[sp1].value.valuel;
			if (((c-a)/b)*b!=(c-a))
			{
				//Error
				return stack1[sp1-2].offset;
			}
			temp = stor_createMatrix(temp, 1, (c-a)/b+1,0);
			for (i = 0; i<(c-a)/b+1; i++)
			{
				*(long *)stor_entry(temp,0,i) = b*i+a;
			}
			stack1[sp1-2].label = 47;
			stack1[sp1-2].value.matrix = temp;
			sp1 = sp1-2;
			sp2 = sp2-2;
		}//3����������
		else
		{
			a = stack1[sp1-1].value.valuel;
			b = stack1[sp1].value.valuel;
			temp = stor_createMatrix(temp, 1, b-a+1,0);
			for (i = 0; i<(b-a)+1; i++)
			{
				*(long *)stor_entry(temp, 0, i) = a+i;
			}
			stack1[sp1-1].label = 47;
			stack1[sp1-1].value.matrix = temp;
			sp1--;
			sp2--;
		}//������������
	}//�ԣ��������

	if (!isMatrix(stack1[sp1].label))
	{
		if (stack1[sp1].label == 39)//����
		{
			stack1[sp1].label = 47;
			temp = stor_createMatrix(temp, 1,1,0);
			*(long *)stor_entry(temp,0,0) = stack1[sp1].value.valuel;
			stack1[sp1].value.matrix = temp;
		}
		else if (stack1[sp1].label == 40)//������
		{
			stack1[sp1].label = 47;
			temp = stor_createMatrix(temp, 1, 1, 1);
			*(long double *)stor_entry(temp, 0, 0) = stack1[sp1].value.valued;
			stack1[sp1].value.matrix = temp;
		}
		else
		{
			//Error
			return stack1[sp1].offset;
		}
	}

	if (stack1[sp1-1].label==0)
	{
		return 0;//��������������
	}
	else
	{
		if (!isMatrix(stack1[sp1-1].label))
		{
			if (stack1[sp1-1].label == 39)//����
			{
				stack1[sp1-1].label = 47;
				stor_createMatrix(temp, 1,1,0);
				*(long *)stor_entry(temp,0,0) = stack1[sp1-1].value.valuel;
				stack1[sp1-1].value.matrix = temp;
			}
			else if (stack1[sp1-1].label == 40)//������
			{
				stack1[sp1-1].label = 47;
				stor_createMatrix(temp, 1, 1, 1);
				*(long double *)stor_entry(temp, 0, 0) = stack1[sp1-1].value.valued;
				stack1[sp1-1].value.matrix = temp;
			}
			else
			{
				//Error
				return stack1[sp1-1].offset;
			}
		}
		temp = stor_mergeColum(stack1[sp1-1].value.matrix, stack1[sp1].value.matrix);
		if (stack1[sp1].label != 37 && stack1[sp1].label != 38)//����ans���ʶ��
		{
			free(stack1[sp1].value.matrix->pd);
			free(stack1[sp1].value.matrix->pl);
			free(stack1[sp1].value.matrix);
		}
		if (stack1[sp1-1].label != 37 && stack1[sp1-1].label != 38)//����ans���ʶ��
		{
			free(stack1[sp1-1].value.matrix->pd);
			free(stack1[sp1-1].value.matrix->pl);
			free(stack1[sp1-1].value.matrix);
		}
		stack1[sp1-1].label = 47;
		stack1[sp1-1].value.matrix = temp;
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
	if (sp1-3>0 && stack1[sp1-3].label == 0)
	{
		if (!isMatrix(stack1[sp1-2].label))
		{
			//Error
			return stack1[sp1-2].offset;
		}
		temp = stor_mergeRow(stack1[sp1-2].value.matrix, stack1[sp1].value.matrix);
		if (stack1[sp1].label != 37 && stack1[sp1].label != 38)//����ans���ʶ��
		{
			free(stack1[sp1].value.matrix->pd);
			free(stack1[sp1].value.matrix->pl);
			free(stack1[sp1].value.matrix);
		}
		if (stack1[sp1-2].label != 37 && stack1[sp1-2].label != 38)//����ans���ʶ��
		{
			free(stack1[sp1-2].value.matrix->pd);
			free(stack1[sp1-2].value.matrix->pl);
			free(stack1[sp1-2].value.matrix);
		}
		stack1[sp1-2].label = 47;
		stack1[sp1-2].value.matrix = temp;
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
 *������
 */
static int pop(int prefer)
{
	int n = 1;//�����м�������
	long l1, l2;//�������
	long double d1, d2;//�������
	Matrix *m1, *m2;//�������
	word temp;
	int temp1[10000] = {0}, temp2[10000] = {0};
	char str[256];
	int i,j;

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
							if (n == 1)//1������
							{
								l1 = stack1[sp1].value.valuel;
								if (!isPInteger(stack1[sp1]))
								{
									//Error
									return stack1[sp1].offset;
								}
								if (stack2[sp2].label == 20)
									rand_Matrix(l1,l1);
								else zeros_Matrix(l1,l1);
								stack1[sp1].label = 47;
								free(stack1[sp1].value.matrix);
								stack1[sp1].value.matrix = stor_createMatrix(stack1[sp1].value.matrix,l1,l1,1);
								stor_assign(stack1[sp1].value.matrix, stor_ans(0));
								break;
							}
							else if (n == 2)
							{
								l2 = stack1[sp1].value.valuel;
								if (!isPInteger(stack1[sp1]))
								{
									//Error
									return stack1[sp1].offset;
								}
								l1 = stack1[sp1 - 1].value.valuel;
								if (!isPInteger(stack1[sp1-1]))
								{
									//Error
									return stack1[sp1-1].offset;
								}
								if (stack2[sp2].label == 20)
									rand_Matrix(l1, l2);
								else zeros_Matrix(l1, l2);
								sp1--;
								stack1[sp1].label = 47;
								free(stack1[sp1].value.matrix);
								stack1[sp1].value.matrix = stor_createMatrix(stack1[sp1].value.matrix, l1, l2, 1);
								stor_assign(stack1[sp1].value.matrix, stor_ans(0));
								break;
							}
							sp2--;
							break;
						case 22://eye
							if (n > 1)
							{
								//Error
								return stack2[sp2].offset;
							}
							l1 = stack1[sp1].value.valuel;
							if (!isPInteger(stack1[sp1]))
							{
								//Error
								return stack1[sp1].offset;
							}
							eye_Matrix(l1);
							stack1[sp1].label = 47;
							free(stack1[sp1].value.matrix);
							stack1[sp1].value.matrix = stor_createMatrix(stack1[sp1].value.matrix,l1,l1,0);
							stor_assign(stack1[sp1].value.matrix, stor_ans(0));
							sp2--;
							break;
						case 23: case 24: case 25://sum,max,min
							if (n > 1)
							{
								//Error
								return stack2[sp2].offset;
							}
							if (!isMatrix(stack1[sp1].label))
							{
								//Error
								return stack1[sp1].offset;
							}
							m1 = stack1[sp1].value.matrix;
							if ((stor_type(m1) & 1) == 0)//long
							{
								if (stack2[sp2].label == 23)
									stack1[sp1].value.valuel = (long)sum_Matrix(m1);
								else if (stack2[sp2].label == 24)
									stack1[sp1].value.valuel = (long)max_Matrix(m1);
								else stack1[sp1].value.valuel = (long)min_Matrix(m1);
								if (stack1[sp1].label == 44 || stack1[sp1].label == 47)//�������Ҫ���ͷ�
								{
									free(m1->pl);
									free(m1->pd);
									free(m1);
								}
								stack1[sp1].label = 39;
								stor_createAns(0,1,1,0);
								*(long*)stor_ansEntry(0,0,0) = stack1[sp1].value.valuel;
							}
							else//long double
							{
								if (stack2[sp2].label == 23)
									stack1[sp1].value.valuel = (long)sum_Matrix(m1);
								else if (stack2[sp2].label == 24)
									stack1[sp1].value.valuel = (long)max_Matrix(m1);
								else stack1[sp1].value.valuel = (long)min_Matrix(m1);
								if (stack1[sp1].label == 44 || stack1[sp1].label == 47)//�������Ҫ���ͷ�
								{
									free(m1->pl);
									free(m1->pd);
									free(m1);
								}
								stack1[sp1].label = 40;
								stor_createAns(0,1,1,1);
								*(long double *)stor_ansEntry(0,0,0) = stack1[sp1].value.valued;
							}
							sp2--;
							break;
						case 26: case 27: case 28:case 29:case 30:case 36://round,upper,lower,reverse,rref,feature
							if (n > 1)
							{
								//Error
								return stack2[sp2].offset;
							}
							if (!isMatrix(stack1[sp1].label))
							{
								//Error
								return stack1[sp1].offset;
							}
							m1 = stack1[sp1].value.matrix;
							if (stack2[sp2].label == 26)
								round_Matrix(m1);
							else if (stack2[sp2].label == 27)
								upper_Matrix(m1);
							else if (stack2[sp2].label == 28)
								lower_Matrix(m1);
							else if (stack2[sp2].label == 29)
								reverse_Matrix(m1);
							else if (stack2[sp2].label == 30)
								rref_Matrix(m1);
							else if (stack2[sp2].label == 36)
								feature_value_Matrix(m1);
							if (stack1[sp1].label == 44 || stack1[sp1].label == 47)//�������Ҫ���ͷ�
							{
								free(m1->pl);
								free(m1->pd);
								free(m1);
							}
							stack1[sp1].label = 47;
							free(stack1[sp1].value.matrix);
							stack1[sp1].value.matrix = stor_createMatrix(stack1[sp1].value.matrix, stor_ans(0)->m, stor_ans(1)->n,stor_type(stor_ans(0)));
							stor_assign(stack1[sp1].value.matrix, stor_ans(0));
							sp2--;
							break;
						case 31: case 32:case 34://det,rank,norm
							if (n > 1)
							{
								//Error
								return stack2[sp2].offset;
							}
							if (!isMatrix(stack1[sp1].label))
							{
								//Error
								return stack1[sp1].offset;
							}
							m1 = stack1[sp1].value.matrix;
							if (stack2[sp2].label == 32)
							{
								temp = stack1[sp1];
								temp.label = 39;
								temp.value.valuel = rank_Matrix(m1);
								stor_createAns(0,1,1,0);
								*(long *)stor_ansEntry(0,0,0) = temp.value.valuel;
							}
							else if (stack2[sp2].label == 31)
							{
								if ((stor_type(m1)&1) == 0)//long
								{
									temp = stack1[sp1];
									temp.label = 39;
									temp.value.valuel = (long)det_Matrix(m1);
									stor_createAns(0,1,1,0);
									*(long *)stor_ansEntry(0,0,0) = temp.value.valuel;
								}
								else
								{
									temp = stack1[sp1];
									temp.value.valued = det_Matrix(m1);
									stor_createAns(0,1,1,0);
									*(long double *)stor_ansEntry(0,0,0) = temp.value.valued;
								}
							}
							else
							{
								temp = stack1[sp1];
								temp.label = 49;
								temp.value.valued = norm_Matrix(m1);
								stor_createAns(0,1,1,1);
								*(long double*)stor_ansEntry(0,0,0) = temp.value.valued;
							}
							if (stack1[sp1].label == 44 || stack1[sp1].label == 47)//�������Ҫ���ͷ�
							{
								free(m1->pl);
								free(m1->pd);
								free(m1);
							}
							stack1[sp1] = temp;
							sp2--;
							break;
						case 33:case 35:
							if (n != 2)
							{
								//Error
								return stack2[sp2].offset;
							}
							//�����������
							m2 = stack1[sp1].value.matrix;
							m1 = stack1[sp1-1].value.matrix;
							temp = stack1[sp1-1];
							temp.label = 40;
							if (stack2[sp2].label == 33)
								temp.value.valued = dot_Matrix(m1,m2);
							else temp.value.valued = angle_Matrix(m1, m2);
							if (stack1[sp1].label == 44 || stack1[sp1].label == 47)//�������Ҫ���ͷ�
							{
								free(m2->pl);
								free(m2->pd);
								free(m2);
							}
							if (stack1[sp1-1].label == 44 || stack1[sp1-1].label == 47)//�������Ҫ���ͷ�
							{
								free(m1->pl);
								free(m1->pd);
								free(m1);
							}
							sp1--;
							sp2--;
							break;
						case 43://ȡ�Ӿ�����
							if (n != 2)
							{
								//Error
								return stack2[sp2].offset;
							}
							if (!isMatrix(stack1[sp1].label)&&stack1[sp1].label != 46)
							{
								//Error
								return stack1[sp1].offset;
							}
							if (!isMatrix(stack1[sp1-1].label)&&stack1[sp1-1].label != 46)
							{
								//Error
								return stack1[sp1-1].offset;
							}
							if (stack1[sp1].label == 46)
							{
								l1 = stack2[sp2].value.matrix->m;
								for (i = 0; i<stack2[sp2].value.matrix->m; i++)
								{
									temp1[i] = i+1;
								}
							}
							else
							{
								if (stack1[sp1].value.matrix->m != 1 || stor_type(stack1[sp1].value.matrix))
								{
									//Error
									return stack1[sp1].offset;
								}
								l1 = stack1[sp1].value.matrix->n;
								for (i = 0; i<stack1[sp1].value.matrix->n; i++)
								{
									temp1[i] = stack1[sp1].value.matrix->pl[0][i];
								}
								util_sort(temp1,0,stack1[sp1].value.matrix->n-1);
							}

							if (stack1[sp1-1].label == 46)
							{
								l2 = stack2[sp2].value.matrix->n;
								for (i = 0; i<stack2[sp2].value.matrix->n; i++)
								{
									temp2[i] = i+1;
								}
							}
							else
							{
								if (stack1[sp1-1].value.matrix->m != 1 || stor_type(stack1[sp1-1].value.matrix))
								{
									//Error
									return stack1[sp1-1].offset;
								}
								l2 = stack1[sp1-1].value.matrix->n;
								for (i = 0; i<stack1[sp1-1].value.matrix->n; i++)
								{
									temp2[i] = stack1[sp1-1].value.matrix->pl[0][i];
								}
								util_sort(temp2,0,stack1[sp1-1].value.matrix->n-1);
							}
							temp = stack2[sp2];
							temp.value.matrix = stor_subMatrix(stack2[sp2].value.matrix, l1, l2, temp1, temp2);
							temp.label = 44;
							temp.m = (int*)malloc(sizeof(int)*l1);
							temp.n = (int*)malloc(sizeof(int)*l2);
							memcpy(temp.m, temp1, l1*sizeof(int));
							memcpy(temp.n, temp2, l2*sizeof(int));
							temp.bigMatrix = stack2[sp2].value.matrix;
							if (stack1[sp1].label == 44 || stack1[sp1].label == 47)//�������Ҫ���ͷ�
							{
								free(stack1[sp1].value.matrix->pl);
								free(stack1[sp1].value.matrix->pd);
								free(stack1[sp1].value.matrix);
							}
							if (stack1[sp1-1].label == 44 || stack1[sp1-1].label == 47)//�������Ҫ���ͷ�
							{
								free(stack1[sp1-1].value.matrix->pl);
								free(stack1[sp1-1].value.matrix->pd);
								free(stack1[sp1-1].value.matrix);
							}
							sp1--;
							stack1[sp1] = temp;
							sp2--;
							break;
						default:
							//Error
							return -1;
					}//switch ����
				}//�����������
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

		if (stack2[sp2].label == 7)//=
		{
			if (!isMatrix(stack1[sp1-1].label))
			{
				//Error
				return stack1[sp1-1].offset;
			}
			if (!isMatrix(stack1[sp1].label))
			{
				//Error
				return stack1[sp1].offset;
			}
			if (stack1[sp1-1].label == 44)//�Ӿ���
			{
				if (stack1[sp1-1].value.matrix->m != stack1[sp1].value.matrix->m
					||stack1[sp1-1].value.matrix->n != stack1[sp1].value.matrix->n)
				{
					//Error
					return stack1[sp1].offset;
				}
				stor_assign(stack1[sp1-1].value.matrix, stack1[sp1].value.matrix);
				stor_assignSubMatrix(stack1[sp1-1].bigMatrix, stack1[sp1-1].value.matrix
					, sizeof(stack1[sp1-1].m)/sizeof(int),sizeof(stack1[sp1-1].n)/sizeof(int)
					,stack1[sp1-1].m,stack1[sp1-1].n);
			}
			else
			{
				for (i = 0; i<stack1[sp1-1].length; i++)
				{
					str[i] = buf1[stack1[sp1-1].offset+i].ch;
				}
				str[i] = 0;
				stor_assign(stor_create(str, stack1[sp1].value.matrix->m, stack1[sp1].value.matrix->n
					, stor_type(stack1[sp1].value.matrix)), stack1[sp1].value.matrix);
			}
			sp2--;
			sp1--;
		}
	}
}

/*
 *���ݴʷ�������Ӧ�ļ�����ж�
 */
static int parser(void)
{
	int i,err;
	Matrix *temp = NULL;

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
			if (buf2[i+1].label != 16)
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
			stack1[sp1] = buf2[i];
			if (buf2[i].label == 37)//ans
			{
				stack1[sp1].value.matrix = stor_ans(0);
			}
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
				stack2[sp2].value.matrix = getMatrix(i);
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
				stack1[sp1] = buf2[i];
				stack1[sp1].value.matrix = getMatrix(i);
				flagn = 1;
				i++;
			}
		}
		else if ((buf2[i].label >=7 && buf2[i].label <= 14 )
			||buf2[i].label == 42)//�����ȼ��������
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
		else if (buf2[i].label == 1)//:
		{
			if (flagn == 1)
			{
				if (err = pop(preference2(buf2[i].label)))
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
				reverse_Matrix(stack1[sp1].value.matrix);
				temp = stor_createMatrix(temp, stack1[sp1].value.matrix->n, stack1[sp1].value.matrix->m, stor_type(stack1[sp1].value.matrix) &1);
				stack1[sp1].label = 47;
				stack1[sp1].offset = stack1[sp1].offset;
				stack1[sp1].length = 0;
				stack1[sp1].value.matrix = temp;
				flagn = 1;
				i++;
			}
		}//'����
		else if (buf2[i].label == 16)//)
		{
			if (flagn == 0)
			{
				//Error
				return buf2[i].offset;
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
			stack1[sp1].length = 0;
			stack1[sp1].value.valued = 0;
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
			stack1[sp1 - 2] = stack1[sp1 - 1];
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
				sp2++;
				stack2[sp2] = buf2[i];
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
	//��ӡ���
	return 0;
}
		
/*
 *�����ָ���ѷ���buf��
 *û�д��󷵻�0���д���1
 */
int com_interpret(void)
{
	int i, j;
	char str[256];
	int flag = 0;//��¼��һ����ѹ��ջ���ǲ���
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
			if (fpin != NULL)
			{
				if (fclose(fpin))
				{
					//Error
					return 1;
				}
			}
			if((fpin = fopen(str, "r")) == NULL)
			{
				//Error
				return 1;
			}
			strcpy(filein, str);
			uni_flag |= 1;
		}
		else
		{
			if (fpout != NULL)
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
				uni_flag &= ~(1<<1);
				return 0;
			}
			if ((fpout = fopen(str, "a")) == NULL)
			{
				//Error
				return 1;
			}
			strcpy(fileout, str);
			uni_flag |= 1<<1;
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
	return 0;
}
