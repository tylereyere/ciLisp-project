/* Name: Tyler Yere
 * Project: ciLisp
 * COMP 262
 */

%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node* symbolTableNode;
};

%token <sval> FUNC
%token <sval> SYMBOL
%token <dval> NUMBER
%token LPAREN RPAREN EOL QUIT LET

%type <astNode> s_expr f_expr
%type <symbolTableNode> scope let_elem let_list

%%

program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
            printf("%lf", eval($1));
            freeNode($1);
        }
    };

s_expr:
    NUMBER {
        fprintf(stderr, "yacc: s_expr ::= NUMBER\n");
        $$ = number($1);
    }
    |
    SYMBOL {
        fprintf(stderr, "yacc: s_expr ::= SYMBOL\n");
        $$ = symbol($1);
    }
    | f_expr {
        $$ = $1;
    }
    | LPAREN scope s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= (scope s_expr)\n");
        $$ = scope($2, $3);
    }
    | QUIT {
        fprintf(stderr, "yacc: s_expr ::= QUIT\n");
        exit(EXIT_SUCCESS);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    };

f_expr:
    LPAREN FUNC s_expr RPAREN {
        fprintf(stderr, "yacc: f_expr ::= LPAREN FUNC s_expr RPAREN\n");
        $$ = function($2, $3, 0);
    }
    | LPAREN FUNC s_expr s_expr RPAREN {
        fprintf(stderr, "yacc: f_expr ::= LPAREN FUNC s_expr s_expr RPAREN\n");
        $$ = function($2, $3, $4);
    };

scope:
    {
        fprintf(stderr, "yacc: scope ::= <empty>\n");
        $$ = NULL;
    }
    | LPAREN LET let_list RPAREN {
        fprintf(stderr, "yacc: scope ::= LPAREN LET let_list RPAREN\n");
        $$ = $3;
    }

let_list:
    let_elem {
        fprintf(stderr, "yacc: let_list ::= let_elem\n");
        $$ = let_list($1, 0);
    }
    | let_list let_elem {
        fprintf(stderr, "yacc: let_list ::= let_list let_elem\n");
        $$ = let_list($1, $2);
    };

let_elem:
    LPAREN SYMBOL s_expr RPAREN {
    fprintf(stderr, "yacc: list_elem ::= LPAREN SYMBOL s_expr RPAREN\n");
    $$ = let_elem($2, $3);
    };

%%

