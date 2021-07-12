// pl/0 compiler with code generation
#include <stdlib.h>
#include <string.h>
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
					getch()
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
	return i;
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
			if (sym == intersym||sym==realsym)//const允许定义为整数或实数
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

void typedeclaration(){
	char id1[al+1];
	if(sym==ident){
		strcpy(id1,id);
		getsym();
		if(sym==eql||sym==becomes){
			if(sym==becomes){
				error(1);
			}
			getsym();
			strcpy(id,id1);
			typeexpression();
		}
		else{
			error(3);
		}
	}else{
		error(4);
	}
}

void typeexpression(){
	__int64 sym1;
	long num3,num4,size;//数组上下界，大小
	long tx1;
	int drtnum=1;//数组维数
	int j;
	if(sym==intersym||sym==realsym||sym==Boolsym){
		enter(type);
		getsym();
	}
	else if(sym==arraysym){

	}
	else{
		error(33);
	}
}

void vardeclaration()
{
	if (sym == ident)
	{
		enter(variable);
		getsym();
	}
	else
	{
		error(4);
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

void expression(unsigned long);
void factor(unsigned long fsys)
{
	long i;

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
					break;
				case variable:
					gen(lod, lev - table[i].level, table[i].addr);
					break;
				case proc:
					error(21);
					break;
				}
			}
			getsym();
		}
		else if (sym == number)
		{
			if (num > amax)
			{
				error(31);
				num = 0;
			}
			gen(lit, 0, num);
			getsym();
		}
		else if (sym == lparen)
		{
			getsym();
			expression(rparen | fsys);
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

void term(unsigned long fsys)
{
	unsigned long mulop;

	factor(fsys | times | slash);
	while (sym == times || sym == slash)
	{
		mulop = sym;
		getsym();
		factor(fsys | times | slash);
		if (mulop == times)
		{
			gen(opr, 0, 4);
		}
		else
		{
			gen(opr, 0, 5);
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

void statement(unsigned long fsys)
{
	long i, cx1, cx2;

	if (sym == ident)
	{
		i = position(id);
		if (i == 0)
		{
			error(11);
		}
		else if (table[i].kind != variable)
		{ // assignment to non-variable
			error(12);
			i = 0;
		}
		getsym();
		if (sym == becomes)
		{
			getsym();
		}
		else
		{
			error(13);
		}
		expression(fsys);
		if (i != 0)
		{
			gen(sto, lev - table[i].level, table[i].addr);
		}
	}
	else if (sym == callsym)
	{
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
			else if (table[i].kind == proc)
			{
				gen(cal, lev - table[i].level, table[i].addr);
			}
			else
			{
				error(15);
			}
			getsym();
		}
	}
	else if (sym == ifsym)
	{
		getsym();
		condition(fsys | thensym | dosym);
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
		statement(fsys);
		code[cx1].a = cx;
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
		cx1 = cx;
		getsym();
		condition(fsys | dosym);
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
		table[tx].size=1;
		table[tx].drt=0;
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
				constdeclaration();//ident=number
				while (sym == comma)//声明第二个变量
				{
					getsym();
					constdeclaration();
				}
				if (sym == semicolon)//const语句结束
				{
					getsym();
				}
				else
				{
					error(5);
				}
			} while (sym == ident);
		}
		if(sym==typesym){//类型声明，占位
			getsym();
			do{
				typedeclaration();
			}while(sym==ident);
		}
		if (sym == varsym)
		{
			getsym();
			do
			{
				vardeclaration();
				while (sym == comma)
				{
					getsym();
					vardeclaration();
				}
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
		while (sym == procsym)
		{
			getsym();
			if (sym == ident)
			{
				enter(proc);
				getsym();
			}
			else
			{
				error(4);
			}
			if (sym == semicolon)
			{
				getsym();
			}
			else
			{
				error(5);
			}
			lev = lev + 1;
			tx1 = tx;
			dx1 = dx;
			block(fsys | semicolon);
			lev = lev - 1;
			tx = tx1;
			dx = dx1;
			if (sym == semicolon)
			{
				getsym();
				test(statbegsys | ident | procsym, fsys, 6);
			}
			else
			{
				error(5);
			}
		}
		test(statbegsys | ident, declbegsys, 7);
	} while (sym & declbegsys);
	code[table[tx0].addr].a = cx;
	table[tx0].addr = cx; // start addr of code
	cx0 = cx;
	gen(Int, 0, dx);
	statement(fsys | semicolon | endsym);
	gen(opr, 0, 0); // return
	test(fsys, 0, 8);
	listcode(cx0);
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
	long p, b, t;  // program-, base-, topstack-registers
	instruction i; // instruction register

	printf("start PL/0\n");
	t = 0;
	b = 1;
	p = 0;
	s[1] = 0;
	s[2] = 0;
	s[3] = 0;
	do
	{
		i = code[p];
		p = p + 1;
		switch (i.f)
		{
		case lit:
			t = t + 1;
			s[t] = i.a;
			break;
		case opr:
			switch (i.a)
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
			case 5:
				t = t - 1;
				s[t] = s[t] / s[t + 1];
				break;
			case 6:
				s[t] = s[t] % 2;
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
			}
			break;
		case lod:
			t = t + 1;
			s[t] = s[base(b, i.l) + i.a];
			break;
		case sto:
			s[base(b, i.l) + i.a] = s[t];
			printf("%10d\n", s[t]);
			t = t - 1;
			break;
		case cal: // generate new block mark
			s[t + 1] = base(b, i.l);
			s[t + 2] = b;
			s[t + 3] = p;
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
			t = t - 1;
		}
	} while (p != 0);
	printf("end PL/0\n");
}

int main()
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
	strcpy(word[1], "AND       ");
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
	strcpy(word[16], "NOT       ");
	strcpy(word[17], "odd       ");
	strcpy(word[18], "of        ");
	strcpy(word[19], "OR        ");
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
	wsym[1] = AND;
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
	wsym[16] = NOT;
	wsym[17] = oddsym;
	wsym[18] = ofsym;
	wsym[19] = OR;
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
	facbegsys = ident | intersym | realsym | lparen | NOT | truesym | falsesym;

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
		for (i = 0; i <= cx; i++)
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
