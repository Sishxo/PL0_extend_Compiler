// pl/0 compiler with code generation
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
//#include <fstream.h>
#include "pl0.h"
#include<windows.h>//控制Dos界面
HANDLE hout;//控制台句柄
void color(int c)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),c);
}


void error(long n)
{
	long i;

	errornum++;
	color(4);
	printf("error ");
	for (i = 1; i <= cc - 1; i++)
	{
		printf(" ");
	}
	printf("^%s\n",wrongmsg[n]);
	color(15);
	err++;
}

void getch()
{ //输出源程序
	if (cc == ll)
	{
		if (feof(infile))
		{
			printf("************************************\n");
			printf("      program incomplete\n");
			printf("************************************\n");
			exit(1);
		}
		ll = 0;
		cc = 0;
		printf("%5d ", cx);
		while ((!feof(infile)) && ((ch = getc(infile)) != '\n'))
		{ //输出一行原代码
			printf("%c", ch);
			ll = ll + 1;
			line[ll] = ch;
		}
		printf("\n");
		ll = ll + 1;
		line[ll] = ' ';
	}
	cc = cc + 1;
	ch = line[cc];
}

void getsym()
{
	long i, j, k;
	int flag = 0;
	double t = 1.0;			//标记小数点后的位数
	double numt = 0, z = 0; //小数部分的值
	while (ch == ' ' || ch == '\t')
	{
		getch();
	}
	if (isalpha(ch))
	{ // identified or reserved  开始进行词法分析
		k = 0;
		do
		{
			if (k < al)
			{
				a[k] = ch;
				k = k + 1;
			}
			getch();
		} while (isalpha(ch) || isdigit(ch));
		if (k >= kk)
		{
			kk = k;
		}
		else
		{
			do
			{
				kk = kk - 1;
				a[kk] = ' ';
			} while (k < kk);
		}
		strcpy(id, a);
		i = 0;
		j = norw - 1; //把符号串（如var）放入id中
		do
		{
			k = (i + j) / 2;
			if (strcmp(id, word[k]) <= 0)
			{
				j = k - 1;
			}
			if (strcmp(id, word[k]) >= 0)
			{
				i = k + 1;
			}
		} while (i <= j); //此循环用于在保留字符表中找到对应的保留字
		if (i - 1 > j)
		{
			sym = wsym[k]; //  sym返回词法单元的类型 如果i-1》j是保留字
		}
		else
		{
			sym = ident; //否则是定义的标识符
		}
	}
	else if (isdigit(ch))
	{ // number
		k = 0;
		num = 0;
		do
		{
			num = num * 10 + (ch - '0');
			k = k + 1;
			getch();
		} while (isdigit(ch));
		if (ch == '.')
		{ //遇到小数点说明是实型数据
			getch();
			if (isdigit(ch))
			{
				while (isdigit(ch))
				{
					t /= 10;
					num = num + (ch - '0') * t;
					k++;
					getch();
				}
				sym = realsym;
			}
			else if (ch == '.')
			{ //又遇到一个点说明是数组的定义
				sym = intersym;
				cc = cc - 2;
			}
			else
			{
				error(57);
			}
		}
		else
		{
			sym = intersym;
		}
		if (k > nmax)
		{
			error(31);
		}
	}
	else if (ch == ':')
	{
		getch();
		if (ch == '=')
		{
			sym = becomes;
			getch();
		}
		else
		{
			sym = nul;
		}
	}
	else if (ch == '/')
	{ //新增
		getch();
		if (ch == '*')
		{
			flag++;
			getch();
			while (flag > 0)
			{
				while (ch != '*')
				{
					getch();
				}
				getch();
				if (ch == '/')
					flag--;
			}
			getch();
			getsym();
		}
		else
		{
			sym = ssym[(unsigned char)'/'];
		}
	}
	else if (ch == '*')
	{
		getch();
		if (ch == '/')
		{
			getch();
			if (ch == '*')
			{
				flag = 0;
				sym = ssym[(unsigned char)'*'];
				flag++;
				getch();
				while (flag > 0)
				{
					while (ch != '*')
					{
						getch();
					}
					getch();
					if (ch == '/')
						flag--;
				}
				getch();
			}
			else
			{
				printf("a superflous note symbol ");
				sym = nul;
			}
		}
		else
		{
			sym = ssym[(unsigned char)'*'];
		}
	}
	else if (ch == '<')
	{
		getch();
		if (ch == '=')
		{
			sym = leq;
			getch();
		}
		else if (ch == '>')
		{
			sym = neq;
			getch();
		}
		else
		{
			sym = lss;
		}
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			sym = geq;
			getch();
		}
		else
		{
			sym = gtr;
		}
	}
	else if (ch == '=')
	{
		getch();
		if (ch == '=')
		{
			getch();
		}
		sym = eql;
	}
	else if (ch == '.')
	{
		getch();
		if (ch == '.')
		{
			sym = dotdot;
			getch();
		}
		else
		{
			sym = period;
		}
	}
	else
	{
		sym = ssym[(unsigned char)ch];
		getch();
	}
}

void gen(enum fct x, long y, double z)
{
	if (cx > cxmax)
	{
		printf("program too long\n");
		exit(1);
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx].a = z;
	cx = cx + 1;
}

void test(__int64 s1, __int64 s2, long n)
{
	if (!(sym & s1))
	{
		error(n);
		s1 = s1 | s2;
		while (!(sym & s1))
		{
			getsym();
		}
	}
}

void enter(enum object k)
{ // enter object into table
	tx = tx + 1;
	strcpy(table[tx].name, id);
	table[tx].kind = k;
	switch (k)
	{
	case constant:
		if (num > amax)
		{
			error(31);
			num = 0;
		}
		table[tx].val = num;
		table[tx].type1 = sym; //记录下常量是实型还是整型
		break;
	case variable:
		table[tx].level = lev;
		table[tx].addr = dx;
		dx = dx + 1;
		break;
	case proc:
		table[tx].level = lev;
		dx = dx + 1;
		break;
	case type:
		table[tx].level = lev; /*table[tx].addr=dx; dx=dx+1;*/
		table[tx].type1 = sym;
		break;
	case func:
		table[tx].level = lev;
		table[tx].funcaddr = dx;
		dx = dx + 1;
		break;
	}
}

long position(char *id)
{ // find identifier id in table
	long i;

	strcpy(table[0].name, id);
	i = tx;
	while (strcmp(table[i].name, id) != 0)
	{
		i = i - 1;
	}
	return i;
}

void constdeclaration()
{
	if (sym == ident)
	{
		getsym(); //取得const常量的下一个词的类型
		if (sym == eql || sym == becomes)
		{
			if (sym == becomes)
			{
				error(1);
			}
			getsym();
			if (sym == intersym || sym == realsym)
			{
				enter(constant);
				getsym();
			}
			else
			{
				error(2);
			}
		}
		else
		{
			error(3);
		}
	}
	else
	{
		error(4);
	}
}

void vardeclaration()
{
	long i;
	int identnum = 0;		 //同类型的变量个数
	char sameid[10][al + 1]; //存放同类型的变量的名字
	long size;
	int j;
	int k = 0;
	int t;
	if (sym == ident)
	{
		identnum++;
		strcpy(sameid[k], id);
		getsym();
		while (sym == comma)
		{
			getsym();
			if (sym == ident)
			{
				for (j = 0; j < identnum; j++)
				{
					if (strcmp(sameid[j], id) == 0)
					{
						error(10);
						break;
					}
				}
				identnum++;
				k++;
				strcpy(sameid[k], id);
				getsym();
			}
		}
		if (sym == nul)
		{ //说明遇到：后面应该是指示变量的类型
			getsym();
			if (sym == intersym || sym == realsym || sym == Boolsym)
			{
				for (j = 0, k = 0; j < identnum; j++)
				{ //之前的变量全部是相同的类型
					strcpy(id, sameid[k]);
					/*if((i=position(id))!=0){   //检查变量是否重复声明
					  error(48);

					}*/
					enter(variable);
					//tx1++;
					table[tx].type1 = sym;

					k++;
				}
			}
			else if (sym == ident)
			{
				i = position(id); //在符号表中找到Id的位置 返给下表付给i
				if (i == 0)
				{
					error(11);
				}
				sym = table[i].type1;
				if (sym == intersym || sym == realsym || sym == Boolsym)
				{
					for (j = 0, k = 0; j < identnum; j++)
					{ //之前的变量全部是相同的类型
						strcpy(id, sameid[k]);
						enter(variable);
						//tx1++;
						k++;
						table[tx].type1 = sym;
					}
				}
				else if (sym == arraysym)
				{
					for (j = 0; j < identnum; j++)
					{ //之前的变量全部是相同的类型
						strcpy(id, sameid[j]);
						enter(variable);
						table[tx].type1 = arraysym;
						table[tx].drt = table[i].drt;
						table[tx].size = table[i].size;
						size = table[tx].size;
						for (t = 1; t <= table[tx].drt; t++)
						{
							table[tx].low[t] = table[i].low[t];
							table[tx].high[t] = table[i].high[t];
						}
						sym = table[i].type2;
						table[tx].type2 = sym;
						dx = dx + 2 * table[i].drt + 1; //存放上下界的空间
						dx += size;
					}
				}
			}
			else
			{
				error(12);
			}
			getsym();
		}
		else
		{
			error(13);
		}
	}
	else
	{
		error(14);
	}
}

void typeexpression()
{
	__int64 sym1;
	long num3, num4, size; //记录数组的上下界以及大小
	long tx1;
	int drtnum = 1;
	int j;
	if (sym == intersym || sym == realsym || sym == Boolsym)
	{
		enter(type);
		getsym();
	}
	else if (sym == arraysym)
	{

		sym1 = sym;
		getsym();
		if (sym == lmparen)
		{
			getsym();
			if (sym == intersym)
			{
				num3 = num;
				//getsym();
				getch();
				getsym();
				if (sym == dotdot)
				{
					getsym();
					sym = sym1;
					num4 = num;
					size = num4 - num3 + 1;
					enter(type);
					table[tx].low[drtnum] = num3;
					table[tx].high[drtnum] = num4;
					table[tx].drt++;
					getsym(); //取得右中括号
					getsym(); //取得of
					if (sym == ofsym)
					{
						getsym();
						//tx1=tx;
						//typeexpression();
						if (sym == intersym || sym == realsym || sym == Boolsym)
						{
							table[tx].type2 = sym;
							table[tx].drt = 1;
							table[tx].size = size;
							/* for(j=1;j<size;j++){//开辟数组空间
								   tx++;
								   table[tx].type2=sym;
								   table[tx].addr=dx;dx++;
								}*/
						}
						else
						{
							while (sym == arraysym)
							{
								//typeexpression();
								table[tx].drt++;
								drtnum++;
								getsym();
								if (sym == lmparen)
								{
									getsym();
									table[tx].low[drtnum] = num;
									getch();
									getsym();
									//getch();
									getsym();
									table[tx].high[drtnum] = num;
									getsym();
									getsym();
									getsym();
									size = size * (table[tx].high[drtnum] - table[tx].low[drtnum] + 1);
								}
								else
								{
									error(34);
								}
								table[tx].size = size;
							}
							if (sym == intersym || sym == realsym || sym == Boolsym)
							{
								table[tx].type2 = sym;
							}
						}
						getsym();
					}
					else
					{
						error(34);
					}
				}
				else
				{
					error(34);
				}
			}
			else
			{
				error(34);
			}
		}
		else
		{
			error(34);
		}
	}
	else
		error(33);
}

void typedeclaration()
{
	char id1[al + 1];
	if (sym == ident)
	{
		strcpy(id1, id);
		getsym();
		if (sym == eql || sym == becomes)
		{
			if (sym == becomes)
			{
				error(6);
			}
			getsym();
			strcpy(id, id1);
			typeexpression(); //传递标识符的名字
		}
		else
		{
			error(7);
		}
	}
	else
	{
		error(8);
	}
}

void listcode(long cx0)
{ // list code generated for this block
	long i;
	for (i = cx0; i <= cx - 1; i++)
	{
		printf("%10d%5s%3d%10.5f\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	}
}
void expre(__int64);
void arraydo(enum fct x, int i)
{
	long d, t;
	switch (x)
	{
	case sto:
		gen(lit, 0, table[i].drt);
		gen(sto, lev - table[i].level, table[i].addr + 1);
		for (t = 1, d = 1; t <= table[i].drt; t++)
		{ //产生指令将上下界存入,还有存数组的维数
			d++;
			gen(lit, 0, table[i].low[t]);
			gen(sto, lev - table[i].level, table[i].addr + d);
			d++;
			gen(lit, 0, table[i].high[t]);
			gen(sto, lev - table[i].level, table[i].addr + d);
		}
		gen(say, lev - table[i].level, table[i].addr);
		break;
	case lod:
		gen(lit, 0, table[i].drt);
		gen(sto, lev - table[i].level, table[i].addr + 1);
		for (t = 1, d = 1; t <= table[i].drt; t++)
		{ //产生指令将上下界存入,还有存数组的维数
			d++;
			gen(lit, 0, table[i].low[t]);
			gen(sto, lev - table[i].level, table[i].addr + d);
			d++;
			gen(lit, 0, table[i].high[t]);
			gen(sto, lev - table[i].level, table[i].addr + d);
		}
		gen(lay, lev - table[i].level, table[i].addr);
		break;
	}
}

void factor(__int64 fsys)
{
	long i;
	long drtnum, away;
	long k;
	long j;
	test(facbegsys, fsys, 30);
	while (sym & facbegsys)
	{
		if (sym == ident)
		{
			i = position(id);
			if (i == 0)
			{
				error(31);
			}
			else
			{
				switch (table[i].kind)
				{
				case constant:
					gen(lit, 0, table[i].val);
					lastsym = table[i].type1;
					getsym();
					break;
				case variable:
					if (table[i].type1 == intersym || table[i].type1 == realsym || table[i].type1 == Boolsym)
					{
						gen(lod, lev - table[i].level, table[i].addr);

						lastsym = table[i].type1;
						getsym();
					}
					else
					{
						if (table[i].type1 == arraysym)
						{ //数组类型
							away = 0;
							getsym();
							for (j = 0; j < table[i].drt; j++)
							{
								if (sym == lmparen)
								{
									getsym();
									expre(fsys | rmparen);
									if (lastsym != intersym)
									{
										lastsym = typeerror;
										error(32);
									}
									if (sym == rmparen)
										getsym();
									else
										error(33);
								}
								else
								{
									error(34);
								}
							}
							if (lastsym != typeerror)
							{
								lastsym = table[i].type2;
							}
							arraydo(lod, i);
						}
					}
					break;
				case proc:
					error(35);
					break;
				case func:
					getsym();
					k = 0;
					if (sym == lparen)
					{
						getsym();
						expre(fsys | comma | rparen);
						if (lastsym != table[i].paral[k])
						{
							error(36);
							lastsym = typeerror;
						}
						while (sym == comma)
						{
							k++;
							getsym();
							expre(fsys | comma | rparen);
							if (lastsym != table[i].paral[k])
							{
								error(36);
								lastsym = typeerror;
							}
						}
						if (sym == rparen)
						{
							gen(cal, lev - table[i].level, table[i].addr);
							getsym();
						}
						for (k = 0; k < table[i].n; k++)
						{
							gen(opr, 0, 7); //把实参弹出栈
						}
						gen(lod, lev - table[i].level, table[i].funcaddr);
						if (lastsym != typeerror)
						{
							lastsym = table[i].type1;
						}
					}
					else if (sym == becomes)
					{													   //说明是函数中的返回语句
						gen(lod, lev - table[i].level, table[i].funcaddr); //将函数返回值放入栈顶
						lastsym = table[i].type1;
					}
					else
					{ //无参函数
						gen(cal, lev - table[i].level, table[i].addr);
						gen(lod, lev - table[i].level, table[i].funcaddr);
						lastsym = table[i].type1;
					}

					break;
				}
			}
		}
		else if (sym == intersym || sym == realsym)
		{
			if (num > amax)
			{
				error(37);
				num = 0;
			}
			gen(lit, 0, num);
			lastsym = sym;
			/*if(lastsym==intersym){
				printf("1\n");
			}else if(lastsym==realsym){
				printf("2\n");
			}else{
				printf("3\n");
			}*/
			getsym();
		}
		else if (sym == not )
		{
			getsym();
			factor(fsys);
			gen(opr, 0, 22);
		}
		else if (sym == truesym)
		{
			gen(lit, 0, 1);
			lastsym = Boolsym;
			getsym();
		}
		else if (sym == falsesym)
		{
			gen(lit, 0, 0);
			lastsym = Boolsym;
			getsym();
		}
		else if (sym == lparen)
		{
			getsym();
			expre(rparen | fsys);
			if (sym == rparen)
			{
				getsym();
			}
			else
			{
				error(38);
			}
		}
		test(fsys, lparen, 39);
	}
}

void term(__int64 fsys)
{
	__int64 mulop;
	__int64 lasttype;
	long cx1, cx2;
	factor(fsys | times | slash | div | mod | and);
	lasttype = lastsym;
	while (sym == times || sym == slash || sym == div || sym == mod || sym == and)
	{
		mulop = sym;
		getsym();
		if (mulop == and)
		{
			cx1 = cx;
			gen(jpc, 0, 0);
		}
		factor(fsys | times | slash | div | mod | and);
		if (mulop == times)
		{
			if (lasttype == intersym && lastsym == intersym)
			{
				lastsym = intersym;
			}
			else if (lasttype == intersym && lastsym == realsym)
			{
				lastsym = realsym;
			}
			else if (lasttype == realsym && lastsym == realsym)
			{
				lastsym = realsym;
			}
			else if (lasttype == realsym && lastsym == intersym)
			{
				lastsym = realsym;
			}
			else
			{
				lastsym = typeerror;
				error(40);
			}
			gen(opr, 0, 4);
		}
		else if (mulop == slash)
		{
			if (lasttype == intersym && lastsym == intersym)
			{
				lastsym = intersym;
			}
			else if (lasttype == intersym && lastsym == realsym)
			{
				lastsym = realsym;
			}
			else if (lasttype == realsym && lastsym == realsym)
			{
				lastsym = realsym;
			}
			else if (lasttype == realsym && lastsym == intersym)
			{
				lastsym = realsym;
			}
			else
			{
				lastsym = typeerror;
				error(41);
			}
			gen(opr, 0, 5);
		}
		else if (mulop == div)
		{
			if (lasttype == intersym && lastsym == intersym)
			{
				lastsym = intersym;
			}
			else if (lasttype == intersym && lastsym == realsym)
			{
				lastsym = realsym;
			}
			else if (lasttype == realsym && lastsym == realsym)
			{
				lastsym = realsym;
			}
			else if (lasttype == realsym && lastsym == intersym)
			{
				lastsym = realsym;
			}
			else
			{
				lastsym = typeerror;
				error(42);
			}
			gen(opr, 0, 18);
		}
		else if (mulop == mod)
		{
			if (lasttype == intersym && lastsym == intersym)
			{
				lastsym = intersym;
			}
			else
			{
				lastsym = typeerror;
				error(43);
			}
			gen(opr, 0, 19);
		}
		else if (mulop == and)
		{
			if (lasttype == Boolsym && lastsym == Boolsym)
			{
				lastsym = Boolsym;
			}
			else
			{
				lastsym = typeerror;
				error(44);
			}
			//and短路计算：如果第一个值为0，则不需要判断后面的，直接跳转到factor执行完后的下一条语句，值栈顶值为0；
			gen(opr, 0, 20);
			cx2 = cx;
			gen(jmp, 0, 0);
			code[cx1].a = cx;
			gen(lit, 0, 0);
			code[cx2].a = cx;
		}
	}
}

void simpexp(__int64 fsys)
{
	__int64 addop;
	__int64 lasttype;
	long cx1, cx2;
	if (sym == plus || sym == minus)
	{
		addop = sym;
		getsym();
		term(fsys | plus | minus | or);
		if (addop == minus)
		{
			gen(opr, 0, 1);
		}
	}
	else
	{
		term(fsys | plus | minus | or);
	}
	while (sym == plus || sym == minus || sym == or)
	{
		addop = sym;
		getsym();
		lasttype = lastsym;
		if (addop == or)
		{
			cx1 = cx;
			gen(jpq, 0, 0); //or指令相关的跳转地址
		}
		term(fsys | plus | minus | or);
		if (addop == plus)
		{
			if (lasttype == intersym && lastsym == intersym)
			{
				lastsym = intersym;
			}
			else if (lasttype == intersym && lastsym == realsym)
			{
				lastsym = realsym;
			}
			else if (lasttype == realsym && lastsym == realsym)
			{
				lastsym = realsym;
			}
			else if (lasttype == realsym && lastsym == intersym)
			{
				lastsym = realsym;
			}
			else
			{
				lastsym = typeerror;
				error(45);
			}
			gen(opr, 0, 2);
		}
		else if (addop == minus)
		{
			if (lasttype == intersym && lastsym == intersym)
			{
				lastsym = intersym;
			}
			else if (lasttype == intersym && lastsym == realsym)
			{
				lastsym = realsym;
			}
			else if (lasttype == realsym && lastsym == realsym)
			{
				lastsym = realsym;
			}
			else if (lasttype == realsym && lastsym == intersym)
			{
				lastsym = realsym;
			}
			else
			{
				lastsym = typeerror;
				error(46);
			}
			gen(opr, 0, 3);
		}
		else
		{
			if (lasttype == Boolsym && lastsym == Boolsym)
			{
				lastsym = Boolsym;
			}
			else
			{
				lastsym = typeerror;
				error(47);
			}
			gen(opr, 0, 21);
			cx2 = cx;
			gen(jmp, 0, 0);
			code[cx1].a = cx;
			gen(lit, 0, 1);
			code[cx2].a = cx;
		}
	}
}

void expre(__int64 fsys)
{
	__int64 relop;
	__int64 lasttype;
	if (sym == oddsym)
	{
		getsym();
		simpexp(fsys);
		if (lastsym = intersym)
		{
			lastsym = Boolsym;
		}
		else
		{
			lastsym = typeerror;
			error(48);
		}
		gen(opr, 0, 6);
	}
	else
	{
		simpexp(fsys | eql | neq | lss | gtr | leq | geq | comma | rparen | rmparen);
	}
	if (sym & (eql | neq | lss | gtr | leq | geq))
	{
		relop = sym;
		getsym();
		lasttype = lastsym;
		simpexp(fsys);
		if (lasttype == intersym && lastsym == intersym)
		{
			lastsym = Boolsym;
		}
		else if (lasttype == intersym && lastsym == realsym)
		{
			lastsym = Boolsym;
		}
		else if (lasttype == realsym && lastsym == realsym)
		{
			lastsym = Boolsym;
		}
		else if (lasttype == realsym && lastsym == intersym)
		{
			lastsym = Boolsym;
		}
		else if (lasttype == Boolsym && lastsym == Boolsym)
		{
			lastsym = Boolsym;
		}
		else
		{
			lastsym = typeerror;
			error(49);
		}
		switch (relop)
		{
		case eql:
			gen(opr, 0, 8);
			break;
		case neq:
			gen(opr, 0, 9);
			break;
		case lss:
			gen(opr, 0, 10);
			break;
		case geq:
			gen(opr, 0, 11);
			break;
		case gtr: //大于
			gen(opr, 0, 12);
			break;
		case leq:
			gen(opr, 0, 13);
			break;
		}
	}
}

void statement(__int64 fsys)
{
	__int64 lasttype;
	long i, cx1, cx2;
	long num1[now];
	int drtnum = 0;
	long away = 0; //数组元素的偏移量
	long ii;
	if (sym == ident)
	{
		i = position(id);
		if (table[i].type1 == arraysym)
		{
			getsym();
			for (ii = 0; ii < table[i].drt; ii++)
			{
				if (sym == lmparen)
				{
					getsym();
					expre(fsys | rmparen);
					if (lastsym != intersym)
					{
						error(50);
					}
					lasttype = lastsym;
					if (sym == rmparen)
					{
						getsym();
					}
				}
				else
				{
					error(51);
				}
			}
			lasttype = table[i].type2;
		}
		else
		{
			lasttype = table[i].type1;
			getsym();
		}
		if (sym == becomes)	
		{
			getsym();
		}
		else
		{
			error(13);
		}
		expre(fsys);
		//printf("%x\n",lasttype);
		/*if(lastsym==intersym){
				printf("1\n");
			}else if(lastsym==realsym){
				printf("2\n");
			}else{
				printf("3\n");
			}*/
		if(lastsym!=table[i].type1){
			error(91);
		}
		if (lasttype == intersym && lastsym == intersym)
		{
			lastsym = voiderror;
		}
		else if (lasttype == intersym && lastsym == realsym)
		{
			lastsym = voiderror;
		}
		else if (lasttype == realsym && lastsym == realsym)
		{
			lastsym = voiderror;
		}
		else if (lasttype == realsym && lastsym == intersym)
		{
			lastsym = voiderror;
		}
		else if (lastsym == Boolsym && lasttype == lastsym)
		{
			lastsym = voiderror;
		}
		else
		{
			lastsym = typeerror;
			error(52);
		}
		if (i != 0)
		{
			switch (table[i].kind)
			{
			case variable:
				if (table[i].type1 == intersym || table[i].type1 == realsym || table[i].type1 == Boolsym)
					gen(sto, lev - table[i].level, table[i].addr);
				else if (table[i].type1 = arraysym)
				{
					arraydo(sto, i);
				}
				break;
			case func:
				gen(sto, lev - table[i].level, table[i].funcaddr);
				break;
			case type:
				error(53);
				break;
			}
		}
	}
	else if (sym == callsym)
	{
		int k = 0;
		getsym();
		if (sym != ident)
		{
			error(54);
		}
		else
		{
			i = position(id);
			if (i == 0)
			{
				error(55);
			}
			else
			{
				if (table[i].kind == proc)
				{
					getsym();
					if (sym == lparen) //有参数
					{
						do
						{
							getsym();
							expre(fsys | comma | rparen);
							if(lastsym!=table[i].paral[k]){
								error(36);
							}
							k++;
						} while (sym == comma);
						if (sym == rparen)
						{
							getsym();
						}
						else
						{
							error(56);
						}
						if (k != table[i].n)
						{
							printf("the parameter cannot match\n");
							error(41);
						}
						gen(cal, lev - table[i].level, table[i].addr);
						// gen(opr,0,7);
						for (k = 0; k < table[i].n; k++)
						{
							gen(opr, 0, 7); //把实参弹出栈
						}
						if (sym == funcsym)
							gen(lod, lev - table[i].level, table[i].addr); //把返回值压入栈顶
					}
					else
					{ //没有参数的过程
						gen(cal, lev - table[i].level, table[i].addr);
					}
				}
				else
				{
					error(57);
				}
			}
		}
	}
	else if (sym == ifsym)
	{
		getsym();
		expre(fsys | thensym | dosym);
		if (lastsym != Boolsym)
		{
			lastsym = typeerror;
			error(58);
		}
		if (sym == thensym)
		{
			getsym();
		}
		else
		{
			error(59);
		}
		cx1 = cx;
		gen(jpc, 0, 0);
		statement(fsys | semicolon | endsym | elsesym);

		if (sym == semicolon)
		{
			getsym();
		}
		if (sym == elsesym)
		{
			getsym();
			cx2 = cx;
			gen(jmp, 0, 0);
			code[cx1].a = cx;
			statement(fsys | semicolon | endsym);
			code[cx2].a = cx;
		}
		else
		{
			code[cx1].a = cx;
			statement(fsys | semicolon | endsym);
		}
	}
	else if (sym == elsesym)
	{
		//cx1=cx;gen(jm p,0,0);
		getsym();
		statement(fsys | semicolon | endsym);
		//code[cx2].a=cx;
		//code[cx1].a=cx;
	}
	else if (sym == beginsym)
	{
		getsym();
		statement(fsys | semicolon | endsym);
		while (sym == semicolon || (sym & statbegsys))
		{
			if (sym == semicolon)
			{
				getsym();
			}
			else
			{
				error(60);
			}
			statement(fsys | semicolon | endsym);
		}
		if (sym == endsym)
		{
			getsym();
		}
		else
		{
			error(61);
		}
	}
	else if (sym == whilesym)
	{
		whilenum++;
		cx1 = cx;
		getsym();
		expre(fsys | dosym);
		if (lastsym != Boolsym)
		{
			lastsym = typeerror;
			error(62);
		}
		cx2 = cx;
		gen(jpc, 0, 0);
		if (sym == dosym)
		{
			getsym();
		}
		else
		{
			error(63);
		}
		statement(fsys);
		gen(jmp, 0, cx1);
		code[cx2].a = cx;
		if (exitcx != 0)
		{ //修改exit语句的跳转地址
			code[exitcx].a = cx;
			exitcx = 0;
		}
		whilenum--;
	}
	else if (sym == exitsym)
	{
		if (whilenum == 0)
		{
			error(64);
		}
		else
		{
			exitcx = cx;
			gen(jmp, 0, 0);
		}
		getsym();
	}

	else if (sym == readsym)
	{
		getsym();
		if (sym != lparen)
		{
			error(65);
		}
		else
		{
			do
			{
				getsym();
				if (sym == ident)
					i = position(id);
				else
					i = 0;
				if (i == 0)
					error(66);
				else if (table[i].kind == constant || table[i].kind == proc || table[i].type1 == Boolsym)
				{ // 不能往常量或过程以及布尔类型读入数据
					error(67);
					i = 0;
					getsym();
					continue;
				}
				else
				{
					if (table[i].type1 == intersym || table[i].type1 == realsym)
					{ //说明要读的只是一个简单变量
						getsym();
						gen(opr, 0, 14);
						gen(sto, lev - table[i].level, table[i].addr);
					}
					else
					{
						if (table[i].type1 == arraysym && (table[i].type2 & (intersym | realsym)))
						{ //数组类型
							//drtnum=0;
							//away=0;
							getsym();
							for (ii = 0; ii < table[i].drt; ii++)
							{
								if (sym == lmparen)
								{
									getsym();
									expre(fsys | rmparen);
									if (lastsym != intersym)
									{
										lastsym = typeerror;
										error(69);
									}
									if (sym == rmparen)
									{
										getsym();
									}
								}
								else
								{
									error(70);
								}
							}
							gen(opr, 0, 14);
							arraydo(sto, i);
						}
						else
							error(71);
					}
				}
			} while (sym == comma);
			//gen(opr, 0, 15); //取得换行符
			if (sym == rparen)
				getsym();
		}
	}
	else if (sym == writesym)
	{
		getsym();
		if (sym == lparen)
		{
			do
			{
				getsym();
				expre(fsys | rparen | comma);
				if (lastsym != intersym && lastsym != realsym && lastsym != constsym)
				{
					lastsym = typeerror;
					error(72);
				}
				if (lastsym == intersym)
				{					 //输出整型
					gen(opr, 0, 16); //生成输出指令，输出栈顶的值
				}
				else if (lastsym == realsym)
				{
					gen(opr, 0, 24);
				}
			} while (sym == comma);
			if (sym != rparen)
			{
				error(73);
			}
			else
				getsym();
			gen(opr, 0, 17); //输出换行符
		}
		else
			error(74);
	}
	test(fsys, 0, 19);
}

void block(__int64 fsys)
{
	int j;
	long i, k = 0;
	long tx0; // initial table index
	long cx0; // initial code index
	long tx1; // save current table index before processing nested procedures
	long dx1; // save data allocation index
	long tx2;

	dx = 3;
	tx0 = tx;
	table[tx].addr = cx;
	gen(jmp, 0, 0);
	table[tx].n = prodn;
	for (j = 0; j < prodn; j++) //参数
	{
		tx++;
		strcpy(table[tx].name, pnow[j].id);
		table[tx].kind = variable;
		table[tx].level = lev;
		//table[tx].size=1;
		//table[tx].drt=0;
		table[tx].addr = dx;
		table[tx].type1 = pnow[j].sym;
		dx++;
	}
	tx2 = tx; //后来加的
			  /*	for(j=0;j<prodn;j++){
	gen(sto,lev-table[tx].level,table[tx].addr-j);
//	tx0--;
	} */
			  /*for(j=0;j<prodn;j++){    //栈顶指针上移
	gen(opr,0,23);
	}*/
	if (lev > levmax)
	{
		error(75);
	}
	do
	{
		if (sym == constsym)
		{
			getsym();
			do
			{
				constdeclaration(); //如果是const常量 则调用这个
				while (sym == comma)
				{
					getsym();
					constdeclaration();
				}
				if (sym == semicolon)
				{
					getsym();
				}
				else
				{
					error(76);
				}
			} while (sym == ident);
		}
		if (sym == typesym)
		{
			getsym();
			do
			{
				typedeclaration();
				if (sym == semicolon)
					getsym();
				else
					error(76);
			} while (sym == ident);
		}
		if (sym == varsym)
		{
			getsym();
			do
			{
				vardeclaration();
				/*hile(sym==comma){
		    getsym(); vardeclaration();
		}*/
				if (sym == semicolon)
				{
					getsym();
				}
				else
				{
					error(76);
				}
			} while (sym == ident);
		}
		while (sym == procsym || sym == funcsym)
		{
			prodn = 0;
			if (sym == procsym)
			{
				k=0;
				getsym();
				if (sym == ident)
				{
					enter(proc);
					getsym();
					i=position(id);
				}
				else
				{
					error(77);
				}
				if (sym == semicolon)
				{
					getsym();
				}
				else if (sym == lparen)
				{ //有左括号，说明是有参数的过程
					getsym();
					while (sym == ident)
					{
						strcpy(pnow[prodn].id, id); //记录参数名
						prodn++;
						if (ch == ':')
						{
							getch();
							getsym();
						}
						else
						{
							error(78);
						}
						if (sym == intersym || sym == realsym || sym == Boolsym)
						{
							pnow[prodn - 1].sym = sym;
							table[i].paral[k]=sym;
							k++;
							getsym();
						}
						else
						{
							error(79);
						}
						if (sym == semicolon || sym == rparen)
						{
							getsym();
						}
						else
						{
							error(80);
						}
					}
					if (sym == semicolon)
					{
						getsym();
					}
				}
				else
				{
					error(81);
				}
				//getsym();
			}
			else if (sym == funcsym)
			{ //处理函数
				k = 0;
				getsym();
				if (sym == ident)
				{
					enter(func);
					getsym();
					i = position(id);
				}
				else
				{
					error(82);
				}
				if (sym == lparen)
				{ //有左括号，说明是有参数的函数
					getsym();
					while (sym == ident)
					{
						strcpy(pnow[prodn].id, id); //记录参数名
						prodn++;
						if (ch == ':')
						{
							getch();
							getsym();
						}
						else
						{
							error(83);
						}
						if (sym == intersym || sym == realsym||sym==Boolsym)
						{
							table[i].paral[k] = sym;
							k++;
							pnow[prodn - 1].sym = sym;
							getsym();
						}
						else
						{
							error(84);
						}
						if (sym == semicolon || sym == rparen)
						{
							getsym();
						}
						else
						{
							error(85);
						}
					}
				}
				if (sym == nul)
				{
					getsym();
					if (sym == intersym || sym == realsym || sym == Boolsym)
					{ //声明返回值的类型
						table[i].type1 = sym;
						getsym();
					}
					else
					{
						error(86);
					}
				}
				else
				{
					error(87);
				}
				if (sym == semicolon)
					getsym();
				else
				{
					error(88);
				}
			}
			lev = lev + 1;
			tx1 = tx;
			dx1 = dx;
			block(fsys | semicolon); //第2层嵌套的子程序
			lev = lev - 1;
			tx = tx1;
			dx = dx1;
			if (sym == semicolon)
			{
				getsym();
				test(statbegsys | ident | procsym | funcsym, fsys, 28);
			}
			else
			{
				error(88);
			}
		}
		test(statbegsys | ident, declbegsys, 29);
	} while (sym & declbegsys);
	code[table[tx0].addr].a = cx; //将跳转地址改为正确的CX以保证调用返回的正确性
	table[tx0].addr = cx;		  // start addr of code
	cx0 = cx;
	for (j = 0; j < table[tx0].n; j++)
	{
		gen(sto, lev - table[tx2].level, table[tx2].addr - j);
		//	tx0--;
	}
	gen(Int, 0, dx + table[tx0].n);
	if (sym == beginsym)
	{
		statement(fsys | semicolon | endsym);
	}
	else
	{
		if(sym==period){
			printf("an illegal period\n");
			//system("pause");
		}	
		error(89);
		getsym();
	}
	gen(opr, 0, 0); // return
	test(fsys, 0, 8);
	//listcode(cx0);
}

long base(long b, long l)
{ //返回访问链的下标
	long b1;
	b1 = b;
	while (l > 0)
	{ // find base l levels down
		b1 = s[b1];
		l = l - 1; //根据访问链往上回溯找到变量
	}
	return b1;
}

void interpret()
{
	//	FILE *infile=fopen("out.txt","rb");
	long p, b, t;  // 指令计数器-,栈基址寄存器,栈顶指针
	instruction i; // instruction register
				   //	instruction code[200];
	int k = 0, kk;
	int a, j;
	int n;
	double getnum = 0;
	double tt = 1.0;
	long ls = 0;
	long away = 0;
	long adr;
	long d;
	char ch /*[nmax]*/;
	printf("start PL/0\n");
	t = 0;
	b = 1;
	p = 0;
	s[1] = 0;
	s[2] = 0;
	s[3] = 0;
	do
	{
		if (t > stacksize)
		{
			printf("overflow running_stack\n");
			exit(0);
		}
		i = code[p];
		p = p + 1;
		switch (i.f)
		{
		case lit:
			t = t + 1;
			s[t] = i.a;
			break;
			/*	case lir:
			t=t+1;
			s[t]=i.a;
			break;*/
		case opr:
			switch ((long)i.a)
			{		// operator
			case 0: // return
				t = b - 1;
				p = s[t + 3];
				b = s[t + 2];
				break;
			case 1:
				s[t] = -s[t];
				break;
			case 2:
				t = t - 1;
				s[t] = s[t] + s[t + 1];
				break;
			case 3:
				t = t - 1;
				s[t] = s[t] - s[t + 1];
				break;
			case 4:
				t = t - 1;
				s[t] = s[t] * s[t + 1];
				break;
			case 5: //slash
				t = t - 1;
				s[t] = (long)s[t] / s[t + 1];
				break;
			case 6:
				s[t] = (long)s[t] % 2;
				break;
			case 7:
				t--;
				break;
			case 8:
				t = t - 1;
				s[t] = (s[t] == s[t + 1]);
				break;
			case 9:
				t = t - 1;
				s[t] = (s[t] != s[t + 1]);
				break;
			case 10:
				t = t - 1;
				s[t] = (s[t] < s[t + 1]);
				break;
			case 11:
				t = t - 1;
				s[t] = (s[t] >= s[t + 1]);
				break;
			case 12:
				t = t - 1;
				s[t] = (s[t] > s[t + 1]);
				break;
			case 13:
				t = t - 1;
				s[t] = (s[t] <= s[t + 1]);
				break;
			case 14: //read指令,读整形3
				j = 0;
				getnum = 0;
				a = 0;
				t++;
				//printf("输入要读的数: ");
				//	scanf("%c",&ch[j]);
				ch = getchar();
				while (ch != ' ' && ch != '.')
				{
					getnum = getnum * 10 + (ch - '0');
					j++;
					//scanf("%c",&ch[j]);
					ch = getchar();
				}
				if (ch == '.')
				{ //说明读入的数是实型
					tt = 1.0;
					//	scanf("%c",&ch[j]);
					ch = getchar();
					while (ch != '\n')
					{
						tt /= 10;
						getnum = getnum + (ch - '0') * tt;
						j++;
						//scanf("%c",&ch[j]);
						ch = getchar();
					}
				}
				s[t] = getnum;
				break;
			case 15:
				ch = getchar(); //读取换行符
				break;
			case 16: //write指令
				printf("%5d", (long)s[t]);
				t--;
				break;
			case 17:
				printf("\n");
				break;
			case 18: //div
				t = t - 1;
				s[t] = s[t] / s[t + 1];
				break;
			case 19:
				t = t - 1;
				s[t] = (long)s[t] % (long)s[t + 1];
				break;
			case 20:
				t = t - 1;
				s[t] = (long)s[t] && (long)s[t + 1];
				break;
			case 21:
				t = t - 1;
				s[t] = (long)s[t] || (long)s[t + 1];
				break;
			case 22:
				s[t] = !(long)s[t];
				break;
			case 23:
				t++;
				break;
			case 24:
				printf("%5.2f  ", s[t]);
				t--;
				break;
			}
			break;
		case lod:
			t = t + 1;
			s[t] = s[base(b, i.l) + (long)i.a];
			break;
		case lay:
			//	adr=table[i.a].drt;
			adr = 0;
			for (k = 0, d = 0; k < s[base(b, i.l) + (long)i.a + 1]; k++)
			{
				ls = s[t - (long)(s[base(b, i.l) + (long)i.a + 1] - 1) + k];
				d++;
				if (ls < s[base(b, i.l) + (long)i.a + 1 + d])
				{
					printf("array overflow\n");
					error(40);
					break;
				}
				d++;
				if (ls > s[base(b, i.l) + (long)i.a + 1 + d])
				{
					printf("array overflow\n");
					error(40);
					break;
				}
				if (s[base(b, i.l) + (long)i.a + 1] == 1 || k == 0)
				{ //第一维情况
					away = ls - s[base(b, i.l) + (long)i.a + 1 + 1];
				}
				else
				{
					away = away * (s[base(b, i.l) + (long)i.a + 1 + d] - s[base(b, i.l) + (long)i.a + 1 + d - 1] + 1) + ls - s[base(b, i.l) + (long)i.a + 1 + d - 1] + 1;
				}
			}
			adr = adr + away;
			t = t + 1 - s[base(b, i.l) + (long)i.a + 1];
			s[t] = s[base(b, i.l) + (long)i.a + 1 + 2 * (long)s[base(b, i.l) + (long)i.a + 1] + 1 + adr];
			break;
		case sto:
			s[base(b, i.l) + (long)i.a] = s[t]; /* printf("%10d\n", s[t]);*/
			t = t - 1;
			break;
		case say:
			//      adr=table[i.a.longnum].drt;
			adr = 0;
			away = 0;
			for (k = 0, d = 0; k < s[base(b, i.l) + (long)i.a + 1]; k++)
			{
				ls = s[t - (long)s[base(b, i.l) + (long)i.a + 1] + k];
				d++;
				if (ls < s[base(b, i.l) + (long)i.a + 1 + d])
				{
					printf("array overflow\n");
					error(40);
					break;
				}
				d++;
				if (ls > s[base(b, i.l) + (long)i.a + 1 + d])
				{
					printf("array overflow\n");
					error(40);
					break;
				}

				if (s[base(b, i.l) + (long)i.a + 1] == 1 || k == 0)
				{
					away = ls - s[base(b, i.l) + (long)i.a + 1 + 1];
					;
				}
				else
				{
					away = away * (s[base(b, i.l) + (long)i.a + 1 + d] - s[base(b, i.l) + (long)i.a + 1 + d - 1] + 1) + ls - s[base(b, i.l) + (long)i.a + 1 + d - 1] + 1;
				}
			}
			adr = adr + away;
			//  t--;
			s[base(b, i.l) + (long)i.a + 1 + 2 * (long)s[base(b, i.l) + (long)i.a + 1] + 1 + adr] = s[t];
			t = t - 1 - s[base(b, i.l) + (long)i.a + 1]; //把要赋的值以及下标出栈
			break;
		case cal:					 // generate new block mark
			s[t + 1] = base(b, i.l); //将父过程基地址入栈  ,访问链
			s[t + 2] = b;			 //本过程基地址   控制链
			s[t + 3] = p;			 //返回地址
			b = t + 1;
			p = i.a;
			break;
		case Int:
			t = t + i.a;
			break;
		case jmp:
			p = i.a;
			break;
		case jpc:
			if (s[t] == 0)
			{
				p = i.a;
			}
			//t=t-1;
			break;
		case jpq:
			if (s[t] == 1)
			{
				p = i.a;
			}
			break;
		}
	} while (p != 0);
	printf("end PL/0\n");
}

void init()
{
	long i;
	for (i = 0; i < 256; i++)
	{
		ssym[i] = nul;
	}
	for (i = 0; i < norw; i++)
		wsym[i] = nul;
	strcpy(word[0], "Boolean   ");
	strcpy(word[1], "and       ");
	strcpy(word[2], "array     ");
	strcpy(word[3], "begin     ");
	strcpy(word[4], "call      ");
	strcpy(word[5], "const     ");
	strcpy(word[6], "div       ");
	strcpy(word[7], "do        ");
	strcpy(word[8], "else      ");
	strcpy(word[9], "end       ");
	strcpy(word[10], "exit      ");
	strcpy(word[11], "false     ");
	strcpy(word[12], "function  ");
	strcpy(word[13], "if        ");
	strcpy(word[14], "integer   ");
	strcpy(word[15], "mod       ");
	strcpy(word[16], "not       ");
	strcpy(word[17], "odd       ");
	strcpy(word[18], "of        ");
	strcpy(word[19], "or        ");
	strcpy(word[20], "procedure ");
	strcpy(word[21], "read      ");
	strcpy(word[22], "real      ");
	strcpy(word[23], "then      ");
	strcpy(word[24], "true      ");
	strcpy(word[25], "type      ");
	strcpy(word[26], "var       ");
	strcpy(word[27], "while     ");
	strcpy(word[28], "write     ");

	strcpy(wrongmsg[0], "placeholder");
	strcpy(wrongmsg[1], "const declare error:expected equal not becomes");
	strcpy(wrongmsg[2], "const declare error:ident type wrong ");
	strcpy(wrongmsg[3], "const declare error:operator wrong");
	strcpy(wrongmsg[4], "const declare error:expected ident");
	strcpy(wrongmsg[5], "const declare error:expected semicolon");
	strcpy(wrongmsg[6], "type declare error:expected equal not becomes");
	strcpy(wrongmsg[7], "type declare error:operator wrong");
	strcpy(wrongmsg[8], "type declare error:expected ident");
	strcpy(wrongmsg[9], "type declare error:expected semicolon");
	strcpy(wrongmsg[10],"var declare error:variable name duplication");
	strcpy(wrongmsg[11],"var declare error:type ident doesn't exist");
	strcpy(wrongmsg[12],"var declare error:expected type definition");
	strcpy(wrongmsg[13],"var declare error:expected colon");
	strcpy(wrongmsg[14],"var declare error:expected ident");
	strcpy(wrongmsg[15],"var declare error:expected semicolon");
	strcpy(wrongmsg[16],"procedure declare error:expected ident");
	strcpy(wrongmsg[17],"procedure declare error:(parameter error)expected colon");
	strcpy(wrongmsg[18],"procedure declare error:(parameter error)expected type");
	strcpy(wrongmsg[19],"procedure declare error:(parameter error)expected semicolon or right parenthesis");
	strcpy(wrongmsg[20],"procedure declare error:expected semicolon or parenthesis");
	strcpy(wrongmsg[21],"function declare error:expected ident");
	strcpy(wrongmsg[22],"function declare error:(parameter error)expecteed colon");
	strcpy(wrongmsg[23],"function declare error:(parameter error)expecteed type");
	strcpy(wrongmsg[24],"function declare error:(parameter error)expecteed end symbol");
	strcpy(wrongmsg[25],"function declare error:wrong function return type");
	strcpy(wrongmsg[26],"function declare error:expecteed colon before return type");
	strcpy(wrongmsg[27],"function declare error:expecteed end symbol");
	strcpy(wrongmsg[28],"proc and func struct error:wrong symbol after definition");
	strcpy(wrongmsg[29],"global struct error:wrong symbol after definition");
	strcpy(wrongmsg[30],"factor error:wrong begin symbol");
	strcpy(wrongmsg[31],"factor error:reference ident not exist");
	strcpy(wrongmsg[32],"factor error:wrong parameter type for an array");
	strcpy(wrongmsg[33],"factor error:expected a right middle parenthesis using array");
	strcpy(wrongmsg[34],"factor error:expected a parenthesis using an array");
	strcpy(wrongmsg[35],"factor error:a procedure can't be a factor");
	strcpy(wrongmsg[36],"factor error:function parameter type error");
	strcpy(wrongmsg[37],"factor error:number overflow");
	strcpy(wrongmsg[38],"factor error:expected a right parenthesis to match the left");
	strcpy(wrongmsg[39],"factor error:not a expected symbol after a factor");
	strcpy(wrongmsg[40],"term error:wrong member type for times");
	strcpy(wrongmsg[41],"term error:wrong member type for slash");
	strcpy(wrongmsg[42],"term error:wrong member type for div");
	strcpy(wrongmsg[43],"term error:wrong member type for mod");
	strcpy(wrongmsg[44],"term error:wrong member type for and");
	strcpy(wrongmsg[45],"simple expression error:wrong member type for plus");
	strcpy(wrongmsg[46],"simple expression error:wrong member type for minus");
	strcpy(wrongmsg[47],"simple expression error:wrong member type for or");
	strcpy(wrongmsg[48],"expression error:wrong member type for odd");
	strcpy(wrongmsg[49],"expression error:wrong member type for operator");
	strcpy(wrongmsg[50],"statement error(ident):array parameter expected an interger");
	strcpy(wrongmsg[51],"statement error(ident):a left middle parenthesis expected");
	strcpy(wrongmsg[52],"statement error(ident):wrong member for becomes");
	strcpy(wrongmsg[53],"statement error(ident):a type can't be duplicated");
	strcpy(wrongmsg[54],"statement error(call):an ident expected");
	strcpy(wrongmsg[55],"statement error(call):procedure or function not exist");
	strcpy(wrongmsg[56],"statement error(call):a right middle parenthesis expected");
	strcpy(wrongmsg[57],"statement error(call):calling a illegal object");
	strcpy(wrongmsg[58],"statement error(if):a Bool parameter expected");
	strcpy(wrongmsg[59],"statement error(if):a then symbol expected to match if");
	strcpy(wrongmsg[60],"statement error(begin):a semicolon is expected");
	strcpy(wrongmsg[61],"statement error(begin):an end symbol is expected");
	strcpy(wrongmsg[62],"statement error(while):a Bool parameter expected");
	strcpy(wrongmsg[63],"statement error(while):a do symbol is expected");
	strcpy(wrongmsg[64],"statement error(exit):an exit symbol must in a while");
	strcpy(wrongmsg[65],"statement error(read):a left parenthesis expected");
	strcpy(wrongmsg[66],"statement error(read):read an undeclared ident");
	strcpy(wrongmsg[67],"statement error(read):read an illegal ident");
	strcpy(wrongmsg[68],"statement error(read):read an illegal ident");
	strcpy(wrongmsg[69],"statement error(read):a int symbol expected");
	strcpy(wrongmsg[70],"statement error(read):a left middle parenthesis expected");
	strcpy(wrongmsg[71],"statement error(read):wrong read in type");
	strcpy(wrongmsg[72],"statement error(write):wro(ng write in type");
	strcpy(wrongmsg[73],"statement error(write):a right parenthesis expected");
	strcpy(wrongmsg[74],"statement error(write):a left parenthesis expected");
	strcpy(wrongmsg[75],"block error:level overflow");
	strcpy(wrongmsg[76],"block error:a semicolon expected");
	strcpy(wrongmsg[77],"block error(procedure):an ident expected");
	strcpy(wrongmsg[78],"block error(procedure):a colon expected");
	strcpy(wrongmsg[79],"block error(procedure):wrong parameter type");
	strcpy(wrongmsg[80],"block error(procedure):a semicolon or rparen expected");
	strcpy(wrongmsg[81],"block error(procedure):wrong symbol input");
	strcpy(wrongmsg[82],"block error(function):expected ident");
	strcpy(wrongmsg[83],"block error(function):expected colon");
	strcpy(wrongmsg[84],"block error(function):illegal parameter type");
	strcpy(wrongmsg[85],"block error(function):expected semicolon or rparen");
	strcpy(wrongmsg[86],"block error(function):wrong symbol input");
	strcpy(wrongmsg[87],"block error(function):expected colon");
	strcpy(wrongmsg[88],"block error:expected semicolon");
	strcpy(wrongmsg[89],"block error:expected begin symbol");
	strcpy(wrongmsg[90],"main struct error:expected a period to end");
	strcpy(wrongmsg[91],"wrong value for the ident type");

	strcpy(mnemonic[lit], "lit");
	strcpy(mnemonic[opr], "opr");
	strcpy(mnemonic[lod], "lod");
	strcpy(mnemonic[sto], "sto");
	strcpy(mnemonic[cal], "cal");
	strcpy(mnemonic[Int], "int");
	strcpy(mnemonic[jmp], "jmp");
	strcpy(mnemonic[jpc], "jpc");
	strcpy(mnemonic[say], "say"); //数组的存指令
	strcpy(mnemonic[lay], "lay"); //数组的取指令
	strcpy(mnemonic[jpq], "jpq");
	wsym[0] = Boolsym;
	wsym[1] = and;
	wsym[2] = arraysym;
	wsym[3] = beginsym;

	wsym[4] = callsym;
	wsym[5] = constsym;
	wsym[6] = div;
	wsym[7] = dosym;
	wsym[8] = elsesym;
	wsym[9] = endsym;
	wsym[10] = exitsym;
	wsym[11] = falsesym;
	wsym[12] = funcsym;
	wsym[13] = ifsym;
	wsym[14] = intersym;
	wsym[15] = mod;
	wsym[16] = not ;
	wsym[17] = oddsym;
	wsym[18] = ofsym;
	wsym[19] = or ;
	wsym[20] = procsym;
	wsym[21] = readsym;
	wsym[22] = realsym;
	wsym[23] = thensym;
	wsym[24] = truesym;
	wsym[25] = typesym;
	wsym[26] = varsym;
	wsym[27] = whilesym;
	wsym[28] = writesym;
	ssym['+'] = plus;
	ssym['-'] = minus;
	ssym['*'] = times;
	ssym['/'] = slash;
	ssym['('] = lparen;
	ssym[')'] = rparen;
	//ssym['=']=eql;
	ssym[','] = comma;
	ssym['.'] = period;
	ssym[';'] = semicolon;
	ssym['['] = lmparen;
	ssym[']'] = rmparen;

	declbegsys = constsym | typesym | varsym | procsym | funcsym;
	statbegsys = beginsym | callsym | ifsym | whilesym | exitsym | writesym | readsym;
	facbegsys = ident | intersym | realsym | lparen | not | truesym | falsesym;
}

int main()
{
	init();
	printf("please input source program file name: ");
	scanf("%s", infilename);
	getchar();
	printf("\n");
	if ((infile = fopen(infilename, "r")) == NULL)
	{
		printf("File %s can't be opened.\n", infilename);
		exit(1);
	}

	err = 0;
	cc = 0;
	cx = 0;
	ll = 0;
	ch = ' ';
	kk = al;
	getsym();
	lev = 0;
	tx = 0;
	//printf("test");
	block(declbegsys | statbegsys | period);
	if (sym != period)
	{
		error(90);
	}
	if (err == 0)
	{
		if ((outfile = fopen("out.txt", "wb")) == NULL)
		{
			printf("File <out.txt> can't be opened.\n");
			exit(1);
		}
		fwrite(&code, sizeof(instruction), cxmax, outfile);
		fclose(outfile);
		if ((outfile = fopen("code.txt", "w")) == NULL)
		{
			printf("File <out.txt> can't be opened.\n");
			exit(1);
		}
		for (int i = 0; i <= cx; i++)
		{
			fprintf(outfile, "%10d%5s%3d%10.5f\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
		}
		fclose(outfile);
		interpret();
	}
	else
	{
		color(4);
		printf("%d errors in %s \n",errornum,infilename);
		color(15);
	}
	fclose(infile);
	system("pause");
	return (0);
}
