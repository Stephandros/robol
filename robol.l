%{
#include <stdlib.h>
#include "robol.h"
#include "y.tab.h"
void yyerror(char *);
extern *flerror;
%}
%option yylineno
%%

"-" |
"+" |
"." |
"(" |
")" |
"<" |
">" |
"=" |
"*" |
"/" |
"'" |
":" |
"{" |
"}" |
"," |
"!" {
       /*printf(yytext);*/
       return *yytext;
    }

"okolina"  return OKOLINA;
"dzidovi"   return DZIDOVI;
"zetoni"    return ZETONI;
"robot"     return ROBOT;
"end"           return END;
"while"         return WHILE;
"ako"          return IF;
"inaku"         return ELSE;
"print"         return PRINT;
"povtoruvaj"	return POVTORUVAJ;
"do"			return DO;
"pati"			return PATI;
"procedura"		return PROCEDURA;
"pocetok"		return POCETOK;
"kraj"			return KRAJ;
"povikaj"		return POVIKAJ;
"broj"			{yylval.iValue =0; return T_BROJ;} //0 is broj
"nasoka"		{yylval.iValue =1; return T_NASOKA;} //1 is nasoka

"I"			{yylval.iValue =0; return NASOKA;}
"Z"			{yylval.iValue =1; return NASOKA;}
"S"			{yylval.iValue =2; return NASOKA;}
"J"			{yylval.iValue =3; return NASOKA;}

"IZ"    {yylval.iValue =0; return PRAVEC;}
"SJ"   { yylval.iValue =1; return PRAVEC;}

"SvrtiLevo"  return SVRTILEVO;
"SvrtiDesno" return SVRTIDESNO;
"Odi"        return ODI;
"Zemi"       return ZEMI;
"Ostavi"     return OSTAVI;

[a-zA-z][a-zA-Z0-9]*   {

			           yylval.iValue = lookup(yytext);
                       //yylval.sIndex = *yytext - 'a';
                       /*printf(yytext);
                       printf("\n");*/
                       return VARIABLE;
                       }

0   {  yylval.iValue = atoi(yytext);
       return INTEGER;
    }

[1-9][0-9]* {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }



[\n]+			{return NEWLINE;}

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;


[ \t]+        ;       /* ignore whitespace */

.              yyerror("Unknown character");

%%

int yywrap(void) {
    return 1;
}
