%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "robol.h"

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *oprt(int oper, int t, int nops, ...);
nodeType *id(int i, int t);
nodeType *con(int value, int t);
nodeEnum getType(nodeType *p);
void freeNode(nodeType *p);
int ex(nodeType *p);
int yylex(void);

nodeType *fct(nodeType *l,nodeType *r);

void yyerror(char *s);
struct symbol symtab[NHASH];
//int symtab[NHASH];                    /* symbol table */
%}

%union {
    int iValue;                 /* integer value */
    char sIndex;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */
};

%error-verbose

%token <iValue> INTEGER NASOKA
%token <sIndex> VARIABLE
%token WHILE IF PRINT
%token NEWLINE POVTORUVAJ DO PATI OSNOVNA_KOMANDA PROCEDURA POCETOK KRAJ T_BROJ T_NASOKA CALL
%token POVTORUVAJ_2
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

//%type <nPtr> stmt stmt_list
%type <nPtr> komanda komanda_list expr
%type <nPtr> function 


%%

program:
        /*PROCEDURA '('')' NEWLINE POCETOK NEWLINE function KRAJ             { //exit(0); 
		}*/
		
		  eden {}
        ;
		
		
eden: 
	eden nesto
	|
	;
		
nesto:
		POCETOK VARIABLE function KRAJ  { symtab[$2].declared= 1; symtab[$2].op[0]= $3; }
		| function2
		| CALL VARIABLE { if(symtab[$2].declared==0) yyerror("not declared"); ex(symtab[$2].op[0]); /*freeNode($1);*/ }
		;

		
function:
          function komanda       { $$ = fct($1,$2); }
        | /* NULL */ {$$=NULL;}
		| error {printf("ERROR!!!");}
        ;
		
function2:
		komanda { ex($1); freeNode($1); }
		;
	

		
komanda:
		  NEWLINE                            { $$ = opr(NEWLINE, 2, NULL, NULL); }
		| OSNOVNA_KOMANDA NEWLINE		  	 { $$ = opr(OSNOVNA_KOMANDA, 2, NULL, NULL); }
        | expr NEWLINE                       { $$ = $1; }
        | PRINT expr NEWLINE                 { $$ = opr(PRINT, 1, $2); }
		| VARIABLE ':' T_BROJ NEWLINE			 { symtab[$1].declared = 1; symtab[$1].tip=0; $$ = id($1,0); }
		| VARIABLE ':' T_NASOKA NEWLINE			 { symtab[$1].declared = 1; symtab[$1].tip=1; $$ = id($1,1); }
        | VARIABLE '=' expr NEWLINE          
		{ 
		if(symtab[$1].declared==0) 
			yyerror("not declared"); 
		if(symtab[$1].tip!=getType($3)) 
			yyerror("type mismatch");  
		$$ = opr('=', 2, id($1,symtab[$1].tip), $3);  
		}
		| VARIABLE '=' NASOKA NEWLINE      { if(symtab[$1].declared==0) yyerror("not declared"); if(symtab[$1].tip==0) yyerror("not a nasoka"); $$ = opr('=', 2, id($1,symtab[$1].tip), con($3,1));}
		| POVTORUVAJ DO '(' expr ')' NEWLINE '!' NEWLINE komanda_list '!' NEWLINE  { $$ = opr(POVTORUVAJ, 2, $4, $9); }
		| POVTORUVAJ INTEGER PATI NEWLINE '!' NEWLINE komanda_list '!' NEWLINE  { $$ = opr(POVTORUVAJ_2, 2, con($2,0), $7); }
        | IF '(' expr ')' '!' komanda_list '!' %prec IFX NEWLINE { $$ = opr(IF, 2, $3, $6); }
        | IF '(' expr ')' '!' komanda_list '!' ELSE '!' komanda_list '!' NEWLINE { $$ = opr(IF, 3, $3, $6, $10); }
        ;

komanda_list:
          komanda                  { $$ = $1;  }
        | komanda_list komanda       { $$ = opr(';', 2, $1, $2); }
        ;
/*
stmt:
          ';'                            { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                       { $$ = $1; }
        | PRINT expr ';'                 { $$ = opr(PRINT, 1, $2); }
        | VARIABLE '=' expr ';'          { $$ = opr('=', 2, id($1), $3); }
        | WHILE '(' expr ')' stmt        { $$ = opr(WHILE, 2, $3, $5); }
		| POVTORUVAJ '(' expr ')' '!' stmt_list '!'   { $$ = opr(POVTORUVAJ, 2, $3, $6); }
        | IF '(' expr ')' stmt %prec IFX { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'              { $$ = $2; }
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;
*/

expr:
          INTEGER               { $$ = con($1,0); }
        | VARIABLE              { if(symtab[$1].declared==0) yyerror("not declared"); $$ = id($1,symtab[$1].tip); }
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         { if(getType($1)!=getType($3)) yyerror("type doesn't match");  $$ = oprt('+',getType($1), 2, $1, $3); }
        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
        | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { if(getType($1)!=getType($3)) yyerror("type doesn't match"); $$ = opr(EQ, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        ;

%%

nodeEnum getType(nodeType *p){

	if(p->type==typeCon)
		return p->con.s_type;
	else if(p->type==typeOpr)
		return p->opr.s_type;
	else if(p->type==typeId)
		return p->id.s_type;
}

nodeType *con(int value, int t) {
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    p->con.value = value;
	p->con.s_type = t;

    return p;
}

nodeType *id(int i, int t) {
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.i = i;
	p->id.s_type = t;

    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    int i;

    /* allocate node, extending op array */
    if ((p = malloc(sizeof(nodeType) + (nops-1) * sizeof(nodeType *))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}


nodeType *oprt(int oper, int t, int nops, ...) {
    va_list ap;
    nodeType *p;
    int i;

    /* allocate node, extending op array */
    if ((p = malloc(sizeof(nodeType) + (nops-1) * sizeof(nodeType *))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
	p->opr.s_type = t;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

nodeType *fct(nodeType *l, nodeType *r) {
    nodeType *p;
    int i;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeFunction;
    p->opr.op[0] = l;
	p->opr.op[1] = r;
    
	return p;
}



void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}
