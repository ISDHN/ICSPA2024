%option noyywrap
%option noinput
%option nounput
%{
#include "expr.tab.h"
#include <stdlib.h>
#include <string.h>
%}

d [0-9]
h [a-fA-F0-9]
w [a-zA-Z0-9]

%%

[ \t\r\n]+      { /* ignore */ }
(${w}+|$$0)     {yylval.reg = strdup(yytext+1); return T_REG;}
{d}+            {yylval.num = atoi(yytext); return T_NUM;}
0x{h}+          {yylval.num = strtol(yytext, NULL, 16); return T_NUM;}
&&              { return T_LAND; }
==              { return T_EQ; }
!=              { return T_NE; }
.               { return yytext[0]; }       


%%

void set_input(const char* str) {
    yy_scan_string(str);
}