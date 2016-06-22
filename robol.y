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
//struct symbol symtab[NHASH];
struct symbol *symtab;
struct symbol *globsym;



//int symtab[NHASH];                    /* symbol table */
%}

%union {
    int iValue;                 /* integer value */
    char sIndex;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */
};

%error-verbose

%token <iValue> INTEGER NASOKA
%token <iValue> VARIABLE
%token WHILE IF PRINT
%token NEWLINE POVTORUVAJ DO PATI POCETOK KRAJ T_BROJ T_NASOKA POVIKAJ SVRTILEVO SVRTIDESNO ZEMI OSTAVI ODI
%token POVTORUVAJ_2 PROCEDURA
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

//%type <nPtr> stmt stmt_list
%type <nPtr> komanda komanda_list expr
%type <nPtr> function osnovna_komanda

%type <nPtr> decl varlist
%type <iValue> poc


%%

program:
        /*PROCEDURA '('')' NEWLINE POCETOK NEWLINE function KRAJ             { //exit(0); 
		}*/
		
		  eden {/**/}
		  | error {printf("ERROR!!!");}
        ;
		
		
eden: 
	eden nesto
	|
	;

poc:
    PROCEDURA VARIABLE NEWLINE {
        symtab[$2].declared= 1;
        globsym=symtab;
        $$=$2;
        globsym[$2].symboltable = malloc(NHASH*sizeof(struct symbol));
        symtab = globsym[$2].symboltable;
        }
    ;
		
nesto:
		poc POCETOK NEWLINE function KRAJ  {
            /*symtab[$1].op[0]= $4;*/
            globsym[$1].op[0]=$4;
            translate($4);
            symtab =symboltable;
		    }
		 | command

		;

command:
        komanda { ex($1); translate($1);  freeNode($1); }
        |decl { ex($1); freeNode($1); }
        | POVIKAJ VARIABLE {
                 if(symtab[$2].declared==0)
                        yyerror("not declared");
                    globsym = symtab;
                    symtab = globsym[$2].symboltable;
                    ex(globsym[$2].op[0]);
                    //printf("%d, %d\n",globsym[$2].op[0]->type,typeFunction);
                    //translate(globsym[$2].op[0]);

                    /*freeNode($1);*/
                    symtab =symboltable;
         }

		
function:
          function komanda       { /*printf("in here");*/ $$ = fct($1,$2); }
          | function decl  { /*printf("in here");*/ $$ = fct($1,NULL); }
        | /* NULL */ {$$=NULL;}
		| error {/*printf("ERROR!!!");*/}
        ;



decl:
     T_BROJ varlist {$$=$2;}
     | T_NASOKA varlist {$$=$2;}
    ;
varlist:
    VARIABLE { symtab[$1].declared = 1; symtab[$1].tip=$<iValue>0; printf("data %s\n",symtab[$1].name);$$ = id($1,$<iValue>0); }
    | varlist ',' VARIABLE { symtab[$3].declared = 1; symtab[$3].tip=$<iValue>0; printf("data %s\n",symtab[$3].name); $$ = opr(';',2,$1, id($3,$<iValue>0)); }
    ;
	
osnovna_komanda:
        ODI {$$=opr(ODI, 2, NULL, NULL);}
        |ZEMI {$$=opr(ZEMI, 2, NULL, NULL);}
        |OSTAVI {$$=opr(OSTAVI, 2, NULL, NULL);}
        |SVRTILEVO {$$=opr(SVRTILEVO, 2, NULL, NULL);}
        |SVRTIDESNO {$$=opr(SVRTIDESNO, 2, NULL, NULL);}
		;
komanda:
		  NEWLINE                            { $$ = opr(NEWLINE, 2, NULL, NULL); }
		| osnovna_komanda NEWLINE		  	 { $$ = $1;  }
        | expr NEWLINE                       { $$ = $1; }
        | PRINT expr NEWLINE                 { /*if($2==NULL) {yyerror("not declared"); $$=NULL;}*/
                                                   /*printf("we are here");*/ $$ = opr(PRINT, 1, $2);

                                                    }
        //| decl NEWLINE                              {$$ = $1;}
		//| VARIABLE ':' T_BROJ NEWLINE			 { symtab[$1].declared = 1; symtab[$1].tip=0; $$ = id($1,0); }
		//| VARIABLE ':' T_NASOKA NEWLINE			 { symtab[$1].declared = 1; symtab[$1].tip=1; $$ = id($1,1); }
        | VARIABLE '=' expr NEWLINE  {
            if(symtab[$1].declared==0){
                yyerror("not declared");
                $$ = NULL;
                }
            else if(symtab[$1].tip!=getType($3)){
                printf("type is %d %d\n",getType($3),symtab[$1].tip);
                yyerror("type mismatch");
                $$ = NULL;
                }
                else{
            $$ = opr('=', 2, id($1,symtab[$1].tip), $3); }}
		| VARIABLE '=' NASOKA NEWLINE {
            if(symtab[$1].declared==0)
                yyerror("not declared");
            if(symtab[$1].tip==0)
                yyerror("not a nasoka");
            $$ = opr('=', 2, id($1,symtab[$1].tip), con($3,1));
            }
		| POVTORUVAJ DO '(' expr ')' NEWLINE '!' NEWLINE komanda_list '!' NEWLINE  { $$ = opr(POVTORUVAJ, 2, $4, $9); }
		| POVTORUVAJ INTEGER PATI NEWLINE '!' NEWLINE komanda_list '!' NEWLINE  { $$ = opr(POVTORUVAJ_2, 2, con($2,0), $7); }
        | IF '(' expr ')' NEWLINE '!' NEWLINE komanda_list '!' %prec IFX NEWLINE { $$ = opr(IF, 2, $3, $8); }
        | IF '(' expr ')' NEWLINE '!' NEWLINE komanda_list '!' NEWLINE ELSE NEWLINE '!' NEWLINE komanda_list '!' NEWLINE { $$ = opr(IF, 3, $3, $8, $15); }
        ;

komanda_list:
          komanda                  { $$ = $1;  }
        | komanda_list komanda       { $$ = opr(';', 2, $1, $2); }
        ;


expr:
          INTEGER               { $$ = con($1,0); }
        | VARIABLE              { if(symtab[$1].declared==0) yyerror("not declared");/*printf("here in var");*/ $$ = id($1,symtab[$1].tip); }
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         { if(getType($1)!=getType($3)) yyerror("type doesn't match");  $$ = oprt('+',getType($1), 2, $1, $3); }
        | expr '-' expr         { if(getType($1)!=getType($3)) yyerror("type doesn't match");  $$ = oprt('-',getType($1), 2, $1, $3);  }
        | expr '*' expr         { if(getType($1)!=getType($3)) yyerror("type doesn't match");  $$ = oprt('*',getType($1), 2, $1, $3);  }
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
    symtab = symboltable;

    FILE *fl;
    fl = fopen("output.txt","w+");

    if(fl==NULL){
        printf("cannot open file\n");
    }
    else {
        fprintf(fl,"print this to file");
        printf("printing successful\n");
    }
    fclose(fl);
    yyparse();
    return 0;
}
