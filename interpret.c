#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include<ctype.h>
#include "pl0.h"

long base(long b, long l){   //���ط��������±�
    long b1;
    b1=b;
    while (l>0){	// find base l levels down
	b1=s[b1]; l=l-1;     //���ݷ��������ϻ����ҵ�����
    }
    return b1;
}


void main(){
	FILE *infile=fopen("pcode2byte.txt","rb");
    long p,b,t;		// ָ�������-,ջ��ַ�Ĵ���,ջ��ָ��
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
			case 14:   //readָ��,������3
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
				if(ch=='.'){       //˵�����������ʵ�� 
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
				ch=getchar();   //��ȡ���з�
				break;
			case 16:     //writeָ��
			/*	if((s[t]-(int)s[t])!=0){  //ջ����ʵ��
				printf("%10.5f",s[t]);
				}
				else{     */  //ջ��������
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
					if(s[base(b,i.l)+(long)i.a+1]==1||k==0){      //��һά���
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
				t=t-1-s[base(b,i.l)+(long)i.a+1]; //��Ҫ����ֵ�Լ��±��ջ
                break;   
	    case cal:		// generate new block mark
		s[t+1]=base(b,i.l);    //�������̻���ַ��ջ  ,������
		s[t+2]=b;              //�����̻���ַ   ������
		s[t+3]=p;              //���ص�ַ
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
		//t=t-1;     //ʵ�ֱ��ʽ��·����ʱע�͵����У����Ժ�����ɶ��ת��������������
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
