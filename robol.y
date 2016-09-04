%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
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
//int env[100][100];
extern int rows;
extern int cols;
int i,j;
extern int ri,rj,rn;

/* stack functions */
int isempty();
int isfull();
int peek();
int pop();
int push(int data);
void emptyStack();

int popValue;
int popTip;
int count;

FILE *flin;
extern FILE *flout, *flrimal, *flerror, *flcommands, *flokolina;
nodeType *fct(nodeType *l,nodeType *r);

char *error_string;
void yyerror(char *s);
//struct symbol symtab[NHASH];
struct symbol *symtab;
struct symbol *globsym;
//int symtab[NHASH];                    /* symbol table */
extern int yylineno;
%}

%union {
    int iValue;                 /* integer value */
    char sIndex;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */
};

%error-verbose

%token <iValue> INTEGER NASOKA PRAVEC
%token <iValue> VARIABLE
%token WHILE IF PRINT
%token NEWLINE POVTORUVAJ DO PATI POCETOK KRAJ T_BROJ T_NASOKA POVIKAJ SVRTILEVO SVRTIDESNO ZEMI OSTAVI ODI END
%token POVTORUVAJ_2 PROCEDURA OKOLINA ZETONI DZIDOVI ROBOT
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

//%type <nPtr> stmt stmt_list
%type <nPtr> komanda komanda_list expr
%type <nPtr> function osnovna_komanda paramlist /*declarations*/ functionCall

%type <nPtr> /*decl varlist*/ declVar listOfVarables
%type <iValue> poc tip


%%

program:
		/*defOkolina NEWLINE END*/ eden END { return 0;};
    ;

eden:
	eden nesto
	| /*NULL*/
	| error NEWLINE { yyerror("ERROR!");}
	;


// a command or a procedure definition
nesto:
    //this is the declaration of a function
		poc listaNaParametri POCETOK NEWLINE function KRAJ  {
            /*symtab[$1].op[0]= $5;*/
            globsym[$1].op[0]=$5;
            translate($5);
            symtab =symboltable;
		    }
		 | command
		;
listaNaParametri:
    '(' paramlist ')' {
                    //fprintf(flout,"this is the list of procedure with name %s\n",globsym[$<iValue>0].name);
                    count=0;

                    while(!isempty()){
                     popValue=pop();
                     popTip = pop();
                     symtab[popValue].declared = 1;
                     symtab[popValue].tip=popTip;
                     globsym[$<iValue>0].paramlist[count] = popValue;
                     count++;
                     }
                     globsym[$<iValue>0].noParams = count;

    }
    ;

poc:
    PROCEDURA VARIABLE {
        symtab[$2].declared= 1;
        globsym=symtab;
        globsym[$2].symboltable = malloc(NHASH*sizeof(struct symbol));
        symtab = globsym[$2].symboltable;
        $$ = $2;
    }
    ;

paramlist:
    tip VARIABLE { push($1); push($2); $$ = id($2,$<iValue>1);}
    | tip VARIABLE ',' paramlist {  push($1); push($2); $$ = opr(';',2,$2, id($2,$<iValue>1)); }
    ;

tip:
    T_BROJ {$$=$<iValue>1;}
    | T_NASOKA {$$ = $<iValue>1;}
    ;

valueList:
    NASOKA { push($1);}
    |
    INTEGER { push($1);}
    |
    valueList ',' NASOKA { push($3);}
    |
    valueList ',' INTEGER { push($3);}
    ;

command:
        komanda {  if($1!=NULL){
          ex($1); translate($1); freeNode($1);}
        }
;


functionCall:
    POVIKAJ VARIABLE valueList{
       if(symtab[$2].declared==0) {yyerror("not declared"); $$=NULL;}
       else {
          $$ = id($2,2);
        }
      }
    ;
// declarations:
//  |  decl { ex($1); freeNode($1); $$=NULL;}
//  | declVar { ex($1); freeNode($1); $$=NULL;}
//  ;
declVar:
    listOfVarables ':' T_BROJ {
       while(!isempty()){
            popValue = pop();
            //fprintf(flout,"type of variable %s is %d",symtab[popValue].name,$<iValue>3);
            symtab[popValue].tip=$<iValue>3;
            symtab[popValue].value=0;
       }
       $$=$1;
     }
    | listOfVarables ':' T_NASOKA {
        while(!isempty()){
            popValue = pop();
            //fprintf(flout,"type of variable %s is %d",symtab[popValue].name,$<iValue>3);
            symtab[popValue].tip=$<iValue>3;
            symtab[popValue].value=0;
       }
    $$=$1;
    }
    ;
listOfVarables:
     VARIABLE { symtab[$1].declared = 1; push($1); /*$$ = id($1,$<iValue>0);*/ $$=NULL; }
     | listOfVarables ',' VARIABLE { symtab[$3].declared = 1; push($3); /*$$ = opr(';',2,$1, id($3,$<iValue>0));*/ $$=NULL; }
     ;


function:
          function komanda       {  $$ = fct($1,$2); }

        | /* NULL */ {$$=NULL;}
		| error {/*printf("ERROR!!!");*/}
        ;


/*
decl:
     T_BROJ varlist {$$=$2;}
     | T_NASOKA varlist {$$=$2;}
    ;*/
/*
varlist:
    VARIABLE { symtab[$1].declared = 1; symtab[$1].tip=$<iValue>0;
    fprintf(flout,"data %s\n",symtab[$1].name);
    fflush(flout);
    $$ = id($1,$<iValue>0);
    }
    | varlist ',' VARIABLE {
    symtab[$3].declared = 1;
    symtab[$3].tip=$<iValue>0;
    printf("data %s\n",symtab[$3].name);
    $$ = opr(';',2,$1, id($3,$<iValue>0)); }
    ;
*/
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
    | PRINT expr NEWLINE                 {
                                                if($2==NULL) { $$=NULL;}
                                               else {
                                               $$ = opr(PRINT, 1, $2); }
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
                else
            if(symtab[$1].tip==0)
                yyerror("not a nasoka");
                else
            $$ = opr('=', 2, id($1,symtab[$1].tip), con($3,1));
            }
		| POVTORUVAJ DO '(' expr ')' NEWLINE '!' NEWLINE komanda_list '!' NEWLINE  { $$ = opr(POVTORUVAJ, 2, $4, $9); }
		| POVTORUVAJ expr PATI NEWLINE '!' NEWLINE komanda_list '!' NEWLINE  { $$ = opr(POVTORUVAJ_2, 2, $2, $7); }
        | IF '(' expr ')' NEWLINE '!' NEWLINE komanda_list '!' %prec IFX NEWLINE { $$ = opr(IF, 2, $3, $8); }
        | IF '(' expr ')' NEWLINE '!' NEWLINE komanda_list '!' NEWLINE ELSE NEWLINE '!' NEWLINE komanda_list '!' NEWLINE { $$ = opr(IF, 3, $3, $8, $15); }
        | functionCall {$$=$1;}
        | declVar { $$=NULL;}
        ;



komanda_list:
          komanda                  { $$ = $1;  }
        | komanda_list komanda     { $$ = opr(';', 2, $1, $2); }
        ;


expr:
          INTEGER               { $$ = con($1,0); }
        | VARIABLE              {
          if(symtab[$1].declared==0)
          {  sprintf(error_string, "Variable %s not declared", symtab[$1].name);  yyerror(error_string); $$ = NULL; }
          else $$ = id($1,symtab[$1].tip);
        }
        | '-' expr %prec UMINUS {
          if($2==NULL) {
            yyerror("Cannot evaluate expression.");
            $$=NULL;
          } else $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         {
          if(hasError($1,$3))
            $$=NULL;
          else $$ = oprt('+',getType($1), 2, $1, $3);
        }
        | expr '-' expr         {
          if(hasError($1,$3))
            $$=NULL;
          else $$ = oprt('-',getType($1), 2, $1, $3);
        }
        | expr '*' expr         {
          if(hasError($1,$3))
            $$=NULL;
          else $$ = oprt('*',getType($1), 2, $1, $3);
        }
        | expr '/' expr         {
          if(hasError($1,$3))
            $$=NULL;
          else $$ = oprt('/',getType($1), 2, $1, $3);  }
        | expr '<' expr         {
          if(hasError($1,$3))
            $$=NULL;
          else $$ = oprt('<',getType($1), 2, $1, $3);  }
        | expr '>' expr         {
          if(hasError($1,$3))
            $$=NULL;
          else  $$ = oprt('>',getType($1), 2, $1, $3);  }
        | expr GE expr          {
          if(hasError($1,$3))
            $$=NULL;
          else  $$ = oprt(GE,3, 2, $1, $3);  }
        | expr LE expr          {
          if(hasError($1,$3))
            $$=NULL;
          else  $$ = oprt(LE,3, 2, $1, $3);  }
        | expr NE expr          {
          if(hasError($1,$3))
            $$=NULL;
          else  $$ = oprt(NE, 3, 2, $1, $3);  }
        | expr EQ expr          {
          if(hasError($1,$3))
            $$=NULL;
          else   $$ = oprt(EQ,3, 2, $1, $3);  }
        | '(' expr ')'          { $$ = $2; }
        ;

%%

int hasError(nodeType *l, nodeType *r){
  if(l==NULL || r==NULL) {
    yyerror("Cannot evaluate expression");
    return 1;
    }
    else if(getType(l)!=getType(r)) {
      yyerror("Type doesn't match");
      return 1;
    }

  return 0;
}

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
    fprintf(flerror, "%s around line %d\n", s,yylineno);
}

#define ROBOT   1
#define COIN    2
#define BARRIER 3

extern int ENVIRONMENT[100][100];
int prev_dim = 0;

void setup_environment(){
    char environment[]     = "Okolina";
    char barriers[]        = "Zidovi";
    char coins[]           = "Zetoni";
    char robot[]           = "Robot";
    char robot_direction[] = "Nasoka Robot";
    char end[]             = "kraj";

    FILE *fp;
    if((fp = fopen("okolina.env", "r")) == NULL)
    {
        fprintf(flout,"Cannot open okolina.env!");
        return;
    }

    char line[100];
    int i, j;

    while(fgets(line, sizeof(line), fp))
    {
        if(strncmp(line, environment, 7) == 0)
        {
            fgets(line, sizeof(line), fp);
            char * split;
            split = strtok(line, " ");
            int row_count = atoi(split);
            split = strtok(NULL, " ");
            int col_count = atoi(split);

            rows=cols=row_count;
            /*for (i = 0; i < prev_dim; ++i)
                delete [] ENVIRONMENT[i];
            delete [] ENVIRONMENT;*/

            prev_dim = row_count;

            // For simplicity our enviroment will have square base.
            /*ENVIRONMENT = new int*[row_count];
            for (i = 0; i < row_count; ++i)
                ENVIRONMENT[i] = new int[row_count];
            for (i = 0; i < row_count; ++i)
                for (j = 0; j < row_count; ++j)
                    ENVIRONMENT[i][j] = 0;*/
        }
        else if(strncmp(line, barriers, 6) == 0)
        {
            fgets(line, sizeof(line), fp);
            fgets(line, sizeof(line), fp);
            int count = 0;
            do
            {
                char * split;
                split = strtok(line, " ");
                int direction = atoi(split);
                split = strtok(NULL, " ");
                int location = atoi(split);
                split = strtok(NULL, " ");
                int start = atoi(split);
                split = strtok(NULL, " ");
                int end = atoi(split);
                int tmp;

                if(direction==1) {
                  start = rows - start;
                  end = rows - end;
                  tmp = start;
                  start = end;
                  end = tmp;
                } else if(direction==0){
                  location = rows - (location + 1);
                }

                for(i = start; i < end; ++i)
                {
                    if(direction == 0)                      // HORIZONTAL
                        ENVIRONMENT[location][i] = BARRIER;
                    else if(direction == 1)                 // VERTICAL
                    ENVIRONMENT[i][location] = BARRIER;
                }

                fgets(line, sizeof(line), fp);
            } while(strncmp(line, end, 4) != 0);
        }
        else if(strncmp(line, coins, 6) == 0)
        {

            fgets(line, sizeof(line), fp);
            fgets(line, sizeof(line), fp);
            int count = 0;
            do
            {
                char * split;
                split = strtok(line, " ");
                j = atoi(split);
                split = strtok(NULL, " ");
                i = atoi(split);
                i=rows-(i+1);
                ENVIRONMENT[i][j] = COIN;

                fgets(line, sizeof(line), fp);
            } while(strncmp(line, end, 4) != 0);
        }
        else if(strncmp(line, robot, 5) == 0)
        {
            fgets(line, sizeof(line), fp);
            char * split;
            split = strtok(line, " ");
            j = atoi(split);
            split = strtok(NULL, " ");
            i = atoi(split);
            i=rows-(i+1);
            ri=i;
            rj=j;
            //ENVIRONMENT[i][j] = ROBOT;
        }

        else if(strncmp(line, robot_direction, 12) == 0)
        {
            fgets(line, sizeof(line), fp);
            rn = atoi(line);
        }

    }

    fclose(fp);
}

int main(void) {

    symtab = symboltable;
    error_string = (char*)malloc(50 * sizeof(char));
    extern FILE* yyin;
    flout = fopen("printingOutput.txt","w+");
    flin = fopen("input.txt","r");
    flrimal = fopen("rimalOutput.txt","w+");
    flerror = fopen("errorOutput.txt","w+");
    flcommands = fopen("commandsOutput.txt","w+");
    char buff[255];

    setup_environment();
    yyin=flin;
    yyparse();

    fprintf(flout,"End of program!");

    fclose(flout);
    fclose(flin);
    fclose(flrimal);
    fclose(flerror);
    fclose(flcommands);

    return 0;

}
