// pl/0 compiler with code generation
#include <stdlib.h>
#include <string.h>
#include<stdio.h>
#include<iostream>
#include<bitset>
#include "pl0.h"
using namespace std;

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
}

int main()
{
	init();
	char a[1000];
    itoa((constsym|statbegsys|period| semicolon | endsym),a,2);

	printf("%s",a);
	
	return 0;
}
