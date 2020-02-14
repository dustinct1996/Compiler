// Names: Christian Young, Dustin Thompson
// NIDs: ch890408, du738547
// Course: COP 3402
// Term: SPRING 2020

#include <stdio.h>
#include <stdlib.h>
#define MAX_INDENTIFIER_LENGTH 11
#define MAX_NUMBER_LENGTH 5
#define MAX_LINE_LENGTH 100

typedef enum
{
    nulsym = 1, indentsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,
    slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17,
    semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22,
    ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
    varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsym = 33
} token_type;

int main(int argc, char **argv)
{
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");

    // I don't know the max line length atm so it is 100 for now.
    char line[MAX_LINE_LENGTH] = "";

    // Syntax check.
    if (argc < 3)
    {
        printf("Error: Improper syntax. Refer to the README for compilation instructions.\n");
        return 0;
    }

    // File not found check.
    if (input == NULL)
    {
        printf("Input file does not exist.\n");
        return 0;
    }

    // Prints for the first part of the output specifications.
    printf("Source Program:\n");
    while (!feof(input))
    {
        fgets(line, MAX_LINE_LENGTH, input);
        printf("%s", line);
    }
    printf("\n");

    // Output the lexeme and the associated token type.
    // Also print the lexeme list.

    printf("Lexeme Table:\nlexeme\ttoken type\n");
    /* PRINT LEXEME TABLE HERE
    */

    printf("Lexeme List:\n");
    /* PRINT LEXEME LIST HERE
    */

    return 0;
}
