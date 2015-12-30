/*----------------------------------------------------------------------------
 Note: the code in this file is not to be shared with anyone or posted online.
 (c) Rida Bazzi, 2015
 ----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "syntax.h"

#define TRUE 1
#define FALSE 0

#define WORD_SIZE 100
#define MAX_SIZE 100


/************************** Global Variables **************************/


int type = FALSE;
int variable = FALSE;
int implicit  = FALSE;
int repeat = 0;
int leftNumber = -1;
int rightNumber = -1;
int switchNumber = -1;
int leftExpr[MAX_SIZE];

//char** tempTypeArray;
//char** tempVarArray;
//char** tempCheckArray;
//char** implicitVarArray;
int i = 0;
int j = 0;
int k = 0;
int l = 0;
int m = 0;
int n = 0;
int o = 0;
int p = 0;
int count = 0;
int dupType = 0;
int count1 = 15;


/************ Structures to save types and variables to check for ERROR CODE 0,1,2,4 ***********/
struct tempType{
    char** tempTypeArray;
    int sequence;
};
struct tempType tempType;

struct tempVar{
    char** tempVarArray;
    int sequence;
};
struct tempVar tempVar;

struct tempCheck{
    char** tempCheckArray;
    int sequence;
};
struct tempCheck tempCheck;

struct implicitVar{
    char** implicitVarArray;
    int sequence;
};
struct implicitVar implicitVar;

/******** Structures to save types and vars and their corresponding numbers and implicit/explicit type for type checking and printing outpur in specific manner as per specificaton *****/

struct Type{
    char** TypeArray;                   //saves the type
    int typeNumber[MAX_SIZE];           //saves the number correspondint to the type above
    int ID[MAX_SIZE];                   //1 if it is ID, 0 if it is not
    int sort[MAX_SIZE];                 //1 for exp type, 2 for implicit, 3 for exp var, 4 for impl var
};
struct Type Type;

struct Var{                             // same as above
    char** VarArray;
    int typeNumber[MAX_SIZE];
    int ID[MAX_SIZE];
    int sort[MAX_SIZE];
};
struct Var Var;


/********  Functions used for type checking and printing output *********/

void initialiseMemory();
void initialiseLeftExpr();
void addToTempTypeList(char* );
void addToTempVarList(char* );
void checkDuplicateTypes();
void checkDuplicateVars();
void checkBothArrays();
int checkBeforeAdd(char* );
void addToTempCheckArray(char* );
int checkImplicitType(char* );
void addToImplicitVarList(char* );
int checkTypeDupBody(char* );
void addToTypeList(char* ,int ,int );
void checkTokenType(int );
void addToVarList(char* , int ,int );
void checkVarType(int );
void findLeftNumber(char* );
void checkFactorNumber(char* );
void checkForError();
void assign_stmtCheck();
void printOutput();


/* ------------------------------------------------------- */
/* -------------------- LEXER SECTION -------------------- */
/* ------------------------------------------------------- */

#define KEYWORDS  11

enum TokenTypes
{
    VAR = 1,
    WHILE,
    INT,
    REAL,
    STRING,
    BOOLEAN,
    TYPE,
    LONG,
    DO,
    CASE,
    SWITCH,
    PLUS,
    MINUS,
    DIV,
    MULT,
    EQUAL,
    COLON,
    COMMA,
    SEMICOLON,
    LBRAC,
    RBRAC,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    NOTEQUAL,
    GREATER,
    LESS,
    LTEQ,
    GTEQ,
    DOT,
    ID,
    NUM,
    REALNUM,
    ERROR
};

char *reserved[] = {
    "",
    "VAR",
    "WHILE",
    "INT",
    "REAL",
    "STRING",
    "BOOLEAN",
    "TYPE",
    "LONG",
    "DO",
    "CASE",
    "SWITCH",
    "+",
    "-",
    "/",
    "*",
    "=",
    ":",
    ",",
    ";",
    "[",
    "]",
    "(",
    ")",
    "{",
    "}",
    "<>",
    ">",
    "<",
    "<=",
    ">=",
    ".",
    "ID",
    "NUM",
    "REALNUM",
    "ERROR"
};

// Global Variables associated with the next input token
#define MAX_TOKEN_LENGTH 100
char token[MAX_TOKEN_LENGTH]; // token string
int ttype; // token type
int activeToken = FALSE;
int tokenLength;
int line_no = 1;

void skipSpace()
{
    char c;
    
    c = getchar();
    line_no += (c == '\n');
    while (!feof(stdin) && isspace(c))
    {
        c = getchar();
        line_no += (c == '\n');
    }
    ungetc(c, stdin);
}

int isKeyword(char *s)
{
    int i;
    
    for (i = 1; i <= KEYWORDS; i++)
        if (strcmp(reserved[i], s) == 0)
            return i;
    return FALSE;
}

/*
 * ungetToken() simply sets a flag so that when getToken() is called
 * the old ttype is returned and the old token is not overwritten.
 * NOTE: BETWEEN ANY TWO SEPARATE CALLS TO ungetToken() THERE MUST BE
 * AT LEAST ONE CALL TO getToken(). CALLING TWO ungetToken() WILL NOT
 * UNGET TWO TOKENS
 */
void ungetToken()
{
    activeToken = TRUE;
}

int scan_number()
{
    char c;
    
    c = getchar();
    if (isdigit(c))
    {
        // First collect leading digits before dot
        // 0 is a nNUM by itself
        if (c == '0')
        {
            token[tokenLength] = c;
            tokenLength++;
            token[tokenLength] = '\0';
        }
        else
        {
            while (isdigit(c))
            {
                token[tokenLength] = c;
                tokenLength++;
                c = getchar();
            }
            ungetc(c, stdin);
            token[tokenLength] = '\0';
        }
        // Check if leading digits are integer part of a REALNUM
        c = getchar();
        if (c == '.')
        {
            c = getchar();
            if (isdigit(c))
            {
                token[tokenLength] = '.';
                tokenLength++;
                while (isdigit(c))
                {
                    token[tokenLength] = c;
                    tokenLength++;
                    c = getchar();
                }
                token[tokenLength] = '\0';
                if (!feof(stdin))
                    ungetc(c, stdin);
                return REALNUM;
            }
            else
            {
                ungetc(c, stdin);
                c = '.';
                ungetc(c, stdin);
                return NUM;
            }
        }
        else
        {
            ungetc(c, stdin);
            return NUM;
        }
    }
    else
        return ERROR;
}

int scan_id_or_keyword()
{
    int ttype;
    char c;
    
    c = getchar();
    if (isalpha(c))
    {
        while (isalnum(c))
        {
            token[tokenLength] = c;
            tokenLength++;
            c = getchar();
        }
        if (!feof(stdin))
            ungetc(c, stdin);
        token[tokenLength] = '\0';
        ttype = isKeyword(token);
        if (ttype == 0)
            ttype = ID;
        return ttype;
    }
    else
        return ERROR;
}

int getToken()
{
    char c;
    
    if (activeToken)
    {
        activeToken = FALSE;
        return ttype;
    }
    skipSpace();
    tokenLength = 0;
    c = getchar();
    switch (c)
    {
        case '.':
            return DOT;
        case '+':
            return PLUS;
        case '-':
            return MINUS;
        case '/':
            return DIV;
        case '*':
            return MULT;
        case '=':
            return EQUAL;
        case ':':
            return COLON;
        case ',':
            return COMMA;
        case ';':
            return SEMICOLON;
        case '[':
            return LBRAC;
        case ']':
            return RBRAC;
        case '(':
            return LPAREN;
        case ')':
            return RPAREN;
        case '{':
            return LBRACE;
        case '}':
            return RBRACE;
        case '<':
            c = getchar();
            if (c == '=')
                return LTEQ;
            else if (c == '>')
                return NOTEQUAL;
            else
            {
                ungetc(c, stdin);
                return LESS;
            }
        case '>':
            c = getchar();
            if (c == '=')
                return GTEQ;
            else
            {
                ungetc(c, stdin);
                return GREATER;
            }
        default:
            if (isdigit(c))
            {
                ungetc(c, stdin);
                return scan_number();
            }
            else if (isalpha(c))
            {
                ungetc(c, stdin);
                return scan_id_or_keyword();
            }
            else if (c == EOF)
                return EOF;
            else
                return ERROR;
    }
}

/* ----------------------------------------------------------------- */
/* -------------------- SYNTAX ANALYSIS SECTION -------------------- */
/* ----------------------------------------------------------------- */

void syntax_error(const char* msg)
{
    printf("Syntax error while parsing %s line %d\n", msg, line_no);
    exit(1);
}


/* -------------------- PARSING AND BUILDING PARSE TREE -------------------- */

// Note that the following function is not
// called case because case is a keyword in C/C++
struct caseNode* cas()
{
    struct caseNode* cas;
    
    cas = ALLOC(struct caseNode);
    
    ttype = getToken();
    if (ttype == CASE) {
        
        ttype = getToken();
        if (ttype == NUM || ttype == INT) {
            
            cas = ALLOC(struct caseNode);
            cas -> num = atoi(token);
            
            ttype = getToken();
            if (ttype == COLON) {
                
                cas -> body = body();
                
                return cas;
            }else{
                syntax_error("case. COLON expected.");
                return NULL; // control never reaches here, this is just for the sake of GCC
            }
        }else if(ttype == REALNUM || ttype == REAL || ttype == LONG || ttype == STRING || ttype == BOOLEAN){
            printf("ERROR CODE 3\n");
            exit(1);
        }else{
            syntax_error("case. NUM expected.");
            return NULL; // control never reaches here, this is just for the sake of GCC
        }
    }else{
        syntax_error("case. CASE expected.");
        return NULL; // control never reaches here, this is just for the sake of GCC
    }
}

struct case_listNode* case_list()
{
    struct case_listNode* caseList;
    
    ttype = getToken();
    if (ttype == CASE) {
        ungetToken();
        caseList = ALLOC(struct case_listNode);
        caseList -> cas = cas();
        
        ttype = getToken();
        if (ttype == CASE) {
            ungetToken();
            caseList->case_list = case_list();
            return caseList;
        }
        else // If the next token is not in FOLLOW(case_list),
            // let the caller handle it.
        {
            ungetToken();
            caseList->case_list = NULL;
            return caseList;
        }
    }else{
        syntax_error("case_list. ID expected.");
        return NULL; // control never reaches here, this is just for the sake of GCC
    }
}

struct switch_stmtNode* switch_stmt()
{
    struct switch_stmtNode* swtch_stmt;
    
    swtch_stmt = ALLOC(struct switch_stmtNode);
    
    ttype = getToken();
    if (ttype == SWITCH) {
        
        ttype = getToken();
        if (ttype == ID || ttype == NUM || ttype == INT) {
            
            dupType = checkTypeDupBody(token);
            if (dupType) {
                printf("ERROR CODE 1");
                exit(1);
            }
            
            implicit = checkBeforeAdd(token);
            if (!implicit) {
                addToTempVarList(token);
                checkDuplicateVars();
                checkDuplicateTypes();
                checkBothArrays();
                addToImplicitVarList(token);
                addToVarList(token, ttype, 4);
                
                int flag = 0;
                int x = 0;
                for (x=0; x<n;x++) {
                    if (!strcmp(Type.TypeArray[x], token)) {
                        int y = 0;
                        for (y = 0; y<o; y++) {
                            if (Var.typeNumber[y] == 0) {
                                Var.typeNumber[y] = Type.typeNumber[x];
                            }
                        }
                        flag = 1;
                    }
                }
                if (!flag) {
                    Var.typeNumber[o-1] = count1++;
                    flag = 0;
                }
                
            }else{
                implicit = 0;
            }
            
            if (ttype == ID) {
                int x = 0;
                for (x = 0; x<o; x++) {
                    if (!strcmp(token, Var.VarArray[x])) {
                        switchNumber = Var.typeNumber[x];
                        break;
                    }
                }
                if (switchNumber != -1) {
                    for (x = 0; x<o; x++) {
                        if (Var.typeNumber[x] == switchNumber) {
                            Var.typeNumber[x] = 10;
                        }
                    }
                    switchNumber = -1;
                }
            }
            
            //            swtch_stmt -> id = ALLOC(char);
            //            swtch_stmt -> id = strdup(token);
            
            swtch_stmt->id = (char *) malloc((tokenLength+1)*sizeof(char));
            strcpy(swtch_stmt->id,token);
            
            
            
            ttype = getToken();
            if (ttype == LBRACE) {
                swtch_stmt -> case_list = case_list();
                
                ttype = getToken();
                if (ttype == RBRACE) {
                    return  swtch_stmt;
                }else{
                    syntax_error("switch_stmt. RBRACE expected.");
                    return NULL; // control never reaches here, this is just for the sake of GCC
                }
            }else{
                syntax_error("switch_stmt. LBRACE expected.");
                return NULL; // control never reaches here, this is just for the sake of GCC
            }
        }
        else{
            if (ttype == REALNUM || ttype == REAL || ttype == STRING || ttype == BOOLEAN || ttype == LONG) {
                printf("ERROR CODE 3\n");
                exit(1);
            }else{
                syntax_error("switch_stmt. ID, NUM or INT expected.");
                return NULL; // control never reaches here, this is just for the sake of GCC
            }
        }
    }else{
        syntax_error("switch_stmt. SWITCH expected.");
        return NULL; // control never reaches here, this is just for the sake of GCC
    }
}

struct while_stmtNode* do_stmt()
{
    struct while_stmtNode* while_stmt;
    
    ttype = getToken();
    if (ttype == DO)
    {
        while_stmt = ALLOC(struct while_stmtNode);
        
        while_stmt->body = body();
        ttype = getToken();
        if (ttype == WHILE) {
            while_stmt->condition = condition();
            
            ttype = getToken();
            if (ttype == SEMICOLON) {
                return while_stmt;
            }
            else{
                syntax_error("do_while_stmt.SEMICOLON expected.");
                return NULL; // control never reaches here, this is just for the sake of GCC
            }
        }else{
            syntax_error("do_while_stmt.WHILE expected.");
            return NULL; // control never reaches here, this is just for the sake of GCC}
        }
    }else{
        syntax_error("do_while_stmt. DO expected");
        return NULL; // control never reaches here, this is just for the sake of GCC
    }
}

struct primaryNode* primary()
{
    struct primaryNode* primary;
	ttype = getToken();
    
    checkFactorNumber(token);
    
	if (ttype == ID){
		primary = ALLOC(struct primaryNode);
        primary -> id = strdup(token);
		primary->tag = ID;
        
        dupType = checkTypeDupBody(token);
        if (dupType) {
            printf("ERROR CODE 1");
            exit(1);
        }
        
        implicit = checkBeforeAdd(token);
        if (!implicit) {
            addToTempVarList(token);
            checkDuplicateVars();
            checkDuplicateTypes();
            checkBothArrays();
            addToImplicitVarList(token);
            addToVarList(token, ttype, 4);
            
            int flag = 0;
            int x = 0;
            for (x=0; x<n;x++) {
                if (!strcmp(Type.TypeArray[x], token)) {
                    int y = 0;
                    for (y = 0; y<o; y++) {
                        if (Var.typeNumber[y] == 0) {
                            Var.typeNumber[y] = Type.typeNumber[x];
                        }
                    }
                    flag = 1;
                }
            }
            if (!flag) {
                Var.typeNumber[o-1] = count1++;
                flag = 0;
            }
        }else{
            implicit = 0;
        }
	}
	else if (ttype == NUM){
        primary = ALLOC(struct primaryNode);
		primary->ival = atoi(token);
		primary->tag = NUM;
	}
	else if (ttype == REALNUM){
		primary = ALLOC(struct primaryNode);
		primary->fval = atof(token);
		primary->tag = REALNUM;
	}
	else{
        ungetToken();
		syntax_error("primary. NUM, REALNUM, or ID expected");
        return NULL; // control never reaches here, this is just for the sake of GCC
	}
    return primary;
}

struct conditionNode* condition()
{
    struct conditionNode* condition;
    
    ttype = getToken();
    
    if (ttype == ID || ttype == NUM || ttype == REALNUM){
        condition = ALLOC(struct conditionNode);
        ungetToken();
        
        condition -> left_operand = primary();
        ttype = getToken();
        
        if (ttype == NOTEQUAL || ttype == GREATER || ttype == LESS || ttype == LTEQ || ttype == GTEQ) {
            condition -> relop = ttype;
            ttype = getToken();
            if (ttype == ID || ttype == NUM || ttype == REALNUM){
                if (ttype == ID) {
                    
                    dupType = checkTypeDupBody(token);
                    if (dupType) {
                        printf("ERROR CODE 1");
                        exit(1);
                    }
                    
                    implicit = checkBeforeAdd(token);
                    if (!implicit) {
                        addToTempVarList(token);
                        checkDuplicateTypes();
                        checkBothArrays();
                        checkDuplicateVars();
                        addToImplicitVarList(token);
                        addToVarList(token, ttype, 4);
                        
                        int flag = 0;
                        int x = 0;
                        for (x=0; x<n;x++) {
                            if (!strcmp(Type.TypeArray[x], token)) {
                                int y = 0;
                                for (y = 0; y<o; y++) {
                                    if (Var.typeNumber[y] == 0) {
                                        Var.typeNumber[y] = Type.typeNumber[x];
                                    }
                                }
                                flag = 1;
                            }
                        }
                        if (!flag) {
                            Var.typeNumber[o-1] = count1++;
                            flag = 0;
                        }
                    }else{
                        implicit = 0;
                    }
                }
                ungetToken();
                condition -> right_operand = primary();
                checkForError();
                initialiseLeftExpr();
            }
            else{
				syntax_error("condition. right_operand expected");
                return NULL; // control never reaches here, this is just for the sake of GCC
			}
        }else{
            if (leftExpr[0] == 10 || leftExpr[0] == 11 || leftExpr[0] == 12 || leftExpr[0] == 14) {
                printf("ERROR CODE 3\n");
                exit(1);
            }else if (leftExpr[0]>14){
                int w = 0;
                for (w = 0; w<o; w++) {
                    if (Var.typeNumber[w] == leftExpr[0]) {
                        Var.typeNumber[w] = 13;
                    }
                }
                for (w = 0; w<n; w++) {
                    if (Type.typeNumber[w] == leftExpr[0]) {
                        Type.typeNumber[w] = 13;
                    }
                }
                initialiseLeftExpr();
            }
            
            ungetToken();
        }
        return condition;
    }
    else if (ttype == ID) {
        condition = ALLOC(struct conditionNode);
        condition -> relop = 0;
        condition -> right_operand = NULL;
        condition -> left_operand -> tag = ID;
        
        dupType = checkTypeDupBody(token);
        if (dupType) {
            printf("ERROR CODE 1");
            exit(1);
        }
        
        implicit = checkBeforeAdd(token);
        if (!implicit) {
            addToTempVarList(token);
            checkDuplicateVars();
            checkDuplicateTypes();
            checkBothArrays();
            addToImplicitVarList(token);
            addToVarList(token, ttype, 4);
            
            int flag = 0;
            int x = 0;
            for (x=0; x<n;x++) {
                if (!strcmp(Type.TypeArray[x], token)) {
                    int y = 0;
                    for (y = 0; y<o; y++) {
                        if (Var.typeNumber[y] == 0) {
                            Var.typeNumber[y] = Type.typeNumber[x];
                        }
                    }
                    flag = 1;
                }
            }
            if (!flag) {
                Var.typeNumber[o-1] = count1++;
                flag = 0;
            }
        }else{
            implicit = 0;
        }
        return condition;
    }else{
		syntax_error("condition. ID or primary expected");
		return NULL; // control never reaches here, this is just for the sake of GCC
	}
}

struct while_stmtNode* while_stmt()
{
    struct while_stmtNode* while_stmt;
    
    ttype = getToken();
    if (ttype == WHILE)
    {
        while_stmt = ALLOC(struct while_stmtNode);
        
        while_stmt->condition = condition();
        while_stmt->body = body();
        return while_stmt;
    }
    else{
        syntax_error("while_stmt. WHILE expected");
        return NULL; // control never reaches here, this is just for the sake of GCC
    }
}

struct exprNode* factor()
{
    struct exprNode* facto;
    
    ttype = getToken();
    if (ttype == LPAREN)
    {
        facto = expr();
        ttype = getToken();
        if (ttype == RPAREN)
            return facto;
        else
            syntax_error("factor. RPAREN expected");
    }
    else if (ttype == NUM)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = NUM;
        facto->primary->ival = atoi(token);
        
        leftExpr[p++] = 10;
        checkForError();
        
        return facto;
    }
    else if (ttype == REALNUM)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = REALNUM;
        facto->primary->fval = atof(token);
        
        leftExpr[p++] = 11;
        checkForError();
        
        return facto;
    }
    else if (ttype == ID)
    {
        dupType = checkTypeDupBody(token);
        if (dupType) {
            printf("ERROR CODE 1");
            exit(1);
        }
        
        implicit = checkBeforeAdd(token);
        if (!implicit) {
            addToTempVarList(token);
            checkDuplicateVars();
            checkDuplicateTypes();
            checkBothArrays();
            addToImplicitVarList(token);
            addToVarList(token, ttype, 4);
            
            int flag = 0;
            int x = 0;
            for (x=0; x<n;x++) {
                if (!strcmp(Type.TypeArray[x], token)) {
                    int y = 0;
                    for (y = 0; y<o; y++) {
                        if (Var.typeNumber[y] == 0) {
                            Var.typeNumber[y] = Type.typeNumber[x];
                        }
                    }
                    flag = 1;
                }
            }
            if (!flag) {
                Var.typeNumber[o-1] = count1++;
                flag = 0;
            }
        }else{
            implicit = 0;
        }
        
        
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = ID;
        facto->primary->id = strdup(token);
        
        checkFactorNumber(token);
        checkForError();
        
        return facto;
    }
    else
        syntax_error("factor. NUM, REALNUM, or ID, expected");
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* term()
{
    struct exprNode* ter;
    struct exprNode* f;
    
    ttype = getToken();
    if (ttype == ID || ttype == LPAREN || ttype == NUM || ttype == REALNUM)
    {
        ungetToken();
        f = factor();
        ttype = getToken();
        if (ttype == MULT || ttype == DIV)
        {
            ter = ALLOC(struct exprNode);
            ter->op = ttype;
            ter->leftOperand = f;
            ter->rightOperand = term();
            ter->tag = EXPR;
            ter->primary = NULL;
            return ter;
        }
        else if (ttype == SEMICOLON || ttype == PLUS ||
                 ttype == MINUS || ttype == RPAREN)
        {
            ungetToken();
            return f;
        }
        else
            syntax_error("term. MULT or DIV expected");
    }
    else
        syntax_error("term. ID, LPAREN, NUM, or REALNUM expected");
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* expr()
{
    struct exprNode* exp;
    struct exprNode* t;
    
    ttype = getToken();
    if (ttype == ID || ttype == LPAREN || ttype == NUM || ttype == REALNUM)
    {
        ungetToken();
        t = term();
        ttype = getToken();
        if (ttype == PLUS || ttype == MINUS)
        {
            exp = ALLOC(struct exprNode);
            exp->op = ttype;
            exp->leftOperand = t;
            exp->rightOperand = expr();
            exp->tag = EXPR;
            exp->primary = NULL;
            return exp;
        }
        else if (ttype == SEMICOLON || ttype == MULT ||
                 ttype == DIV || ttype == RPAREN)
        {
            ungetToken();
            return t;
        }
        else
            syntax_error("expr. PLUS, MINUS, or SEMICOLON expected");
    }
    else
        syntax_error("expr. ID, LPAREN, NUM, or REALNUM expected");
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct assign_stmtNode* assign_stmt()
{
    struct assign_stmtNode* assignStmt;
    
    ttype = getToken();
    if (ttype == ID)
    {
        dupType = checkTypeDupBody(token);
        if (dupType) {
            printf("ERROR CODE 1");
            exit(1);
        }
        
        implicit = checkBeforeAdd(token);
        if (!implicit) {
            addToTempVarList(token);
            checkDuplicateVars();
            checkDuplicateTypes();
            checkBothArrays();
            addToImplicitVarList(token);
            addToVarList(token, ttype, 4);
            
            int flag = 0;
            int x = 0;
            for (x=0; x<n;x++) {
                if (!strcmp(Type.TypeArray[x], token)) {
                    int y = 0;
                    for (y = 0; y<o; y++) {
                        if (Var.typeNumber[y] == 0) {
                            Var.typeNumber[y] = Type.typeNumber[x];
                        }
                    }
                    flag = 1;
                }
            }
            if (!flag) {
                Var.typeNumber[o-1] = count1++;
                flag = 0;
            }
        }else{
            implicit = 0;
        }
        
        
        assignStmt = ALLOC(struct assign_stmtNode);
        assignStmt->id = strdup(token);
        
        findLeftNumber(assignStmt->id);
        
        ttype = getToken();
        if (ttype == EQUAL)
        {
            assignStmt->expr = expr();
            ttype = getToken();
            if (ttype == SEMICOLON){
                rightNumber = leftExpr[p-1];
                
                if (leftNumber != rightNumber) {
                    assign_stmtCheck();
                }
                
                initialiseLeftExpr();
                leftNumber = -1;
                rightNumber = -1;
                return assignStmt;
            }
            else
                syntax_error("asign_stmt. SEMICOLON expected");
        }
        else
            syntax_error("assign_stmt. EQUAL expected");
    }
    else
        syntax_error("assign_stmt. ID expected");
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct stmtNode* stmt()
{
    struct stmtNode* stm;
    
    stm = ALLOC(struct stmtNode);
    ttype = getToken();
    if (ttype == ID) // assign_stmt
    {
        ungetToken();
        stm->assign_stmt = assign_stmt();
        stm->stmtType = ASSIGN;
    }
    else if (ttype == WHILE) // while_stmt
    {
        ungetToken();
        stm->while_stmt = while_stmt();
        stm->stmtType = WHILE;
    }
    else if (ttype == DO)  // do_stmt
    {
        ungetToken();
        stm->while_stmt = do_stmt();
        stm->stmtType = DO;
    }
    else if (ttype == SWITCH) // switch_stmt
    {
        ungetToken();
        stm->switch_stmt = switch_stmt();
        stm->stmtType = SWITCH;
    }
    else
        syntax_error("stmt. ID, WHILE, DO or SWITCH expected");
    return stm;
}

struct stmt_listNode* stmt_list()
{
    struct stmt_listNode* stmtList;
    
    ttype = getToken();
    if (ttype == ID || ttype == WHILE ||
        ttype == DO || ttype == SWITCH)
    {
        ungetToken();
        stmtList = ALLOC(struct stmt_listNode);
        stmtList->stmt = stmt();
        ttype = getToken();
        if (ttype == ID || ttype == WHILE ||
            ttype == DO || ttype == SWITCH)
        {
            ungetToken();
            stmtList->stmt_list = stmt_list();
            return stmtList;
        }
        else // If the next token is not in FOLLOW(stmt_list),
            // let the caller handle it.
        {
            ungetToken();
            stmtList->stmt_list = NULL;
            return stmtList;
        }
    }
    else
        syntax_error("stmt_list. ID, WHILE, DO or SWITCH expected");
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct bodyNode* body()
{
    struct bodyNode* bod;
    
    ttype = getToken();
    if (ttype == LBRACE)
    {
        bod = ALLOC(struct bodyNode);
        bod->stmt_list = stmt_list();
        ttype = getToken();
        if (ttype == RBRACE)
            return bod;
        else
            syntax_error("body. RBRACE expected");
    }
    else
        syntax_error("body. LBRACE expected");
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct programNode* program()
{
    struct programNode* prog;
    
    prog = ALLOC(struct programNode);
    ttype = getToken();
    if (ttype == TYPE || ttype == VAR || ttype == LBRACE)
    {
        ungetToken();
        prog->decl = decl();
        prog->body = body();
        return prog;
    }
    else
        syntax_error("program. TYPE or VAR or LBRACE expected");
    return NULL; // control never reaches here, this is just for the sake of GCC
}










/******* This function allocates memory to all the arrays used in the funciton *******/


void initialiseMemory(){
    Type.TypeArray = (char** )malloc(sizeof(char* )*MAX_SIZE);
    
    for (i = 0; i<MAX_SIZE; i++) {
        Type.TypeArray[i] = (char* )malloc(sizeof(char)* MAX_SIZE);
    }
    
    Var.VarArray = (char** )malloc(sizeof(char* )*MAX_SIZE);
    
    for (i = 0; i<MAX_SIZE; i++) {
        Var.VarArray[i] = (char* )malloc(sizeof(char)* MAX_SIZE);
    }
    
    tempType.tempTypeArray = (char** )malloc(sizeof(char* )*MAX_SIZE);
    
    for (i = 0; i<MAX_SIZE; i++) {
        tempType.tempTypeArray[i] = (char* )malloc(sizeof(char)* MAX_SIZE);
    }
    
    tempVar.tempVarArray = (char** )malloc(sizeof(char* )*MAX_SIZE);
    
    for (i = 0; i<MAX_SIZE; i++) {
        tempVar.tempVarArray[i] = (char* )malloc(sizeof(char)* MAX_SIZE);
    }
    
    tempCheck.tempCheckArray = (char** )malloc(sizeof(char* )*MAX_SIZE);
    
    for (i = 0; i<MAX_SIZE; i++) {
        tempCheck.tempCheckArray[i] = (char* )malloc(sizeof(char)* MAX_SIZE);
    }
    
    implicitVar.implicitVarArray = (char** )malloc(sizeof(char* )*MAX_SIZE);
    
    for (i = 0; i<MAX_SIZE; i++) {
        implicitVar.implicitVarArray[i] = (char* )malloc(sizeof(char)* MAX_SIZE);
    }
}

/** This function initializes the left expression array which is used for type checking in statements **/
void initialiseLeftExpr(){
    int z = 0;
    for(z = 0;z<MAX_SIZE;z++){
        leftExpr[z] = -1;
    }
    p = 0;
}

/***** These functions adds type and var to the arrays for error checking and type mismatch ******/
void addToTempTypeList(char* token1){
    strcpy(tempType.tempTypeArray[j++], token1);
    tempType.sequence = count++;
    return;
}

void addToTypeList(char* token1, int ttype, int sort){
    strcpy(Type.TypeArray[n], token1);
    
    if (sort == 1) {
        Type.sort[n] = 1;
    }else if (sort == 2){
        Type.sort[n] = 2;
    }
    
    if (ttype == INT || ttype == NUM) {
        Type.typeNumber[n] = 10;
        Type.ID[n] = 0;
    }else if (ttype == REAL || ttype == REALNUM){
        Type.typeNumber[n] = 11;
        Type.ID[n] = 0;
    }else if (ttype == STRING){
        Type.typeNumber[n] = 12;
        Type.ID[n] = 0;
    }else if (ttype == BOOLEAN){
        Type.typeNumber[n] = 13;
        Type.ID[n] = 0;
    }else if (ttype == LONG){
        Type.typeNumber[n] = 14;
        Type.ID[n] = 0;
    }else if (ttype == ID){
        Type.ID[n] = 1;
        Type.typeNumber[n] = 0;
    }else{
        syntax_error("token type. Token type expected.");
    }
    n++;
    return;
}

void checkTokenType(int ttype){
    if (ttype == INT || ttype == NUM) {
        int x;
        for (x = 0; x<n; x++) {
            if (Type.ID[x] == 1) {
                Type.typeNumber[x] = 10;
                Type.ID[x] = 0;
            }
        }
    }else if (ttype == REAL || ttype == REALNUM) {
        int x;
        for (x = 0; x<n; x++) {
            if (Type.ID[x] == 1) {
                Type.typeNumber[x] = 11;
                Type.ID[x] = 0;
            }
        }
    }else if (ttype == STRING) {
        int x;
        for (x = 0; x<n; x++) {
            if (Type.ID[x] == 1) {
                Type.typeNumber[x] = 12;
                Type.ID[x] = 0;
            }
        }
    }else if (ttype == BOOLEAN) {
        int x;
        for (x = 0; x<n; x++) {
            if (Type.ID[x] == 1) {
                Type.typeNumber[x] = 13;
                Type.ID[x] = 0;
            }
        }
    }else if (ttype == LONG) {
        int x;
        for (x = 0; x<n; x++) {
            if (Type.ID[x] == 1) {
                Type.typeNumber[x] = 14;
                Type.ID[x] = 0;
            }
        }
    }
    return;
}

void addToTempVarList(char* token1){
    strcpy(tempVar.tempVarArray[k++], token1);
    tempVar.sequence = count++;
    return;
}


void addToVarList(char* token1, int ttype, int sort){
    strcpy(Var.VarArray[o], token1);
    if (sort == 3) {
        Var.sort[o] = 3;
    }else if (sort == 4){
        Var.sort[o] = 4;
    }
    
    if (ttype == INT || ttype == NUM) {
        Var.typeNumber[o] = 10;
        Var.ID[o] = 0;
    }else if (ttype == REAL || ttype == REALNUM){
        Var.typeNumber[o] = 11;
        Var.ID[o] = 0;
    }else if (ttype == STRING){
        Var.typeNumber[o] = 12;
        Var.ID[o] = 0;
    }else if (ttype == BOOLEAN){
        Var.typeNumber[o] = 13;
        Var.ID[o] = 0;
    }else if (ttype == LONG){
        Var.typeNumber[o] = 14;
        Var.ID[o] = 0;
    }else if (ttype == ID){
        Var.ID[o] = 1;
        Var.typeNumber[o] = 0;
    }else{
        syntax_error("token type. Token type expected.");
    }
    o++;
    return;
}

void checkVarType(int ttype){
    if (ttype == INT || ttype == NUM) {
        int x;
        for (x = 0; x<o; x++) {
            if (Var.ID[x] == 1) {
                Var.typeNumber[x] = 10;
                Var.ID[x] = 0;
            }
        }
    }else if (ttype == REAL || ttype == REALNUM) {
        int x;
        for (x = 0; x<o; x++) {
            if (Var.ID[x] == 1) {
                Var.typeNumber[x] = 11;
                Var.ID[x] = 0;
            }
        }
    }else if (ttype == STRING) {
        int x;
        for (x = 0; x<o; x++) {
            if (Var.ID[x] == 1) {
                Var.typeNumber[x] = 12;
                Var.ID[x] = 0;
            }
        }
    }else if (ttype == BOOLEAN) {
        int x;
        for (x = 0; x<o; x++) {
            if (Var.ID[x] == 1) {
                Var.typeNumber[x] = 13;
                Var.ID[x] = 0;
            }
        }
    }else if (ttype == LONG) {
        int x;
        for (x = 0; x<o; x++) {
            if (Var.ID[x] == 1) {
                Var.typeNumber[x] = 14;
                Var.ID[x] = 0;
            }
        }
    }
    return;
}

void addToTempCheckArray(char* token){
    strcpy(tempCheck.tempCheckArray[l++], token);
    //    tempCheck.sequence = count++;
    return;
}

void addToImplicitVarList(char* token){
    strcpy(implicitVar.implicitVarArray[m++], token);
    //    implicitVar.sequence = count++;
    return;
}

/***** This function checks if same type is declared twice ******/

void checkDuplicateTypes(){
    int x = 0;
    int y = 0;
    
    for (x = 0; x<j; x++) {
        for (y = x + 1; y<j; y++) {
            if (!strcmp(tempType.tempTypeArray[x], tempType.tempTypeArray[y])) {
                printf("ERROR CODE 0\n");
                exit(1);
            }
        }
    }
    return;
}

/***** This function checks if same variable is declared twice ******/

void checkDuplicateVars(){
    int x = 0;
    int y = 0;
    
    for (x = 0; x<k; x++) {
        for (y = x + 1; y<k; y++) {
            if (!strcmp(tempVar.tempVarArray[x], tempVar.tempVarArray[y])) {
                printf("ERROR CODE 2\n");
                exit(1);
            }
        }
    }
    return;
}

/***** This function checks if type is declared as variable or vice - versa ******/

void checkBothArrays(){
    int x = 0;
    int y = 0;
    
    for (x = 0; x<j; x++) {
        for (y = 0; y<k; y++) {
            if (!strcmp(tempType.tempTypeArray[x], tempVar.tempVarArray[y])) {
                if (tempType.sequence>tempVar.sequence) {
                    printf("ERROR CODE 4\n");
                    exit(1);
                }else if(tempType.sequence<tempVar.sequence){
                    printf("ERROR CODE 1\n");
                    exit(1);
                }
            }
        }
    }
    return;
}

/*** This functions prevents adding of same token more than once ***/

int checkBeforeAdd(char* token){
    int x = 0;
    int flag = 0;
    for (x= 0; x<k; x++) {
        if (!strcmp(token, tempVar.tempVarArray[x])) {
            flag = 1;
            break;
        }
    }
    return flag;
}

int checkTypeDupBody(char* token){
    int x = 0;
    int flag = 0;
    for (x= 0; x<j; x++) {
        if (!strcmp(token, tempType.tempTypeArray[x])) {
            flag = 1;
            break;
        }
    }
    return flag;
}

/********* This function checks if the type is implicit or not ******/
int checkImplicitType(char* token){
    int flag = 0;
    
    int x = 0;
    for (x= 0; x<l; x++) {
        if (!strcmp(token, tempCheck.tempCheckArray[x])) {
            flag = 1;
            break;
        }
    }
    for (x= 0; x<j; x++) {
        if (!strcmp(token, tempType.tempTypeArray[x])) {
            flag = 1;
            break;
        }
    }
    return flag;
}

/**** this finds the type number of left var ***/
void findLeftNumber(char* id){
    int x = 0;
    for (x = 0; x<o; x++) {
        if (!strcmp(id, Var.VarArray[x])) {
            leftNumber = Var.typeNumber[x];
            break;
        }
    }
    return;
}

/**** this finds the type number of all variables in right hand side of the statements ****/
void checkFactorNumber(char* token){
    int x = 0;
    for (x = 0; x<o; x++) {
        if (!strcmp(token, Var.VarArray[x])) {
            leftExpr[p] = Var.typeNumber[x];
            p++;
            break;
        }
    }
    return;
}

/****** This checks for type mis match error, else provides same type to differnt variables involved in statements *****/

void checkForError(){
    int x= 0;
    int y = 0;
    int w = 0;
    
    for (x=0; x<MAX_SIZE && leftExpr[x] != -1; x++) {
        for (y = x+1; y<MAX_SIZE && leftExpr[y] != -1; y++) {
            if (leftExpr[x] != leftExpr[y]) {
                if ((leftExpr[x]>=10 && leftExpr[x]<=14) && (leftExpr[y]>=10 && leftExpr[y]<=14) ) {
                    printf("ERROR CODE 3\n");
                    exit(1);
                }
                else if ((leftExpr[x]>=10 && leftExpr[x]<=14) && (leftExpr[y]>14)){
                    for (w = 0; w<o; w++) {
                        if (Var.typeNumber[w] == leftExpr[y]) {
                            Var.typeNumber[w] = leftExpr[x];
                        }
                    }
                    for (w = 0; w<n; w++) {
                        if (Type.typeNumber[w] == leftExpr[y]) {
                            Type.typeNumber[w] = leftExpr[x];
                        }
                    }
                    leftExpr[y] = leftExpr[x];
                }
                else if ((leftExpr[y]>=10 && leftExpr[y]<=14) && (leftExpr[x]>14)){
                    for (w = 0; w<o; w++) {
                        if (Var.typeNumber[w] == leftExpr[x]) {
                            Var.typeNumber[w] = leftExpr[y];
                        }
                    }
                    for (w = 0; w<n; w++) {
                        if (Type.typeNumber[w] == leftExpr[x]) {
                            Type.typeNumber[w] = leftExpr[y];
                        }
                    }
                    leftExpr[x] = leftExpr[y];
                }
                else if ((leftExpr[x]>14) && (leftExpr[y]>14)){
                    for (w = 0; w<o; w++) {
                        if (Var.typeNumber[w] == leftExpr[y]) {
                            Var.typeNumber[w] = leftExpr[x];
                        }
                    }
                    for (w = 0; w<n; w++) {
                        if (Type.typeNumber[w] == leftExpr[y]) {
                            Type.typeNumber[w] = leftExpr[x];
                        }
                    }
                    leftExpr[y] = leftExpr[x];
                }
            }
        }
    }
    return;
}
/**** this specifically checks the assignment statement *****/

void assign_stmtCheck(){
    int w = 0;
    if ((leftNumber >=10 && leftNumber <=14) && (rightNumber>=10 && rightNumber<=14)) {
        printf("ERROR CODE 3\n");
        exit(1);
    }else if ((leftNumber >=10 && leftNumber <=14) && rightNumber>14){
        for (w = 0; w<o; w++) {
            if (Var.typeNumber[w] == rightNumber) {
                Var.typeNumber[w] = leftNumber;
            }
        }
        for (w = 0; w<n; w++) {
            if (Type.typeNumber[w] == rightNumber) {
                Type.typeNumber[w] = leftNumber;
            }
        }
        rightNumber = leftNumber;
    }else if ((rightNumber >=10 && rightNumber <=14) && leftNumber>14){
        for (w = 0; w<o; w++) {
            if (Var.typeNumber[w] == leftNumber) {
                Var.typeNumber[w] = rightNumber;
            }
        }
        for (w = 0; w<n; w++) {
            if (Type.typeNumber[w] == leftNumber) {
                Type.typeNumber[w] = rightNumber;
            }
        }
        leftNumber = rightNumber;
    }else if (rightNumber >14 && leftNumber>14){
        for (w = 0; w<o; w++) {
            if (Var.typeNumber[w] == leftNumber) {
                Var.typeNumber[w] = rightNumber;
            }
        }
        for (w = 0; w<n; w++) {
            if (Type.typeNumber[w] == leftNumber) {
                Type.typeNumber[w] = rightNumber;
            }
        }
        leftNumber = rightNumber;
    }
    return;
}


int main()
{
    struct programNode* parseTree;
    
    initialiseMemory();
    initialiseLeftExpr();
    
    parseTree = program();
    

    
// TODO: do type checking & print output according to project specification
    return 0;
}