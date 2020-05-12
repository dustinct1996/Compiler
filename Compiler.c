#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEXEMES 500
#define MAX_CODE_LENGTH 200

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

int L;
int A;
int V;

int isScannerArg()
{
    return L;
}

int isParserArg()
{
    return A;
}

int isVMArg()
{
    return V;
}

char **LexicallyAnalyze(FILE *input, FILE *output);

instruction *Parse(char **lexemeList, FILE *output);

void VM(instruction *text, FILE *output);

// Source program -> [Lex] -> Lexemes -> [Parser/CodeGen] -> Symbol Table ->  [VM] -> Virtual machine output
int main(int argc, char **argv)
{
    char **lexemeList = calloc(MAX_LEXEMES, sizeof(char*));
    instruction *text = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
    FILE *input;
    FILE *output;
    L = 0;
    A = 0;
    V = 0;
    
    // Syntax check.
    if (argc < 3 || argc > 6)
    {
        printf("Error: Improper syntax. Refer to the README for compilation instructions.\n");
        return 0;
    }

    if (argc == 6)
    {
        L = 1;
        A = 1;
        V = 1;

        input = fopen(argv[4], "r");
        output = fopen(argv[5], "w");
    }

    else if (argc == 5)
    {
        for (int i = 1; i < 3; i++)
        {
            if (strcmp(argv[i], "-l") == 0)
                L = 1;
            else if (strcmp(argv[i], "-a") == 0)
                A = 1;
            else if (strcmp(argv[i], "-v") == 0)
                V = 1;
        }

        input = fopen(argv[3], "r");
        output = fopen(argv[4], "w");
    }

    else if (argc == 4)
    {
        if (strcmp(argv[1], "-l") == 0)
            L = 1;
        else if (strcmp(argv[1], "-a") == 0)
            A = 1;
        else if (strcmp(argv[1], "-v") == 0)
            V = 1;

        input = fopen(argv[2], "r");
        output = fopen(argv[3], "w");
    }

    else if (argc == 3)
    {
        input = fopen(argv[1], "r");
        output = fopen(argv[2], "w");
    }
    // File not found check.
    if (input == NULL)
    {
        printf("Input file does not exist.\n");
        return 0;
    }

    // Pass the PL/0 code to the lexical analyzer to construct the lexemes.
    // Prints the lexeme list and the symbolic representation.
    lexemeList = LexicallyAnalyze(input, output);

    // Pass the generated lexemes to the parser to create the PM/0 code.
    // Prints the intermediate code generation.
    text = Parse(lexemeList, output);

    // Pass the PM/0 code to the virtual machine to be executed.
    // Prints the result of the virtual machine execution.
    VM(text, output);

    for (int i = 0; i < MAX_LEXEMES; i++)
        free(lexemeList[i]);
    free(lexemeList);
    free(text);
    fclose(input);
    fclose(output);
    return 0;
}
