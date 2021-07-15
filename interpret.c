#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include<ctype.h>
#include "pl0.h"

long base(long b, long l){   //返回访问链的下标
    long b1;
    b1=b;
    while (l>0){	// find base l levels down
	b1=s[b1]; l=l-1;     //根据访问链往上回溯找到变量
    }
    return b1;
}


void main(){
	FILE *infile=fopen("pcode2byte.txt","rb");
    long p,b,t;		// 指令计数器-,栈基址寄存器,栈顶指针
    instruction i;	// instruction register
    int k=0,kk;
	int a,j;
	int n;
	double getnum=0;
	double tt=1.0;
	long ls=0;
	long away=0;
	long adr;
	long d;
	char ch;
    printf("\n\ninterret run pcode program\n");
    t=0; b=1; p=0;
    s[1]=0; s[2]=0; s[3]=0;
	fread(&code,sizeof(instruction),cxmax,infile);
	fclose(infile);
	//printf("test");
	/*for(kk=0;kk<200;kk++){
		printf("%10d%5d%3d%10.5f\n", kk,code[kk].f, code[kk].l, code[kk].a);
	}*/
    do{
		if(t>stacksize){
			printf("overflow running_stack\n");
			exit(0);
		}
	i=code[p]; p=p+1;
	switch(i.f){
	    case lit:
		t=t+1; 
		s[t]=i.a;
		break;
	/*	case lir:
			t=t+1;
			s[t]=i.a;
			break;*/
	    case opr:
		switch((long)i.a){ 	// operator
		    case 0:	// return
			t=b-1; p=s[t+3]; b=s[t+2];
			break;
		    case 1:
			s[t]=-s[t];
			break;
		    case 2:
			t=t-1; s[t]=s[t]+s[t+1];
			break;
		    case 3:
			t=t-1; s[t]=s[t]-s[t+1];
			break;
		    case 4:
			t=t-1; s[t]=s[t]*s[t+1];
			break;
		    case 5:     //slash
			t=t-1; s[t]=(long)s[t]/(long)s[t+1];
			break;
		    case 6:
			s[t]=(long)s[t]%2;
			break;
			case 7:
			t--;
			break;
		    case 8:
			t=t-1; s[t]=(s[t]==s[t+1]);
			break;
		    case 9:
			t=t-1; s[t]=(s[t]!=s[t+1]);
			break;
		    case 10:
			t=t-1; s[t]=(s[t]<s[t+1]);
			break;
		    case 11:
			t=t-1; s[t]=(s[t]>=s[t+1]);
			break;
		    case 12:
			t=t-1; s[t]=(s[t]>s[t+1]);
			break;
		    case 13:
			t=t-1; s[t]=(s[t]<=s[t+1]);
			break;
			case 14:   //read指令,读整形3
				j=0;
				getnum=0;
				a=0;
				t++;
				ch=getchar();
				while(ch!=' '&&ch!='.'){
					getnum=getnum*10+(ch-'0');
					j++;
					//scanf("%c",&ch[j]);
					ch=getchar();
				}
				if(ch=='.'){       //说明读入的数是实型 
					tt=1.0;
				//	scanf("%c",&ch[j]);
					ch=getchar();
					while(ch!='\n'){
						tt/=10;
					    getnum=getnum+(ch-'0')*tt;
					    j++;
					    //scanf("%c",&ch[j]);
						ch=getchar();
					}
				}
				s[t]=getnum;
				break;
			case 15:   
				ch=getchar();   //读取换行符
				break;
			case 16:     //write指令
			/*	if((s[t]-(int)s[t])!=0){  //栈顶是实型
				printf("%10.5f",s[t]);
				}
				else{     */  //栈顶是整型
					printf("%5d",(long)s[t]);
					t--;
				//}
				break;
			case 17:
				printf("\n");
				break;
			case 18:     //div
				t=t-1; s[t]=s[t]/s[t+1];
				break;
			case 19:
				t=t-1; s[t]=(long)s[t]%(long)s[t+1];
				break;
			case 20:
				t=t-1; s[t]=(long)s[t]&&(long)s[t+1];
				break;
			case 21:
				t=t-1; s[t]=(long)s[t]||(long)s[t+1];
				break;
			case 22:
				s[t]=!(long)s[t];
				break;
			case 23:
				t++;
				break;
			case 24:
				printf("%5.2f  ",s[t]);
				t--;
				break;

		}
		break;
	    case lod:
		t=t+1; 
		s[t]=s[base(b,i.l)+(long)i.a];
		break;
		case lay:
		//	adr=table[i.a].drt;
			adr=0;
			for(k=0,d=0;k<s[base(b,i.l)+(long)i.a+1];k++)   
                {   
                    ls=s[t-(long)(s[base(b,i.l)+(long)i.a+1]-1)+k];
					d++;
                    if(ls<s[base(b,i.l)+(long)i.a+1+d])  
                    {   
                        printf("array overflow\n");   
                        exit(0);   
                        break;   
                    }
					d++;
					if(ls>s[base(b,i.l)+(long)i.a+1+d]){
			            printf("array overflow\n");   
                       exit(0);
                        break;  
					}	
					if(s[base(b,i.l)+(long)i.a+1]==1||k==0){      //第一维情况
						away=ls-s[base(b,i.l)+(long)i.a+1+1];
					}
					else{
						away=away*(s[base(b,i.l)+(long)i.a+1+d]-s[base(b,i.l)+(long)i.a+1+d-1]+1)+ls-s[base(b,i.l)+(long)i.a+1+d-1]+1;      
						}
                } 
				adr=adr+away;
			    t=t+1-s[base(b,i.l)+(long)i.a+1];
                s[t]=s[base(b,i.l)+(long)i.a+1+2*(long)s[base(b,i.l)+(long)i.a+1]+1+adr];   
				break;
	    case sto:
		s[base(b,i.l)+(long)i.a]=s[t];/* printf("%10d\n", s[t]);*/t=t-1;
		break;
		case say:
          //      adr=table[i.a.longnum].drt;   
		     	adr=0;
				away=0;
                for(k=0,d=0;k<s[base(b,i.l)+(long)i.a+1];k++)  
                {    
					ls=s[t-(long)s[base(b,i.l)+(long)i.a+1]+k];
                    //ls=s[base(b,i.l)+table[i.a.longnum].addr+k];   
                    d++;
                    if(ls<s[base(b,i.l)+(long)i.a+1+d])  
                    {   
                        printf("array overflow\n");   
                        exit(0);  
                        break;   
                    }
					d++;
					if(ls>s[base(b,i.l)+(long)i.a+1+d]){
			            printf("array overflow\n");   
                        exit(0);   
                        break;  
					}	
					
					if(s[base(b,i.l)+(long)i.a+1]==1||k==0){
							away=ls-s[base(b,i.l)+(long)i.a+1+1];;
					}
					else{
						away=away*(s[base(b,i.l)+(long)i.a+1+d]-s[base(b,i.l)+(long)i.a+1+d-1]+1)+ls-s[base(b,i.l)+(long)i.a+1+d-1]+1; 
						}
  
                }  
		        adr=adr+away; 
              //  t--;   
                s[base(b,i.l)+(long)i.a+1+2*(long)s[base(b,i.l)+(long)i.a+1]+1+adr]=s[t];   
				t=t-1-s[base(b,i.l)+(long)i.a+1]; //把要赋的值以及下标出栈
                break;   
	    case cal:		// generate new block mark
		s[t+1]=base(b,i.l);    //将父过程基地址入栈  ,访问链
		s[t+2]=b;              //本过程基地址   控制链
		s[t+3]=p;              //返回地址
		b=t+1; p=i.a;
		break;
	    case Int:
		t=t+i.a;
		break;
	    case jmp:
		p=i.a;
		break;
	    case jpc:
		if(s[t]==0){
		    p=i.a;
		}
		//t=t-1;     //实现表达式短路计算时注释掉此行，若以后函数或啥跳转出现问题在这找
		break;
		case jpq:
			if(s[t]==1){
		    p=i.a;
			}
			break;
	}
    }while(p!=0);
    //printf("end PL/0\n");
	//fclose(FILE *infile);
	system("pause");
}
