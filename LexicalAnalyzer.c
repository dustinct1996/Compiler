#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INDENTIFIER_LENGTH 11
#define MAX_NUMBER_LENGTH 5

// Enum types representing each of the token symbols.
typedef enum
{
    nulsym = 1, indentsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,
    slashsym = 7, oddsym = 8, eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17,
    semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22,
    ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
    varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsym = 33
} token_type;

// Determines if the current string is an identifier.
// If so, return 2 (the identifier token type).
int isIdent(char* string)
{
    int i;

    if(!(string[0] >= 'a' && string[0] <= 'z') && !(string[0] >= 'A' && string[0] <= 'Z'))
        return 0;


    for(i = 1; i < strlen(string); i++)
        if(!(string[i] >= 'a' && string[i] <= 'z') && !(string[i] >= 'A' && string[i] <= 'Z') && !(string[i] >= '0' && string[i] <= '9'))
            return 0;

    return indentsym;
}

// Determines if the current string is a digit.
// If so, return 3 (the digit token type).
int isDigit(char* string)
{
    int i;

    for(i = 0; i < strlen(string); i++)
        if(!(string[i] >= '0' && string[i] <= '9'))
            return 0;

    return numbersym;
}

// Determines if the current string is one of the following operators/symbols.
// If so, return the associated token type.
int isSign(char* string)
{
    if(strcmp(string, "+") == 0)
        return plussym;
    else if(strcmp(string, "-") == 0)
        return minussym;
    else if(strcmp(string, "*") == 0)
        return multsym;
    else if(strcmp(string, "/") == 0)
        return slashsym;
    else if(strcmp(string, "odd") == 0)
        return oddsym;
    else if(strcmp(string, "=") == 0)
        return eqlsym;
    else if(strcmp(string, "!=") == 0)
        return neqsym;
    else if(strcmp(string, "<") == 0)
        return lessym;
    else if(strcmp(string, "<=") == 0)
        return leqsym;
    else if(strcmp(string, ">") == 0)
        return gtrsym;
    else if(strcmp(string, ">=") == 0)
        return geqsym;
    else if(strcmp(string, "(") == 0)
        return lparentsym;
    else if(strcmp(string, ")") == 0)
        return rparentsym;
    else if(strcmp(string, ",") == 0)
        return commasym;
    else if(strcmp(string, ";") == 0)
        return semicolonsym;
    else if(strcmp(string, ".") == 0)
        return periodsym;
    else if(strcmp(string, ":=") == 0)
        return becomessym;

    return 0;
}

int isCharSign(char character)
{
    if(character == '+')
        return plussym;
    else if(character == '-')
        return minussym;
    else if(character == '*')
        return multsym;
    else if(character == '/')
        return slashsym;
    else if(character == '=')
        return eqlsym;
    else if(character == '<')
        return lessym;
    else if(character == '>')
        return gtrsym;
    else if(character == '(')
        return lparentsym;
    else if(character == ')')
        return rparentsym;
    else if(character == ',')
        return commasym;
    else if(character == ';')
        return semicolonsym;
    else if(character == '.')
        return periodsym;

    return 0;
}

// Determines if the current string is one of the following reserved words.
// If so, return the associated token type.
int isReservedWord(char* string)
{
    if(strcmp(string, "begin") == 0)
        return beginsym;
    else if(strcmp(string, "end") == 0)
        return endsym;
    else if(strcmp(string, "if") == 0)
        return ifsym;
    else if(strcmp(string, "then") == 0)
        return thensym;
    else if(strcmp(string, "while") == 0)
        return whilesym;
    else if(strcmp(string, "do") == 0)
        return dosym;
    else if(strcmp(string, "call") == 0)
        return callsym;
    else if(strcmp(string, "const") == 0)
        return constsym;
    else if(strcmp(string, "var") == 0)
        return varsym;
    else if(strcmp(string, "procedure") == 0)
        return procsym;
    else if(strcmp(string, "write") == 0)
        return writesym;
    else if(strcmp(string, "read") == 0)
        return readsym;
    else if(strcmp(string, "else") == 0)
        return elsym;

    return 0;
}

int main(int argc, char **argv)
{
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");
    int tokenType = 0, charCount = 0, i = 0;
    char **identList = calloc(500, sizeof(char*));
    int *tokenList = calloc(500, sizeof(int));
    char *str = calloc(11, sizeof(char));
    char ch = '\0', c;
    int flag = 0;

    // Syntax check.
    if (argc < 3)
    {
        fprintf(output, "Error: Improper syntax. Refer to the README for compilation instructions.\n");
        return 0;
    }

    // File not found check.
    if (input == NULL)
    {
        fprintf(output, "Input file does not exist.\n");
        return 0;
    }

    // Prints the first part of the output specifications and rewinds the file pointer.
    c = fgetc(input);
    fprintf(output, "Source Program:\n");

    while(c != EOF)
    {
        fprintf(output, "%c", c);
        c = fgetc(input);
    }

    fprintf(output, "\n\n");
    rewind(input);

    // Lexically analyzes the program and prints the lexeme table simultaneously.
    fprintf(output, "Lexeme Table:\nlexeme\t\ttoken type\n");
    while (1)
    {
        ch = fgetc(input);

        // This handles the comments.
        if(ch == '/')
        {
            if((ch = fgetc(input)) == '*')
            {
                while(1)
                {
                    ch = fgetc(input);

                    if(ch == '*')
                    {
                        if(ch = fgetc(input) == '/')                
                            break;
                        else
                            ungetc(ch, input);
                    }
                    
                    if(ch == EOF)
                    {
                        printf("Error. Comment token was not closed.\n");
                        exit(0);
                    }
                    
                    continue;
                }
            }   
            else
                ungetc(ch, input);

            ch = fgetc(input);
        }

        // If the current token one of the operators.
        if(tokenType = isSign(str))
        {
            // Print the token and its associated type to the output file.
            fprintf(output, "%s\t\t%d\n", str, tokenType);

            // Populate the tokenList with the associated token type.
            tokenList[i++] = tokenType;

            // Reset the current string.
            strcpy(str, "");

            // If the current character isn't whitespace, set it as the current string.
            if(!isspace(ch))
                strncat(str, &ch, 1);

            // Breaks if it is the end of file.
            if (ch == EOF)
                break;

            continue;
        }

        if(isCharSign(ch) != 0 && isSign(str) == 0 && strcmp(str, "") != 0 && strcmp(str, ":") != 0)
        {
            // If the current token is a digit.
            if(tokenType = isDigit(str))
            {
                // Error if the number is greater than 99,999.
                if(strlen(str) > MAX_NUMBER_LENGTH)
                {
                    fprintf(output, "Error. Number greater than 5 digits in length: \"%s\"\n", str);
                    exit(0);
                }

                // Print the token and its associated type to the output file.
                fprintf(output, "%s\t\t%d\n", str, tokenType);

                // Update the token list array with the current token, allocate
                // space in the identifier list for the string and insert it.
                tokenList[i] = tokenType;
                identList[i] = calloc(strlen(str), sizeof(char));
                strcpy(identList[i++], str);

                // Reset the string and replace it with the current character.
                strcpy(str, "");
                strncat(str, &ch, 1);

                // Breaks if it is the end of file.
                if (ch == EOF)
                    break;

                continue;
            }

            // If the current token one of the operators.
            if(tokenType = isSign(str))
            {
                strncat(str, &ch, 1);
                continue;
            }

            // If the current token is one of the reserved words.
            if(tokenType = isReservedWord(str))
            {
                // Print the token and its associated type to the output file.
                fprintf(output, "%s\t\t%d\n", str, tokenType);

                // Update the token list array with the current token.
                tokenList[i++] = tokenType;
                
                // Reset the string and replace it with the current character.
                strcpy(str, "");
                strncat(str, &ch, 1);
                
                // Breaks if it is the end of file.
                if (ch == EOF)
                    break;

                continue;
            }
            
            // If the current token is an identifier.
            if(tokenType = isIdent(str))
            {
                // Error if the identifier is greater than 11 characters.
                if (strlen(str) > MAX_INDENTIFIER_LENGTH)
                {
                    fprintf(output, "Error. Identifier greater than 11 characters: \"%s\"\n", str);
                    exit(0);
                }

                // Print the token and its associated type to the output file.
                fprintf(output, "%s\t\t%d\n", str, tokenType);

                // Update the token list array with the current token, allocate
                // space in the identifier list for the string and insert it.
                tokenList[i] = tokenType;
                identList[i] = calloc(strlen(str), sizeof(char));
                strcpy(identList[i++], str);
                
                // Reset the string and replace it with the current character.
                strcpy(str, "");
                strncat(str, &ch, 1);

                // Breaks if it is the end of file.
                if (ch == EOF)
                    break;
                
                continue;
            }
            
            // If none of the above conditions were true, the string is an invalid symbol.
            fprintf(output, "Error. Invalid symbol: \"%s\"\n", str);
            exit(0);
        }

        if((tokenType = isIdent(str)) != 0 && ch == ':')
        {
            // Error if the identifier is greater than 11 characters.
            if (strlen(str) > MAX_INDENTIFIER_LENGTH)
            {
                fprintf(output, "Error. Identifier greater than 11 characters: \"%s\"\n", str);
                exit(0);
            }

            // Print the token and its associated type to the output file.
            fprintf(output, "%s\t\t%d\n", str, tokenType);

            // Update the token list array with the current token, allocate
            // space in the identifier list for the string and insert it.
            tokenList[i] = tokenType;
            identList[i] = calloc(strlen(str), sizeof(char));
            strcpy(identList[i++], str);
            
            // Reset the string and replace it with the current character.
            strcpy(str, "");
            strncat(str, &ch, 1);

            // Breaks if it is the end of file.
            if (ch == EOF)
                break;
            
            continue;
        }
        
        // If the character is whitespace or end of file.
        if(isspace(ch) || ch == EOF)
        {
            if(strcmp(str, "") == 0)
                continue;

            // If the current token type is a digit.
            if(tokenType = isDigit(str))
            {
                // Error if the number is greater than 99,999.
                if(strlen(str) > MAX_NUMBER_LENGTH)
                {
                    fprintf(output, "Error. Number greater than 5 digits in length: \"%s\"\n", str);
                    exit(0);
                }

                // Print the token and its associated type to the output file.
                fprintf(output, "%s\t\t%d\n", str, tokenType);

                // Update the token list array with the current token, allocate
                // space in the identifier list for the string and insert it.
                tokenList[i] = tokenType;
                identList[i] = calloc(strlen(str), sizeof(char));
                strcpy(identList[i++], str);

                // Reset the string.
                strcpy(str, "");

                // Break if it is end of file.
                if (ch == EOF)
                    break;

                continue;
            }

            // If the token is one of the operators.
            if(tokenType = isSign(str))
            {
                // Print the token and its associated type to the output file.
                fprintf(output, "%s\t\t%d\n", str, tokenType);

                // Update the token list array with the current token.
                tokenList[i++] = tokenType;

                // Reset the string.
                strcpy(str, "");

                // Break if it is end of file.
                if (ch == EOF)
                    break;

                continue;
            }
            
            // If the current token is one of the reserved words.
            if(tokenType = isReservedWord(str))
            {
                // Print the token and its associated type to the output file.
                fprintf(output, "%s\t\t%d\n", str, tokenType);

                // Update the token list array with the current token.
                tokenList[i++] = tokenType;

                // Reset the string.
                strcpy(str, "");

                // Break if it is end of file.
                if (ch == EOF)
                    break;

                continue;
            }
            
            // If the current token is an identifier.
            if(tokenType = isIdent(str))
            {
                // Error if the identifier is greater than 11 characters.
                if (strlen(str) > MAX_INDENTIFIER_LENGTH)
                {
                    fprintf(output, "Error. Identifier greater than 11 characters: \"%s\"\n", str);
                    exit(0);
                }
                
                // Error if the identifier begins with a number.
                if (isdigit(str[0]))
                {
                    fprintf(output, "Error. Identifier begins with a number.\"%s\"\n", str);
                    exit(0);
                }

                // Print the token and its associated type to the output file.
                fprintf(output, "%s\t\t%d\n", str, tokenType);

                // Update the token list array with the current token, allocate
                // space in the identifier list for the string and insert it.
                tokenList[i] = tokenType;
                identList[i] = calloc(strlen(str), sizeof(char));
                strcpy(identList[i++], str);

                // Reset the string.
                strcpy(str, "");

                // Break if it is end of file.
                if (ch == EOF)
                    break;
                    
                continue;
            }
            
            // If none of the above conditions were true, the string is an invalid symbol.
            fprintf(output, "Error. Invalid symbol: \"%s\"\n", str);
            exit(0);

        }
        
        // Add the current character to the end of the current string.
        strncat(str, &ch, 1);
    }

    // Prints lexeme list
    fprintf(output, "\nLexeme List:\n");
    for (int k = 0; k < 500; k++)
    {
        if (tokenList[k] == 0)
            break;
        fprintf(output, "%d ", tokenList[k]);

        if (tokenList[k] == 2 || tokenList[k] == 3)
            fprintf(output, "%s ", identList[k]);
        }
    fprintf(output, "\n");


    // Free all allocated memory and close all files.
    for (int j = 0; j < 500; j++)
        free(identList[j]);
    free(str);
    fclose(input);
    fclose(output);
    free(tokenList);
    free(identList);
    return 0;
}
