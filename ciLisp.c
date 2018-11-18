/* Name: Tyler Yere
 * Project: ciLisp
 * COMP 262
 */

#include "ciLisp.h"
#include <math.h>

void yyerror(char *s) {
    printf("\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

//
// find out which function it is
//
int resolveFunc(char *func) {
    char *funcs[] = {"neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min",
                     "exp2", "cbrt", "hypot", ""};

    int i = 0;
    while (funcs[i][0] != '\0') {
        if (strcmp(funcs[i], func) == 0)
            return i;

        i++;
    }
    yyerror("invalid function");
    return INVALID_OPER;
}

//
// create a node for a number
//
AST_NODE *number(double value) {
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.value = value;

    return p;
}

//
//  create a node for a symbol that is used in the function trees (NOT the scope definitions)
//
AST_NODE *symbol(char *name) {
    AST_NODE *p;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE) + sizeof(SYMBOL_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = SYMBOL_TYPE;
    p->data.symbol.name = (char *) malloc (strlen(name)*sizeof(char));
    p->data.symbol.name = name;

    return p;
}

//
// create a node for a function
//
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2) {
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed size and the operands op1 and op2
    nodeSize = sizeof(AST_NODE) + sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;

    p->data.function.op1 = op1;
    p->data.function.op1->parent = p;   //creates link to traverse the scopes later in resolveSymbol function

    if (op2 == NULL)    //function can be called with only one operand
    {
        p->data.function.op2 = NULL;
    }
    else {
        p->data.function.op2 = op2;
        p->data.function.op2->parent = p;
    }

    return p;
}

//
// connects list of defined symbols (the scope) to the given s_expr node
// scopeList is the head of the scope and returns the s_expr node
//
AST_NODE *scope(SYMBOL_TABLE_NODE *scopeList, AST_NODE *s_expr)
{
    s_expr->scope = scopeList;
    return s_expr;
}

//
//  connects nodes of symbols defined in scope together
//  adds current given node to the head of the list
//  returns the head of the list
//
SYMBOL_TABLE_NODE *let_list(SYMBOL_TABLE_NODE *symbol, SYMBOL_TABLE_NODE *let_list)
{
    symbol->next = let_list;
    return symbol;
}

//
//  creates nodes of symbols defined in scope
//
SYMBOL_TABLE_NODE *let_elem(char *symbolName, AST_NODE *s_expr)
{
    SYMBOL_TABLE_NODE *p;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE) + sizeof(SYMBOL_TABLE_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->ident = symbolName;
    p->val = s_expr;
    p->next = NULL;

    return p;
}

//
//  searches for symbol values defined in scopes, starting from the current parent scope and going up to that parent's parent scope and so on
//
double resolveSymbol(char *symbolName, AST_NODE *p)
{
    return 0.0;
}

//
// free a node
//
void freeNode(AST_NODE *p) {
    if (!p)
        return;

    if (p->type == FUNC_TYPE) {
        free(p->data.function.name);
        freeNode(p->data.function.op1);
        freeNode(p->data.function.op2);
    }

    free(p);
}

//
// evaluate an abstract syntax tree
//
// p points to the root
//
double eval(AST_NODE *p) {
    if (!p)
        return 0.0;

// TBD: implement

    switch (p->type)
    {
        case NUM_TYPE:
            return p->data.number.value;
        case FUNC_TYPE:
            switch (resolveFunc(p->data.function.name))
            {
                case 0: //neg
                    return -1 * eval(p->data.function.op1);
                case 1: //abs
                    return fabs(eval(p->data.function.op1));
                case 2: //exp
                    return exp(eval(p->data.function.op1));
                case 3: //sqrt                                                  //all single operand functions assume op1 contains relevant operand
                    return sqrt(eval(p->data.function.op1));
                case 4: //add
                    return eval(p->data.function.op1) + eval(p->data.function.op2);
                case 5: //sub
                    return eval(p->data.function.op1) - eval(p->data.function.op2);
                case 6: //mult
                    return eval(p->data.function.op1) * eval(p->data.function.op2);
                case 7: //div
                    return eval(p->data.function.op1) / eval(p->data.function.op2);
                case 8: //remainder
                    return remainder(eval(p->data.function.op1), eval(p->data.function.op2));
                case 9: //log
                    return log(eval(p->data.function.op1));
                case 10:    //pow
                    return pow(eval(p->data.function.op1), eval(p->data.function.op2));
                case 11:    //max
                    return fmax(eval(p->data.function.op1), eval(p->data.function.op2));
                case 12:    //min
                    return fmin(eval(p->data.function.op1), eval(p->data.function.op2));
                case 13:    //exp2
                    return exp2(eval(p->data.function.op1));
                case 14:    //cbrt
                    return cbrt(eval(p->data.function.op1));
                case 15:    //hypot
                    return hypot(eval(p->data.function.op1), eval(p->data.function.op2));
                default:
                    break;
            }
        default:
            break;
    }

}  

