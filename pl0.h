#include <stdio.h>

#define norw       29             // no. of reserved words
#define txmax      100            // length of identifier table
#define nmax       14             // max. no. of digits in numbers
#define al         10             // length of identifiers
#define amax       2047           // maximum address
#define levmax     3              // maximum depth of block nesting
#define cxmax      2000          /* 最多的虚拟机代码数 */
#define now        10             //数组的最大维数

#define nul	       0x1
#define ident      0x2
#define pnum       10              //函数参数个数的上限
//#define number     0x4
#define plus       0x8
#define minus      0x10
#define times      0x20
#define slash      0x40
#define oddsym     0x80
#define eql        0x100
#define neq        0x200
#define lss        0x400
#define leq        0x800
#define gtr        0x1000
#define geq        0x2000
#define lparen     0x4000
#define rparen     0x8000
#define comma      0x10000
#define semicolon  0x20000
#define period     0x40000
#define becomes    0x80000
#define beginsym   0x100000
#define endsym     0x200000
#define ifsym      0x400000
#define thensym    0x800000
#define whilesym   0x1000000
#define dosym      0x2000000
#define callsym    0x4000000
#define constsym   0x8000000
#define varsym     0x10000000
#define procsym    0x20000000  //后边用于二分法查找关键字
#define typesym    0x40000000
#define arraysym   0x80000000
#define ofsym      0x100000000
#define intersym   0x200000000
#define realsym    0x400000000
#define Boolsym    0x800000000
#define funcsym    0x1000000000
#define elsesym    0x2000000000
#define writesym   0x4000000000
#define readsym    0x8000000000
#define exitsym    0x10000000000
#define or         0x20000000000
#define and        0x40000000000
#define not        0x80000000000
#define div        0x100000000000
#define mod        0x200000000000
#define truesym    0x400000000000
#define falsesym   0x800000000000
#define lmparen    0x1000000000000
#define rmparen    0x2000000000000
#define typeerror  0x4000000000000
#define voiderror  0x8000000000000
#define dotdot     0x10000000000000


enum object {
    constant, variable, proc, type, func
};
enum fct {
    lit, opr, lod, sto, cal, Int, jmp, jpc,say,lay,jpq//计算or短路，栈顶元素为1时跳转         // functions
};

typedef struct{
    enum fct f;		// function code
    long l; 		// level
/*	union{
		long longnum;
		float dounum;
	}*/double a; 		// displacement address
} instruction;
/*  lit 0, a : load constant a
    opr 0, a : execute operation a
    lod l, a : load variable l, a
    sto l, a : store variable l, a
    cal l, a : call procedure a at level l
    Int 0, a : increment t-register by a
    jmp 0, a : jump to a
    jpc 0, a : jump conditional to a       */

int errornum=0; 
char ch;               // last character read
__int64 sym;     // last symbol read 最后一个词的类型
__int64 lastsym;   //最后取得的词法类型
char id[al+1];         // last identifier read
double num;              // last number read 
//float numreal;            //实型 
long cc;               /* getch使用的计数器，cc表示当前字符(ch)的位置 */
long ll;               // line length
long kk, err;           /* 错误计数器 */
long cx;               /* 虚拟机代码指针, 取值围[0, cxmax-1]*/

char line[81];/* 读取行缓冲区 */
char a[al+1]; /* 临时符号, 多出的一个字节用于存放0 */
instruction code[cxmax+1];/* 存放虚拟机代码的数组 */
char word[norw][al+1];//保留字
char wrongmsg[100][100];
__int64 wsym[norw];//保留字对应的符号值
__int64 ssym[256];//单字符的符号值

char mnemonic[11][5];//虚拟机代码指令名称
__int64 declbegsys, statbegsys, facbegsys;//声明开始的符号，语句开始的符号，因子开始的符号  

struct{
    char name[al+1];
    enum object kind;
    double val;
    long level;
    long addr;
	long funcaddr;     //给函数使用，记录函数在符号表中的地址偏移量
    __int64 paral[pnum]; //存放函数参数类型，用于类型检查
	__int64 type1;     //在符号表中增加类型项
    long size;         //若是数组类型还要记录大小
	long drt;            //数组维数
	long low[now];          //数组每一维下界
	long high[now];         //数组每一维上界
	__int64 type2;     //数组元素的类型
	int n;             //过程或函数的参数个数
}table[txmax+1];      //符号表的结构

char infilename[80];
FILE* infile,*outfile;

struct prod 
{ 
	char id[al+1];     //参数名
	__int64 sym;       //参数类型
}; 
struct prod pnow[15];  //记录过程或函数的参数信息
int  prodn=0; 

// the following variables for block
long dx;		// data allocation index   符号表中的地址
long lev;		// current depth of block nesting
long tx;		// current table index 符号表栈序号
long whilenum=0;   //记录while循环嵌套次数，用于exit的判断
long exitcx=0;   //记录最新的exit语句的中间代码的位置

// the following array space for interpreter
#define stacksize 5000
double s[stacksize];	// datastore
