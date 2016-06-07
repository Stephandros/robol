%{
#include <stdlib.h>
#include "robol.h"
#include "y.tab.h"
void yyerror(char *);
%}

%%

"while"         return WHILE;
"ako"            return IF;
"inaku"          return ELSE;
"print"         return PRINT;
"povtoruvaj"	return POVTORUVAJ;
"do"			return DO;
"pati"			return PATI;
"procedura"		return PROCEDURA;
"pocetok"		return POCETOK;
"kraj"			return KRAJ;
"povikaj"		return CALL;

"broj"			return T_BROJ;
"nasoka"		return T_NASOKA;
I			{yylval.iValue =0; return NASOKA;}
Z			{yylval.iValue =1; return NASOKA;}
S			{yylval.iValue =2; return NASOKA;}
J			{yylval.iValue =3; return NASOKA;}

"SvrtiLevo"|"SvrtiDesno"|"Odi"|"Zemi"|"Ostavi"	{ return OSNOVNA_KOMANDA; }	


[a-zA-z][a-zA-Z0-9]*       { 
				yylval.sIndex = lookup(yytext);
                //yylval.sIndex = *yytext - 'a';
                return VARIABLE;
            }

0           {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[1-9][0-9]* {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[-()<>=+*/;:{}.] {
                return *yytext;
             }
			 
[!]	{
	return *yytext;
}

[\n]+			return NEWLINE;	

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;


[ \t]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}