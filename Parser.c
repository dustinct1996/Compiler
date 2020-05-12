// Names: Christian Young, Dustin Thompson
// NIDs: ch890408, du738547
// Course: COP 3402
// Term: SPRING 2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SYMBOL_TABLE_SIZE 200
#define MAX_CODE_LENGTH 200

typedef enum
{
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,
    slashsym = 7, oddsym = 8, eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17,
    semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22,
    ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
    varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsym = 33
} token_type;

typedef struct instruction
{
    // Opcode
    int op;
    // Register
    int R;
    // Lexicographical level
    int L;
    // Modifier
    int M;
} instruction;

// Note: Not all of these error messages may be used, and you may choose to
// create some error messages of your own to more accurately represent certain situations.
const char *error[] = 
{
    // 0
    "Use '=' instead of ':=' for const",
    // 1
    "'=' must be followed by a number.",
    // 2
    "Identifier must be followed by '='.",
    // 3
    "const, var, write, procedure must be followed by identifier.",
    // 4
    "Semicolon or comma missing.",
    // 5
    "Incorrect symbol after procedure declaration.",
    // 6
    "Statement expected.",
    // 7
    "Incorrect symbol after statement part in block.",
    // 8
    "Period expected.",
    // 9
    "Undeclared identifier.",
    // 10
    "Assignment to constant or procedure is not allowed.",
    // 11
    "Assignment operator expected.",
    // 12
    "\'call\' must be followed by an identifier.",
    // 13
    "Call of a constant or variable is meaningless.",
    // 14
    "\'then\' expected.",
    // 15
    "\'do\' expected.",
    // 16
    "Relational operator expected.",
    // 17
    "Expression must not contain a procedure.",
    // 18
    "Right parenthesis missing.",
    // 19
    "An expression cannot begin with this symbol.",
};

typedef struct symbol
{
    // const = 1, var = 2, procedure = 3
    int kind;

    // name up to 11 chars
    char name[12];

    // number (ASCII value)
    int val;

    // L level
    int level;

    // M address
    int addr;
} symbol;

instruction *text;
symbol *symbol_table;

int isParserArg();
void expression(char **lexemeList, FILE *output);

// cx is the index of the array for emit.
int cx;
// sx is the index of the array for enter.
int sx;
// The current token.
int token;
// tx is the index of the array for lexemeList.
int tx;
// Register pointer
int regPointer;
// Stack Pointer
int stackPointer;
// A saved identifier used in GET_TOKEN()
char *ident;
// A saved number used in GET_TOKEN()
int num;
// Lexicographical level
int level;
// Address
int address;
// Search index
int searchx;
// flag for end.
int flag;
// Line number to keep track of the PC (for procedures).
int lineNumber;
// a array;
int *a;
// a counter;
int ax;

void ERROR(int error_code, FILE *output)
{
    fprintf(output, "%s", error[error_code]);
    exit(0);
}

void GET_TOKEN(char **lexemeList)
{
    token = atoi(lexemeList[tx]);

    // If the token is an identifer, save it in ident.
    if (token == identsym)
    {
        tx++;
        strcpy(ident, lexemeList[tx]);
    }
    // If the token is a number, save it in num.
    else if (token == numbersym)
    {
        tx++;
        num = atoi(lexemeList[tx]);
    }

    tx++;
}

// Searches for something in the symbol table.
int search()
{
    for (searchx = 0; searchx < MAX_SYMBOL_TABLE_SIZE; searchx++)
    {
        if (strcmp(symbol_table[searchx].name, ident) == 0)
            return 1;
    }
    return 0;
}

// Enter variable into symbol table.
void enter(int kind, char *ident, int val, int L, int addr)
{
    symbol_table[sx].kind = kind;
    strcpy(symbol_table[sx].name, ident);
    symbol_table[sx].val = val;
    symbol_table[sx].level = L;
    symbol_table[sx].addr = addr;
    sx++;
}

// Emit function from slides.
// Code generation.
void emit(int OP, int R, int L, int M)
{   
    text[cx].op = OP;
    text[cx].R = R;
    text[cx].L = L;
    text[cx].M = M;
    cx++;
    lineNumber++;
}

void factor(char **lexemeList, FILE *output)
{
    if (token == procsym)
        ERROR(17, output);

    else if (token == identsym)
    {
        if (!search())
            ERROR(9, output);
        
        // Load the identifier
        if (symbol_table[searchx].kind == 2)
            emit(3, regPointer++, level, stackPointer--);

        // Lit if it is a const.
        if (symbol_table[searchx].kind == 1)
            emit(1, 0, level, symbol_table[searchx].val);
        
        GET_TOKEN(lexemeList);
    }

    else if (token == numbersym)
    {
        GET_TOKEN(lexemeList);
        // Literal
        emit(1, 0, level, num);
    }
    
    else if (token == lparentsym)
    {
        GET_TOKEN(lexemeList);
        expression(lexemeList, output);
        if (token != rparentsym)
            ERROR(18, output);
        
        GET_TOKEN(lexemeList);
    }

    else
        ERROR(19, output);
}

void term(char **lexemeList, FILE *output)
{
    int mulop = 0;

    factor(lexemeList, output);

    while (token == multsym || token == slashsym)
    {
        mulop = token;
        GET_TOKEN(lexemeList);
        factor(lexemeList, output);

        if (mulop == multsym)
            // Multiply
            emit(15, ++regPointer, level, regPointer);
        else
            // Divide
            emit(16, ++regPointer, level, regPointer);
    }
}

void expression(char **lexemeList, FILE *output)
{
    int addop = 0;

    if (token == plussym || token == minussym)
    {
        addop = token;
        GET_TOKEN(lexemeList);
        term(lexemeList, output);
    
        // Negate
        if (addop == minussym)
            emit(12, regPointer, level, 0);
    }
    
    else
        term(lexemeList, output);

    while (token == plussym || token == minussym)
    {
        addop = token;
        GET_TOKEN(lexemeList);
        term(lexemeList, output);

        // Add
        if (addop == plussym)
            emit(13, ++regPointer, level, regPointer);
        
        // Sub
        else
            emit(14, ++regPointer, level, regPointer);

    }
}

void condition(char **lexemeList, FILE *output)
{
    int addop;
    
    if (token == oddsym)
    {
        GET_TOKEN(lexemeList);
        expression(lexemeList, output);
    }
    else
    {
        expression(lexemeList, output);
        addop = token;
        GET_TOKEN(lexemeList);
        expression(lexemeList, output);

        if(addop != eqlsym)
          if(addop != neqsym)
            if(addop != lessym)
              if(addop != leqsym)
                if(addop != gtrsym)
                  if(addop != geqsym)
                    ERROR(16, output);
                  else
                    emit(24, ++regPointer, level, regPointer);
                else
                  emit(23, ++regPointer, level, regPointer);
              else
                emit(22, ++regPointer, level, regPointer);
            else
              emit(21, ++regPointer, level, regPointer);
          else
            emit(20, ++regPointer, level, regPointer);
        else
          emit(19, ++regPointer, level, regPointer);
    }
}

void statement(char **lexemeList, FILE *output)
{
    if (token == identsym)
    {
        if (search(ident))
            if (symbol_table[searchx].kind == 1)
                ERROR(10, output);
        
        flag = 1;
        GET_TOKEN(lexemeList);
        if (token == periodsym)
            ERROR(7, output);
        else if (token != becomessym)
            ERROR(11, output);

        GET_TOKEN(lexemeList);
        expression(lexemeList, output);
    }
    
    else if (token == callsym)
    {
        GET_TOKEN(lexemeList);

        if (token != identsym)
            ERROR(12, output);

        if (search(ident))
            if (symbol_table[searchx].kind == 1 || symbol_table[searchx].kind == 2)
                ERROR(13, output);

        stackPointer += 5;

        // Call procedure at code index
        // TODO: Fix
        emit(5, 0, level++, a[ax] + 1);

        GET_TOKEN(lexemeList);
    }

    else if (token == beginsym)
    {
        // Inc
        emit(6, 0, 0, 6);
        stackPointer += 5;

        GET_TOKEN(lexemeList);

        if (token == semicolonsym)
            ERROR(6, output);

        statement(lexemeList, output);

        while (token == semicolonsym)
        {
            if (flag == 1)
                emit(4, 0, level, regPointer++);
            flag = 0;
            GET_TOKEN(lexemeList);
            statement(lexemeList, output);
        }

        if (token != endsym)
            ERROR(7, output);

        if (!(lexemeList[tx] == NULL))
            GET_TOKEN(lexemeList);
    }

    else if (token == ifsym)
    {
        GET_TOKEN(lexemeList);
        condition(lexemeList, output);

        if (token != thensym)
            ERROR(14, output);
        
        GET_TOKEN(lexemeList);
        statement(lexemeList, output);

        if (token == elsym)
        {
            GET_TOKEN(lexemeList);
            statement(lexemeList, output);
        }

    }

    else if (token == whilesym)
    {
        int cx1 = cx;
        
        GET_TOKEN(lexemeList);
        condition(lexemeList, output);
        
        int cx2 = cx;
        
        emit(8, regPointer, level, 20);
        
        if (token != dosym)
            ERROR(15, output);
        
        GET_TOKEN(lexemeList);
        statement(lexemeList, output);
        emit(3, 0, 0, 0); // jump
        text[cx2].M = cx;
    }

    else if (token == readsym)
    {
        GET_TOKEN(lexemeList);
        if (token != identsym)
            ERROR(3, output);

        if (token != semicolonsym)
            ERROR(4, output);
        
        // SIO 2, read
        emit(10, 0, 0, 2);

        GET_TOKEN(lexemeList);
    }

    else if (token == writesym)
    {
        GET_TOKEN(lexemeList);
        // ?? might not need this
        if (token != identsym)
            ERROR(3, output);
        
        expression(lexemeList, output);

        if (token != semicolonsym)
            ERROR(4, output);
        
        // SIO 1, write
        emit(9, 0, 0, 1);

        GET_TOKEN(lexemeList);
    }
}

void block(char **lexemeList, FILE *output)
{
    // Const dec
    if (token == constsym)
    {
        do
        {
            GET_TOKEN(lexemeList);
            if (token != identsym)
                ERROR(3, output);

            GET_TOKEN(lexemeList);
            if (token == becomessym)
                ERROR(0, output);
            else if (token != eqlsym)
                ERROR(2, output);

            GET_TOKEN(lexemeList);
            if (token != numbersym)
                ERROR(1, output);

            // Enter the const into symbol table.
            enter(1, ident, num, level, address);

            GET_TOKEN(lexemeList);

        } while (token == commasym);

        if (token != semicolonsym)
            ERROR(4, output);

        GET_TOKEN(lexemeList);
    }

    // Var dec
    if (token == varsym)
    {
        do
        {
            GET_TOKEN(lexemeList);
            if (token != identsym)
                ERROR(3, output);

            // Enter the var into symbol table.
            enter(2, ident, 0, level, address);

            GET_TOKEN(lexemeList);

        } while (token == commasym);
        
        if (token != semicolonsym)
            ERROR(4, output);
        
        GET_TOKEN(lexemeList);
    }

    // Proc dec
    if (token == procsym)
    {
        // Save the line number where this procedure was found
        a[ax] = lineNumber;
        // The jump to the first begin after the procedure
        emit(7, 0, 0, 0);

        GET_TOKEN(lexemeList);
        if (token != identsym)
            ERROR(3, output);
        
        GET_TOKEN(lexemeList);

        if (token != semicolonsym)
            ERROR(5, output);
        
        // Enter procedure into symbol table.
        enter(3, ident, 0, level, 0);
        
        GET_TOKEN(lexemeList);

        block(lexemeList, output);

        if (token != semicolonsym)
            ERROR(4, output);

        emit(2, 0, 0, 0);
        // Modify the instruction to jump to the line after this return ^
        text[a[ax]].M = lineNumber;
        ax++;
        
        GET_TOKEN(lexemeList);
    }

    statement(lexemeList, output);
}

void program(char **lexemeList, FILE *output)
{
    GET_TOKEN(lexemeList);
    block(lexemeList, output);
    if (token != periodsym)
        ERROR(8, output);
    else
        // SIO 3 - End program
        emit(11, 0, 0, 3);
}

instruction *Parse(char **lexemeList, FILE *output)
{
    symbol_table = malloc(sizeof(symbol) * MAX_SYMBOL_TABLE_SIZE);
    text = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
    ident = malloc(sizeof(char) * 12);
    tx = 0, cx = 0, sx = 0, token = 0, regPointer = 0, level = 0, address = 0;
    stackPointer = 0, searchx = 0, lineNumber = 0, flag = 0;
    int i = 0;
    int PC = 0;
    a = calloc(100, sizeof(int));
    ax = 0;

    if (isParserArg())
        fprintf(output, "Parser Output:\n\n");

    program(lexemeList, output);

    // Modify the jumps for procedures here

    if (isParserArg())
    {
        fprintf(output, "No errors, program is syntactically correct.\n\n");
        fprintf(output, "Code Generator Output:\n\n");
        while (text[i].op != 0)
        {
            fprintf(output, "%d. %d %d %d %d\n", PC++, text[i].op, text[i].R, text[i].L, text[i].M);
            i++;
        }

        fprintf(output, "\n");
    }
    
    free(symbol_table);
    free(ident);
    return text;
}
