%code requires{
    #include <memory/vaddr.h>
    #include <isa.h>

    int yylex(void);
    void yyerror(uint64_t* result, const char *s);
}

%union {
    uint64_t num;
    char* reg;
}

%parse-param {uint64_t* result}

%token <reg> T_REG
%token <num> T_NUM

%type <num> expr
%type <num> number

%left T_LAND
%left T_EQ T_NE
%left '+' '-'
%left '*' '/'
%right NEGM  DREF

%start res

%%

res : expr { *result = $1; }
    ;

number : T_NUM { 
            $$ = $1; 
        }
        | T_REG { 
            bool success = false;
            $$ = isa_reg_str2val($1, &success);
            if (!success) {
                yyerror(result, "Invalid register name");
                YYABORT;
            }
        }
       ;

expr : number { $$ = $1; }
    | '*' expr %prec DREF { $$ = vaddr_read($2, 4); }
    | expr '+' expr { $$ = $1 + $3; }
    | expr '-' expr { $$ = $1 - $3; }
    | expr '*' expr {  $$ = $1 * $3; }
    | expr '/' expr { 
            if ($3 == 0) {
                yyerror(result, "Division by zero");
                YYABORT;
            }
            $$ = $1 / $3; 
        }
    | '-' expr %prec NEGM { $$ = - $2; }
    | '(' expr ')' { $$ = $2; }
    | expr T_EQ expr { $$ = $1 == $3; }
    | expr T_NE expr { $$ = $1 != $3; }
    | expr T_LAND expr { $$ = $1 && $3; }
     ;

%%

void yyerror(uint64_t* result, const char *s) {
    printf("Error: %s\n", s);
}