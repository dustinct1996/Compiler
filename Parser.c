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
    "const, var, write must be followed by identifier.",
    // 4
    "Semicolon or comma missing.",
    // 5
    "Statement expected.",
    // 6
    "Incorrect symbol after statement part in block.",
    // 7
    "Period expected.",
    // 8
    "Semicolon between statements missing.",
    // 9
    "Undeclared identifier.",
    // 10
    "Assignment operator expected.",
    // 11
    "\'then\' expected.",
    // 12
    "Semicolon or \'}\' expected.",
    // 13
    "\'do\' expected.",
    // 14
    "Incorrect symbol following statement.",
    // 15
    "Relational operator expected.",
    // 16
    "Right parenthesis missing.",
    // 17
    "The preceding factor cannot begin with this symbol.",
    // 18
    "An expression cannot begin with this symbol.",
    // 19
    "This number is too large.",
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

int flag;

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
int search(int tok)
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
}

void factor(char **lexemeList, FILE *output)
{
    if (token == identsym)
    {
        if (!search(token))
            ERROR(9, output);
        
        // Load the identifier
        if (symbol_table[searchx].kind == 2)
            emit(3, regPointer++, level, stackPointer--);

        // Lit if it is a const.
        if (symbol_table[searchx].kind == 1)
            emit(1, regPointer, level, symbol_table[searchx].val);
        
        GET_TOKEN(lexemeList);
    }

    else if (token == numbersym)
    {
        GET_TOKEN(lexemeList);
        // Literal
        emit(1, regPointer, level, num);
    }
    
    else if (token == lparentsym)
    {
        GET_TOKEN(lexemeList);
        expression(lexemeList, output);
        if (token != rparentsym)
            ERROR(16, output);
        
        GET_TOKEN(lexemeList);
    }

    else
        ERROR(17, output);
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
    
        if (addop == minussym)
            // Negate
            emit(12, regPointer, level, 0);
    }
    
    else
        term(lexemeList, output);

    while (token == plussym || token == minussym)
    {
        addop = token;
        GET_TOKEN(lexemeList);
        term(lexemeList, output);

        if (addop == plussym)
            // Add
            emit(13, ++regPointer, level, regPointer);
        else
            // Sub
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
                    ERROR(15, output);
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
        flag = 1;
        GET_TOKEN(lexemeList);
        if (token != becomessym)
            ERROR(2, output);

        GET_TOKEN(lexemeList);
        expression(lexemeList, output);
    }

    else if (token == beginsym)
    {
        GET_TOKEN(lexemeList);
        statement(lexemeList, output);

        while (token == semicolonsym)
        {
            if (flag == 1)
                emit(4, regPointer, level, stackPointer);
            flag = 0;
            GET_TOKEN(lexemeList);
            statement(lexemeList, output);
        }

        if (token != endsym)
            ERROR(6, output);

        GET_TOKEN(lexemeList);
    }

    else if (token == ifsym)
    {
        GET_TOKEN(lexemeList);
        condition(lexemeList, output);

        if (token != thensym)
            ERROR(11, output);
        
        GET_TOKEN(lexemeList);
        statement(lexemeList, output);
    }

    else if (token == whilesym)
    {
        int cx1 = cx;
        
        GET_TOKEN(lexemeList);
        condition(lexemeList, output);
        
        int cx2 = cx;
        
        emit(8, regPointer, level, 20);
        
        if (token != dosym)
            ERROR(13, output);
        
        GET_TOKEN(lexemeList);
        statement(lexemeList, output);
        emit(3, 0, 0, 0); // jump
        text[cx2].M = cx;
    }

    else if (token == writesym)
    {
        GET_TOKEN(lexemeList);
        if (token != identsym)
            ERROR(3, output);
        
        expression(lexemeList, output);

        if (token != semicolonsym)
            ERROR(4, output);
        
        // SIO 1, write
        emit(9, regPointer, level, 1);

        GET_TOKEN(lexemeList);
    }
}

void block(char **lexemeList, FILE *output)
{
    // Initial inc.
    emit(6, 0, 0, 7);
    stackPointer += 6;

    if (token == constsym)
    {
        do
        {
            GET_TOKEN(lexemeList);
            if (token != identsym)
                ERROR(3, output);

            GET_TOKEN(lexemeList);
            if (token != eqlsym)
                ERROR(0, output);

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

    statement(lexemeList, output);
}

void program(char **lexemeList, FILE *output)
{
    GET_TOKEN(lexemeList);
    block(lexemeList, output);
    if (token != periodsym)
        ERROR(7, output);
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
    stackPointer = 0, searchx = 0;
    int i = 0;
    flag = 0;

    if (isParserArg())
        fprintf(output, "Parser Output:\n\n");

    program(lexemeList, output);

    if (isParserArg())
    {
        fprintf(output, "No errors, program is syntactically correct.\n\n");
        fprintf(output, "Code Generator Output:\n\n");
        while (text[i].op != 0)
        {
            fprintf(output, "%d %d %d %d\n", text[i].op, text[i].R, text[i].L, text[i].M);
            i++;
        }

        fprintf(output, "\n");
    }
    
    free(symbol_table);
    free(ident);
    return text;
}
