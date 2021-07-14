// pl/0 compiler with code generation
#include <stdlib.h>
#include <string.h>
#include<ctype.h>
#include<math.h>
#include "pl0.h"

void error(long n)
{
	long i;

	printf(" ****");
	for (i = 1; i <= cc - 1; i++)
	{
		printf(" ");
	}
	printf("^%2d\n", n);
	err++;
}

void getch()
{ //输入源程序和读入下一个字符
	if (cc == ll)
	{ //如果当前字符是本行最后一个
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
		{
			printf("%c", ch);
			ll = ll + 1;
			line[ll] = ch;
		}
		printf("\n");
		ll = ll + 1;
		line[ll] = ' ';
	}
	cc = cc + 1;
	ch = line[cc]; //从读好的一行里取一个
}

void getsym()
{ //读取一个字符
	long i, j, k;
	int flag = 0;
	double t = 1.0;			//标记小数点后位数
	double numt = 0, z = 0; //小数部分值

	while (ch == ' ' || ch == '\t')
	{
		getch();
	}
	if (isalpha(ch)) //开头要是字母
	{				 // identified or reserved
		k = 0;
		do
		{
			if (k < al)
			{
				a[k] = ch; //a中为暂存的符号
				k = k + 1;
			}
			getch();
		} while (isalpha(ch) || isdigit(ch)); //读入一直到下一个非字母或者数字为止
		if (k >= kk)						  //字符长度超出
		{
			kk = k;
		}
		else
		{ //字符不足的部分用空格补齐
			do
			{
				kk = kk - 1;
				a[kk] = ' ';
			} while (k < kk);
		}
		strcpy(id, a);
		i = 0;
		j = norw - 1;
		do //二分法依次在保留字表里寻找相等地址
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
		} while (i <= j);
		if (i - 1 > j)
		{ //若为保留字
			sym = wsym[k];
		}
		else
		{ //为变量
			sym = ident;
		}
	}
	else if (isdigit(ch))
	{ // number
		k = 0;
		num = 0; //k记录数字长度，num记录数值大小
		//sym = number;
		do
		{
			num = num * 10 + (ch - '0');
			k = k + 1;
			getch();
		} while (isdigit(ch));
		if (ch == '.')
		{
			getch();
			if (isdigit(ch))
			{
				while (isdigit(ch))
				{
					t /= 10;
					;
					num = num + (ch - '0') * t;
					k++;
					getch();
				}
				sym = realsym; //实数类型
			}
			else if (ch == '.')
			{ //第二个点是数组的定义
				sym = intersym;
				cc = cc - 2;
				getch();
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
			sym = nul; //无法识别，:必须和=搭配
		}
	}
	else if (ch == '/')
	{ //注释部分
		getch();
		if (ch == '*')
		{
			flag++; //flag=1,之后都为注释
			getch();
			while (flag > 0)
			{
				while (ch != '*')
				{
					getch();
				}
				getch();
				if (ch = '/')
				{
					flag--; //找到*/，注释结束
				}
			}
			getch();
			getsym(); //读取下一个单词
		}
		else if (ch == '/')
		{
			flag++;
			getch();
			while (flag > 0)
			{
				while (cc != ll)
				{
					getch();
				}
				flag--;
			}
			getch();
			getsym();
		}
		else
		{ //其实是个除法符号
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
					{
						flag--;
					}
				}
				getch();
			}
			else
			{
				printf("wrong symbol");
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
			sym = dotdot; //数组中的..
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

void gen(enum fct x, long y, long z)
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

void test(unsigned long s1, unsigned long s2, long n)
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
		table[tx].type1 = sym;
		break;
	case variable:
		table[tx].level = lev;
		table[tx].addr = dx;
		dx = dx + 1;
		break;
	case proc:
		table[tx].level = lev;
		dx = dx + 1; //procedure为什么要加偏移地址？
		break;
	case type:
		table[tx].type1 = sym;
		table[tx].level = lev;
		break;
	case func:
		table[tx].funcaddr = dx;
		table[tx].level = lev;
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
	return i;//若返回0，意味着未找到相同名称的变量，否则有
}

void constdeclaration()
{
	if (sym == ident)
	{
		getsym();
		if (sym == eql || sym == becomes)
		{
			if (sym == becomes)
			{
				error(1);
			}
			getsym();
			if (sym == intersym || sym == realsym) //const允许定义为整数或实数
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

void typeexpression()
{
	__int64 sym1;
	long num3, num4, size; //数组上下界，大小
	long tx1;
	int drtnum = 1; //数组维数
	int j;
	if (sym == intersym || sym == realsym || sym == Boolsym)
	{
		enter(type);
		getsym();
	}
	else if (sym == arraysym)
	{
		//sym1=sym;
		getsym();
		if (sym == lmparen)
		{
			getsym();
			if (sym == intersym)
			{
				num3 = num; //给数组的前边界赋值
				getsym();
				if (sym == dotdot)
				{
					getsym();
					if (sym == intersym)
					{
						num4 = num;
						if (num4 < num3)
						{
							error(35);
						}
						size = num4 - num3 + 1;
						enter(type);
						table[tx].low[drtnum] = num3; //数组第一维的上下界
						table[tx].high[drtnum] = num4;
						table[tx].drt++;
						getsym();
						if (sym != rmparen)
						{
							error(34);
						}
						getsym();
						if (sym == ofsym)
						{
							getsym();
							if (sym == intersym || sym == realsym || sym == Boolsym)
							{
								table[tx].type2 = sym;
								table[tx].drt = 1;
								table[tx].size = size; //不懂为什么，只声明了数组属性，没有给他开辟空间
							}
							else
							{
								while (sym == arraysym)
								{
									table[tx].drt++;
									drtnum++;
									getsym();
									if (sym == lmparen)
									{
										getsym();
										if (sym != intersym)
										{
											error(34);
										}
										table[tx].low[drtnum] = num;
										getsym();
										if (sym != dotdot)
										{
											error(34);
										}
										getsym();
										if (sym != intersym)
										{
											error(34);
										}
										if (num < table[tx].low[drtnum])
										{
											error(35);
										}
										table[tx].high[drtnum] = num;
										getsym();
										if (sym != rmparen)
										{
											error(34);
										}
										getsym();
										if (sym != ofsym)
										{
											error(34);
										}
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
							//getsym();垃圾代码！！
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
		{
			error(34);
		}
	}
	else
	{
		error(33);
	}
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
				error(1);
			}
			getsym();
			strcpy(id, id1);
			typeexpression();
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
	long i;//用于存储在符号表中搜索得到的index
	int identnum=0;//同类型的变量个数
	char sameid[10][al+1];//存放变量名
	long size;
	if (sym == ident)
	{
		strcpy(sameid[identnum],id);
		identnum++;
		getsym();
		while(sym==comma){
			getsym();
			if(sym==ident){
				for(int j=0;j<identnum;j++){
					if(strcmp(sameid[j],id)==0){
						error(48);
						break;
					}
				}
				strcpy(sameid[identnum],id);
				identnum++;
				getsym();
			}
		}
		if(sym==nul){//遇到了:,对声明的变量进行类型定义
			getsym();
			if(sym==intersym||sym==realsym||sym==Boolsym){
				for(int j=0;j<identnum;j++){
					strcpy(id,sameid[j]);
					i=position(id);
					if(i!=0){//意味着已存在该变量
						error(48);
					}
					enter(variable);
					table[tx].type1=sym;
				}
			}
			else if(sym==ident){
				i=position(id);
				if(i==0){
					error(11);//不存在用于定义类型的类型变量
				}
				sym=table[i].type1;
				if(sym==intersym||sym==realsym||sym==Boolsym){
					for(int j=0;j<identnum;j++){
						strcpy(id,sameid[j]);
						enter(variable);
						table[tx].type1=sym;
					}
				}
				else if(sym==arraysym){
					for(int j=0;j<identnum;j++){
						strcpy(id,sameid[j]);
						enter(variable);
						table[tx].type1=arraysym;
						table[tx].size=table[i].size;
						size=table[tx].size;
						table[tx].drt=table[i].drt;
						for(int t=1;t<=table[tx].drt;t++){
							table[tx].low[t]=table[i].low[t];
							table[tx].high[t]=table[i].high[t];
						}
						sym=table[i].type2;
						table[tx].type2=sym;
						dx=dx+2*table[i].drt+1;//每一维都需要2+1个空间的控制信息
						dx+=size;
					}
				}
			}
			else{
				error(36);
			}
		}
		else{
			error(55);
		}
	}
	else
	{
		error(4);
	}
}

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

void listcode(long cx0)
{ // list code generated for this block
	long i;

	for (i = cx0; i <= cx - 1; i++)
	{
		printf("%10d%5s%3d%5d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	}
}

void expre(__int64);

void factor(__int64 fsys)
{
	long i;
	long drtnum, away;
	long k;
	long j;
	test(facbegsys, fsys, 24);
	while (sym & facbegsys)
	{
		if (sym == ident)
		{
			i = position(id);
			if (i == 0)
			{
				error(11);
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
										error(49);
									}
									if (sym == rmparen)
										getsym();
									else
										error(40);
								}
								else
								{
									error(46);
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
					error(21);
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
							error(49);
							lastsym = typeerror;
						}
						while (sym == comma)
						{
							k++;
							getsym();
							expre(fsys | comma | rparen);
							if (lastsym != table[i].paral[k])
							{
								error(49);
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
				error(31);
				num = 0;
			}
			gen(lit, 0, num);
			lastsym = sym;
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
				error(22);
			}
		}
		test(fsys, lparen, 23);
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
				error(50);
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
				error(50);
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
				error(50);
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
				error(50);
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
				error(50);
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
				error(50);
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
				error(50);
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
				error(50);
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
			error(50);
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
			error(50);
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

void expression(unsigned long fsys)
{
	unsigned long addop;

	if (sym == plus || sym == minus)
	{
		addop = sym;
		getsym();
		term(fsys | plus | minus);
		if (addop == minus)
		{
			gen(opr, 0, 1);
		}
	}
	else
	{
		term(fsys | plus | minus);
	}
	while (sym == plus || sym == minus)
	{
		addop = sym;
		getsym();
		term(fsys | plus | minus);
		if (addop == plus)
		{
			gen(opr, 0, 2);
		}
		else
		{
			gen(opr, 0, 3);
		}
	}
}

void condition(unsigned long fsys)
{
	unsigned long relop;

	if (sym == oddsym)
	{
		getsym();
		expression(fsys);
		gen(opr, 0, 6);
	}
	else
	{
		expression(fsys | eql | neq | lss | gtr | leq | geq);
		if (!(sym & (eql | neq | lss | gtr | leq | geq)))
		{
			error(20);
		}
		else
		{
			relop = sym;
			getsym();
			expression(fsys);
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
			case gtr:
				gen(opr, 0, 12);
				break;
			case leq:
				gen(opr, 0, 13);
				break;
			}
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
						error(46);
					}
					lasttype = lastsym;
					if (sym == rmparen)
					{
						getsym();
					}
				}
				else
				{
					error(46);
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
			error(51);
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
				error(56);
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
			error(14);
		}
		else
		{
			i = position(id);
			if (i == 0)
			{
				error(11);
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
							k++;
						} while (sym == comma);
						if (sym == rparen)
						{
							getsym();
						}
						else
						{
							error(40);
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
					error(15);
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
			error(52);
		}
		if (sym == thensym)
		{
			getsym();
		}
		else
		{
			error(16);
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
				error(10);
			}
			statement(fsys | semicolon | endsym);
		}
		if (sym == endsym)
		{
			getsym();
		}
		else
		{
			error(17);
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
			error(52);
		}
		cx2 = cx;
		gen(jpc, 0, 0);
		if (sym == dosym)
		{
			getsym();
		}
		else
		{
			error(18);
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
			error(47);
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
			error(35);
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
					error(36);
				else if (table[i].kind == constant || table[i].kind == proc || table[i].type1 == Boolsym)
				{ // 不能往常量或过程以及布尔类型读入数据
					error(12);
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
										error(46);
									}
									if (sym == rmparen)
									{
										getsym();
									}
								}
								else
								{
									error(46);
								}
							}
							gen(opr, 0, 14);
							arraydo(sto, i);
						}
						else
							error(39);
					}
				}
			} while (sym == comma);
			gen(opr, 0, 15); //取得换行符
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
					error(53);
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
				error(35);
			}
			else
				getsym();
			gen(opr, 0, 17); //输出换行符
		}
		else
			error(35);
	}
	test(fsys, 0, 19);
}

void block(unsigned long fsys)
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
	for (j = 0; j < prodn; j++)
	{ //读入参数的初始化
		tx++;
		strcpy(table[tx].name, pnow[j].id);
		table[tx].kind = variable;
		table[tx].level = lev;
		table[tx].size = 1;
		table[tx].drt = 0;
		table[tx].addr = dx;
		table[tx].type1 = pnow[j].sym;
		dx++;
	}
	tx2 = tx; //unknown
	if (lev > levmax)
	{
		error(32);
	}
	do
	{
		if (sym == constsym)
		{
			getsym();
			do
			{
				constdeclaration();	 //ident=number
				while (sym == comma) //声明第二个变量
				{
					getsym();
					constdeclaration();
				}
				if (sym == semicolon) //const语句结束
				{
					getsym();
				}
				else
				{
					error(5);
				}
			} while (sym == ident);
		}
		if (sym == typesym)
		{ //类型声明，占位
			getsym();
			do
			{
				typedeclaration();
				getsym();
				if (sym == semicolon)
				{
					getsym();
				}
				else
				{
					error(33);
				}
			} while (sym == ident);
		}
		if (sym == varsym)
		{
			getsym();
			do
			{
				vardeclaration();
				getsym();
				if (sym == semicolon)
				{
					getsym();
				}
				else
				{
					error(5);
				}
			} while (sym == ident);
		}
		while (sym == procsym||funcsym)
		{
			prodn=0;
			if(sym==procsym){
				getsym();
				if(sym==ident){
					enter(proc);
					getsym();
				}
				else{
					error(4);
				}
				if(sym==semicolon){
					getsym();
				}
				else if(sym==lparen){
					getsym();
					while(sym==ident){
						strcpy(pnow[prodn].id,id);
						prodn++;
						if(ch==':'){
							getch();
							getsym();
						}
						else{
							error(38);
						}
						if(sym==intersym||sym==realsym||sym==Boolsym){
							pnow[prodn-1].sym=sym;
							getsym();
						}
						else{
							error(39);
						}
						if(sym==semicolon||sym==rparen){
							getsym();
						}
						else{
							error(5);
						}
					}
					if(sym==semicolon){
						getsym();
					}
				}
				else{
					error(5);
				}
			}
			else if(sym==funcsym){
				k=0;
				getsym();
				if(sym==ident){
					enter(func);
					getsym();
					i=position(id);
				}
				else{
					error(4);
				}
				if(sym==lparen){
					getsym();
					while(sym==ident){ 
						strcpy(pnow[prodn].id,id);
						prodn++;
						if(ch==':'){
							getch();
							getsym();
						}
						else{
							error(38);
						}
						if(sym==intersym||sym==realsym||sym==Boolsym){
							table[i].paral[k]=sym;
							k++;
							pnow[prodn-1].sym=sym;
							getsym();

						}
						else{
							error(39);
						}
						if(sym==semicolon||sym==rparen){
							getsym();
						}
						else{
							error(5);
						}
					}
				}
				if(sym==nul){//接收到了:
					getsym();
					if(sym==intersym||sym==realsym||sym==Boolsym){//函数返回值类型
						table[i].type1=sym;
						getsym;
					}else{
						error(45);
					}
				}else{
					error(44);
				}
				if(sym==semicolon){
					getsym();
				}else{
					error(5);
				}
			}
			lev=lev+1;
			tx1=tx;
			dx1=dx;
			block(fsys|semicolon);//递归下降
			lev=lev-1;
			tx=tx1;
			dx=dx1;
			if(sym==semicolon){
				getsym();
				test(statbegsys|ident|procsym|funcsym,fsys,6);
			}
			else{
				error(5);
			}
		}
		test(statbegsys | ident, declbegsys, 7);
	} while (sym & declbegsys);
	code[table[tx0].addr].a = cx;//切换回上层调用点地址
	table[tx0].addr = cx; // start addr of code
	cx0 = cx;
	for(j=0;j<table[tx0].n;j++){
		gen(sto,lev-table[tx2].level,table[tx2].addr-j);
	}
	gen(Int, 0, dx+table[tx0].n);
	if(sym==beginsym){
		statement(fsys | semicolon | endsym);
	}else{
		error(54);
		getsym();
	}
	gen(opr, 0, 0); // return
	test(fsys, 0, 8);
	//listcode(cx0);
}

long base(long b, long l)
{
	long b1;

	b1 = b;
	while (l > 0)
	{ // find base l levels down
		b1 = s[b1];
		l = l - 1;
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
	{
		wsym[i] = nul;
	}
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
	wsym[16] = not;
	wsym[17] = oddsym;
	wsym[18] = ofsym;
	wsym[19] = or;
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
	ssym['='] = eql;
	ssym[','] = comma;
	ssym['.'] = period;
	ssym[';'] = semicolon;
	ssym['['] = lmparen;
	ssym[']'] = rmparen;
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
	declbegsys = constsym | varsym | procsym | funcsym | typesym;
	statbegsys = beginsym | callsym | ifsym | whilesym | exitsym | writesym | readsym;
	facbegsys = ident | intersym | realsym | lparen | not | truesym | falsesym;
}

int main()
{
	init();
	printf("please input source program file name: ");
	scanf("%s", infilename);
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
	block(declbegsys | statbegsys | period);
	if (sym != period)
	{
		error(9);
	}
	if (err == 0)
	{
		if ((outfile = fopen("out.txt", "wb")) == NULL)
		{
			printf("File <out.txt> open failed.");
			exit(1);
		}
		fwrite(&code, sizeof(instruction), cxmax, outfile);
		fclose(outfile);
		if ((outfile = fopen("code.txt", "wb")) == NULL)
		{
			printf("File <code.txt> open failed.");
			exit(1);
		}
		for (long i = 0; i <= cx; i++)
		{
			fprintf(outfile, "%10d%5s%3d%10.5f\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
		}
		fclose(outfile);
		interpret();
	}
	else
	{
		printf("errors in PL/0 program\n");
	}
	fclose(infile);
	system("pause");
	return 0;
}
