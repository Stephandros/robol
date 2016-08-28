#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "robol.h"
#include "y.tab.h"


FILE *flout, *flrimal, *flerror, *flcommands;


/* symbol table */
/* hash a symbol */
static unsigned symhash(char *sym) {
    unsigned int hash = 0;
    unsigned c;

    while (c = *sym++) {
        hash = hash * 9 ^ c;
    }
    return hash;
}

int lookup(char *sym) {
    // int symbol = symhash(sym)%NHASH;
    // return symbol;

    //this probabliy ain't good

    struct symbol *sp = &symtab[symhash(sym) % NHASH];
    int h = symhash(sym) % NHASH;
    int scount = NHASH; /* how many have we looked at */
    while (--scount >= 0) {
        if (sp->name && !strcmp(sp->name, sym)) { /*return sp;*/ if (sp->value == -1) sp->value = 0;
            return h;
        }
        if (!sp->name) { /* new entry */
            sp->name = strdup(sym);
            sp->value = -1;
            // sp->func = NULL;
            // sp->syms = NULL;
            return h;
            //return sp;
        }
        if (++sp >= symtab + NHASH) sp = symtab; /* try the next entry */
        h = (h + 1) % NHASH;
    }
    yyerror("symbol table overflow\n");
    abort(); /* tried them all, table is full */
}

int counter;

int ex(nodeType *p) {
    if (!p) return 0;
    switch (p->type) {
        case typeCon:
            return p->con.value;
        case typeId:
            return symtab[p->id.i].value;
        case typeFunction:
            ex(p->opr.op[0]);
            ex(p->opr.op[1]);
            return 0;
        case typeOpr:
            switch (p->opr.oper) {
                case WHILE:
                    while (ex(p->opr.op[0])) ex(p->opr.op[1]);
                    return 0;
                case POVTORUVAJ:
                    while (ex(p->opr.op[0])) ex(p->opr.op[1]);
                    return 0;
                case POVTORUVAJ_2:
                    counter = (p->opr.op[0])->con.value;
                    while (counter > 0) {
                        counter--;
                        ex(p->opr.op[1]);
                    }
                    return 0;
                case IF:
                    if (ex(p->opr.op[0])) ex(p->opr.op[1]); else if (p->opr.nops > 2) ex(p->opr.op[2]);
                    return 0;
                case PRINT:
                    /*printing nasoka constant is not implemented */
                    if (p->opr.op[0]->type == 1 && p->opr.op[0]->id.s_type == 1) {
                        switch (ex(p->opr.op[0])) {
                            case 0:
                                fprintf(flout, "I\n");
                                break;
                            case 1:
                                fprintf(flout, "Z\n");
                                break;
                            case 2:
                                fprintf(flout, "S\n");
                                break;
                            case 3:
                                fprintf(flout, "J\n");;
                                break;
                        }
                    } else {
                        fprintf(flout, "%d\n", ex(p->opr.op[0]));
                    }
                    return 0;
                case ';':
                    ex(p->opr.op[0]);
                    return ex(p->opr.op[1]);
                case NEWLINE:
                    ex(p->opr.op[0]);
                    return ex(p->opr.op[1]);
                case ODI: fprintf(flcommands, "GO\n"); ex(p->opr.op[0]);
                    return ex(p->opr.op[1]);
                case ZEMI: fprintf(flcommands, "TK\n"); ex(p->opr.op[0]);
                    return ex(p->opr.op[1]);
                case OSTAVI: fprintf(flcommands, "LV\n"); ex(p->opr.op[0]);
                    return ex(p->opr.op[1]);
                case SVRTIDESNO: fprintf(flcommands, "RR\n"); ex(p->opr.op[0]);
                    return ex(p->opr.op[1]);
                case SVRTILEVO: fprintf(flcommands, "RL\n"); ex(p->opr.op[0]);
                    return ex(p->opr.op[1]);
                case '=':
                    return (symtab[p->opr.op[0]->id.i].value) = ex(p->opr.op[1]);
                case UMINUS:
                    return -ex(p->opr.op[0]);
                case '+':
                    return ex(p->opr.op[0]) + ex(p->opr.op[1]);
                case '-':
                    return ex(p->opr.op[0]) - ex(p->opr.op[1]);
                case '*':
                    return ex(p->opr.op[0]) * ex(p->opr.op[1]);
                case '/':
                    return ex(p->opr.op[0]) / ex(p->opr.op[1]);
                case '<':
                    return ex(p->opr.op[0]) < ex(p->opr.op[1]);
                case '>':
                    return ex(p->opr.op[0]) > ex(p->opr.op[1]);
                case GE:
                    return ex(p->opr.op[0]) >= ex(p->opr.op[1]);
                case LE:
                    return ex(p->opr.op[0]) <= ex(p->opr.op[1]);
                case NE:
                    return ex(p->opr.op[0]) != ex(p->opr.op[1]);
                case EQ:
                    return ex(p->opr.op[0]) == ex(p->opr.op[1]);
            }
    }
    return 0;
}

int a;
int b;
int gn = 0;

//0 means const, 1 means expression
int translate(nodeType *p) {
    int a;
    int b;
    int k;
    char str[15];
    char str2[15];

    if (!p) return 0;
    switch (p->type) {
        case typeCon:
            if (gn == 0) { fprintf(flrimal, "move nx %d\n", p->con.value); }
            if (gn == 1) { fprintf(flrimal, "move mx %d\n", p->con.value); }
            gn++;
            gn = gn % 2;
            return 0;

        case typeFunction:
            //printf("inhere\n");
            translate(p->opr.op[0]);
            translate(p->opr.op[1]);
            return 1;

        case typeId:
            if (gn == 0) fprintf(flrimal, "move nx %s\n", symtab[p->id.i].name);
            if (gn == 1) fprintf(flrimal, "move mx %s\n", symtab[p->id.i].name);
            gn++;
            gn = gn % 2;
            return 0;

        case typeOpr:
            switch (p->opr.oper) {
                case '=':
                    if (p->opr.op[1]->type == typeCon) {
                        a = ex(p->opr.op[1]);
                        fprintf(flrimal,"move %s %d\n", symtab[p->opr.op[0]->id.i].name, a);
                    }
                    else {
                        translate((p->opr.op[1]));
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"move %s nx\n", symtab[p->opr.op[0]->id.i].name);
                    }
                    return 1;
                case '*':
                    a = p->opr.op[0]->type;
                    b = p->opr.op[1]->type;

                    if (a == typeCon && b == typeOpr) {
                        b = translate(p->opr.op[1]);
                        a = translate(p->opr.op[0]);
                    }
                    else if (a == typeId && b == typeOpr) {

                        b = translate(p->opr.op[1]);
                        a = translate(p->opr.op[0]);

                    }

                    else {
                        a = translate(p->opr.op[0]);
                        b = translate(p->opr.op[1]);
                    }

                    if (a == 0 && b == 0) {
                        fprintf(flrimal,"mul nx mx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    } else if (a == 0) {
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"mul mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    }
                    else if (b == 0) {
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"mul mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    } else {
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"mul mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");

                    }
                    gn = 0;
                    return 1;

                case '+':
                    a = p->opr.op[0]->type;
                    b = p->opr.op[1]->type;
                    if (a == typeCon && b == typeOpr) {
                        b = translate(p->opr.op[1]);
                        a = translate(p->opr.op[0]);
                    }
                    else if (a == typeId && b == typeOpr) {

                        b = translate(p->opr.op[1]);
                        a = translate(p->opr.op[0]);
                    }
                    else {
                        a = translate(p->opr.op[0]);
                        b = translate(p->opr.op[1]);
                    }

                    if (a == 0 && b == 0) {
                        fprintf(flrimal,"add nx mx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    } else if (a == 0) {
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"add mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    }
                    else if (b == 0) {
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"add mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    } else {
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"add mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    }
                    gn = 0;
                    return 1;


                case '-':
                    a = p->opr.op[0]->type;
                    b = p->opr.op[1]->type;

                    if (a == typeCon && b == typeOpr) {
                        b = translate(p->opr.op[1]);
                        a = translate(p->opr.op[0]);
                    }
                    else if (a == typeId && b == typeOpr) {

                        b = translate(p->opr.op[1]);
                        a = translate(p->opr.op[0]);

                    }

                    else {
                        a = translate(p->opr.op[0]);
                        b = translate(p->opr.op[1]);
                    }

                    if (a == 0 && b == 0) {
                        fprintf(flrimal,"sub nx mx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    } else if (a == 0) {
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"sub mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    }
                    else if (b == 0) {
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"sub mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    } else {
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"sub mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");

                    }
                    gn = 0;
                    return 1;

                case '/':
                    a = p->opr.op[0]->type;
                    b = p->opr.op[1]->type;

                    if (a == typeCon && b == typeOpr) {
                        b = translate(p->opr.op[1]);
                        a = translate(p->opr.op[0]);
                    }
                    else if (a == typeId && b == typeOpr) {

                        b = translate(p->opr.op[1]);
                        a = translate(p->opr.op[0]);

                    }

                    else {
                        a = translate(p->opr.op[0]);
                        b = translate(p->opr.op[1]);
                    }

                    if (a == 0 && b == 0) {
                        fprintf(flrimal,"div nx mx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    } else if (a == 0) {
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"div mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    }
                    else if (b == 0) {
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"div mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");
                    } else {
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"move mx nx\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"div mx nx\n");
                        fprintf(flrimal,"move fx nx\n");
                        fprintf(flrimal,"push\n");

                    }
                    gn = 0;
                    return 1;

                case ';': {
                    translate(p->opr.op[0]);
                    return translate(p->opr.op[1]);
                }


                case EQ:
                case NE:
                case '>':
                case GE:
                case '<':
                case LE: {
                    a = translate(p->opr.op[0]);
                    b = translate(p->opr.op[1]);
                    if (a == 0 && b == 0) {
                        fprintf(flrimal,"cmp regNX regMX\n");
                    }
                    else if (a == 0) {
                        fprintf(flrimal,"move regNX regMX\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"cmp regNX regMX\n");
                    }
                    else if (b == 0) {
                        fprintf(flrimal,"move regNX regMX\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"cmp regNX regMX\n");
                    }
                    else {
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"move regMX regNX\n");
                        fprintf(flrimal,"pop\n");
                        fprintf(flrimal,"cmp regNX regMX\n");
                    }
                    gn = 0;
                    return 1;
                }

                case IF: {
                    translate(p->opr.op[0]);        // this contains the condition
                    switch ((p->opr.op[0])->opr.oper) {
                        case EQ: {
                            fprintf(flrimal,"JNE here\n");
                            break;
                        }
                        case NE: {
                            fprintf(flrimal,"JIE here\n");
                            break;
                        }
                        case '>': {
                            fprintf(flrimal,"JL here\n");
                            break;
                        }
                        case GE: {
                            fprintf(flrimal,"JLE here\n");
                            break;
                        }
                        case '<': {
                            fprintf(flrimal,"JM here\n");
                            break;
                        }
                        case LE: {
                            fprintf(flrimal,"JME here\n");
                            break;
                        }
                    }
                    translate(p->opr.op[1]);        // this contains the if statements
                    fprintf(flrimal,"here:\n");
                    if (p->opr.nops > 2) {
                        translate(p->opr.op[2]);    // this contains the else statements
                    }
                    gn = 0;
                    return 1;
                }
                case POVTORUVAJ : {
                    //translate(p->opr.op[0]);

                    switch ((p->opr.op[0])->opr.oper) {
                        case EQ: {
                            fprintf(flrimal,"JNE endwhile\n");
                            break;
                        }
                        case NE: {
                            fprintf(flrimal,"JIE endwhile\n");
                            break;
                        }
                        case '>': {
                            fprintf(flrimal,"JL endwhile\n");
                            break;
                        }
                        case GE: {
                            fprintf(flrimal,"JLE endwhile\n");
                            break;
                        }
                        case '<': {
                            fprintf(flrimal,"JM endwhile\n");
                            break;
                        }
                        case LE: {
                            fprintf(flrimal,"JME endwhile\n");
                            break;
                        }
                    }
                    fprintf(flrimal,"while:\n");
                    //translate(p->opr.op[1]);
                    fprintf(flrimal,"JMP while\n");
                    fprintf(flrimal,"endwhile:\n");
                    return 0;
                }


                case POVTORUVAJ_2: // povtoruvaj x pati ! ... !
                {
                    fprintf(flrimal,"move regGX %d\n", (p->opr.op[0])->con.value);
                    fprintf(flrimal,"while:\n");
                    fprintf(flrimal,"cmp regGX 0\n");
                    fprintf(flrimal,"JIE end_while\n");
                    translate(p->opr.op[1]);
                    fprintf(flrimal,"move regPX regNX\n");
                    fprintf(flrimal,"decN\n");
                    fprintf(flrimal,"move regNX regPX\n");
                    fprintf(flrimal,"JMP while\n");
                    fprintf(flrimal,"end_while:\n");
                    gn = 0;
                    return 1;
                }

                case ODI:
                    fprintf(flrimal,"GO\n");
                    return 1;
                case ZEMI:
                    fprintf(flrimal,"TK\n");
                    return 1;
                case OSTAVI:
                    fprintf(flrimal,"LV\n");
                    return 1;
                case SVRTIDESNO:
                    fprintf(flrimal,"RR\n");
                    return 1;
                case SVRTILEVO:
                    fprintf(flrimal,"RL\n");
                    return 1;

            }
    }

    return 0;


}


//************ IMPLEMENTATION OF A STACK STRUCTURE ************//

int MAXSIZE = 8;
int stack[8];
int top = -1;

int isempty() {

    if(top == -1)
        return 1;
    else
        return 0;
}

int isfull() {

    if(top == MAXSIZE)
        return 1;
    else
        return 0;
}

int peek() {
    return stack[top];
}


int pop() {
    int data;

    if(!isempty()) {
        data = stack[top];
        top = top - 1;
        return data;
    }else {
        printf("Could not retrieve data, Stack is empty.\n");
    }
}

int push(int data) {

    if(!isfull()) {
        top = top + 1;
        stack[top] = data;
    }else {
        printf("Could not insert data, Stack is full.\n");
    }
}

void emptyStack(){
    top = -1;
}

//**************** END OF STACK IMPLEMENTATION ****************//
