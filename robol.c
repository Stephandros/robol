#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "robol.h"
#include "y.tab.h"


/* symbol table */
/* hash a symbol */
static unsigned symhash(char *sym)
{
	unsigned int hash = 0;
	unsigned c;
	
	while(c = *sym++) 
	{
		hash = hash*9 ^ c;
	}
	return hash;
}
int lookup(char* sym)
{
	// int symbol = symhash(sym)%NHASH;
	// return symbol;
	
	//this probabliy ain't good
	
	struct symbol *sp = &symtab[symhash(sym)%NHASH];
	int h = symhash(sym)%NHASH;
	int scount = NHASH; /* how many have we looked at */
	while(--scount >= 0) {
	if(sp->name && !strcmp(sp->name, sym)) { /*return sp;*/ if(sp->value==-1) sp->value = 0; return h; }
	if(!sp->name) { /* new entry */
	sp->name = strdup(sym);
	sp->value = -1;
	// sp->func = NULL;
	// sp->syms = NULL;
	return h;
	//return sp;
	}
	if(++sp >= symtab+NHASH) sp = symtab; /* try the next entry */
	h=(h+1)%NHASH;
	}
	yyerror("symbol table overflow\n");
	abort(); /* tried them all, table is full */
}

int ex(nodeType *p) {
    if (!p) return 0;
    switch(p->type) {
    case typeCon:       return p->con.value;
    case typeId:        return symtab[p->id.i].value;
	case typeFunction: ex(p->opr.op[0]); ex(p->opr.op[1]); return 0;
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:     while(ex(p->opr.op[0])) ex(p->opr.op[1]); return 0;
		case POVTORUVAJ:     while(ex(p->opr.op[0])) ex(p->opr.op[1]); return 0;
		case POVTORUVAJ_2: while((p->opr.op[0])->con.value>0) { ((p->opr.op[0])->con.value)--; ex(p->opr.op[1]); } return 0;
        case IF:        if (ex(p->opr.op[0]))
                            ex(p->opr.op[1]);
                        else if (p->opr.nops > 2)
                            ex(p->opr.op[2]);
                        return 0;
        case PRINT:     if(p->opr.op[0]->id.s_type==0) printf("%d\n", ex(p->opr.op[0])); 
		else switch(ex(p->opr.op[0])){
		case 0: printf("I"); break;
		case 1: printf("Z"); break;	
		case 2: printf("S"); break;
		case 3: printf("J"); break;
		}//ovde treba da se smeni ne sekogas e id
		return 0;
        case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
		case NEWLINE:       ex(p->opr.op[0]); return ex(p->opr.op[1]);
		case OSNOVNA_KOMANDA: printf("%d\n",p->opr.oper); ex(p->opr.op[0]); return ex(p->opr.op[1]);
        case '=':       return (symtab[p->opr.op[0]->id.i].value) = ex(p->opr.op[1]);
        case UMINUS:    return -ex(p->opr.op[0]);
        case '+':       return ex(p->opr.op[0]) + ex(p->opr.op[1]);
        case '-':       return ex(p->opr.op[0]) - ex(p->opr.op[1]);
        case '*':       return ex(p->opr.op[0]) * ex(p->opr.op[1]);
        case '/':       return ex(p->opr.op[0]) / ex(p->opr.op[1]);
        case '<':       return ex(p->opr.op[0]) < ex(p->opr.op[1]);
        case '>':       return ex(p->opr.op[0]) > ex(p->opr.op[1]);
        case GE:        return ex(p->opr.op[0]) >= ex(p->opr.op[1]);
        case LE:        return ex(p->opr.op[0]) <= ex(p->opr.op[1]);
        case NE:        return ex(p->opr.op[0]) != ex(p->opr.op[1]);
        case EQ:        return ex(p->opr.op[0]) == ex(p->opr.op[1]);
        }
    }
    return 0;
}

int a;
int b;
int gn=0;
//0 means const, 1 means expression
int translate(nodeType *p){
	int a;
	int b;
	int k;
	char str[15];
	char str2[15];

	if (!p) return 0;
	switch(p->type) {
		case typeCon:
			if(gn==0) printf("move nx %d\n",p->con.value);
			if(gn==1) printf("move mx %d\n",p->con.value);
			gn++; gn=gn%2;
			return 0;
		case typeOpr:
			switch(p->opr.oper) {
				case '*':
					a=p->opr.op[0]->type;
					b=p->opr.op[1]->type;

					if(a==typeCon&&b==typeOpr) {
						b=translate(p->opr.op[1]);
						a=translate(p->opr.op[0]);
					}
					else {
						a=translate(p->opr.op[0]);
						b=translate(p->opr.op[1]);
					}

					if(a==0&&b==0){
						printf("mul nx mx\n");
						printf("push\n");
					} else if(a==0){
						printf("move mx nx\n");
						printf("pop\n");
						printf("mul mx nx\n");
						printf("push\n");
					}
					else if (b==0){
						printf("move mx nx\n");
						printf("pop\n");
						printf("mul mx nx\n");
						printf("push\n");
					} else {
						printf("pop\n");
						printf("move mx nx\n");
						printf("pop\n");
						printf("mul mx nx\n");
						printf("push\n");

					}
					printf("\n");
					gn=0;
					return 1;

				case '+':
					a = translate(p->opr.op[0]);
					b = translate(p->opr.op[1]);
					if(a==0&&b==0){
						printf("add nx mx\n");
						printf("push\n");
					} else if(a==0){
						printf("move mx nx\n");
						printf("pop\n");
						printf("add mx nx\n");
						printf("push\n");
					}
					else if (b==0){
						printf("move mx nx\n");
						printf("pop\n");
						printf("add mx nx\n");
						printf("push\n");
					} else {
						printf("pop\n");
						printf("move mx nx\n");
						printf("pop\n");
						printf("add mx nx\n");
						printf("push\n");

					}
					printf("\n");
					gn=0; return 1;
			}
	}
	return 0;


}
