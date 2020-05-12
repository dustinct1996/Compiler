#include <stdio.h>
#include <stdlib.h>
#define MAX_STACK_HEIGHT 40
#define MAX_CODE_LENGTH 200
#define MAX_LEXI_LEVELS 3
#define NUM_REGISTERS 8

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

const char opStrings[24][4] =
{
    "lit\0",
    "rtn\0",
    "lod\0",
    "sto\0",
    "cal\0",
    "inc\0",
    "jmp\0",
    "jpc\0",
    "sio\0",
    "sio\0",
    "sio\0",
    "neg\0",
    "add\0",
    "sub\0",
    "mul\0",
    "div\0",
    "odd\0",
    "mod\0",
    "eql\0",
    "neq\0",
    "lss\0",
    "leq\0",
    "gtr\0",
    "geq\0"
};

// Stack, Size 40
int *stack;

// Register File, Size 8
int *RF;

/* Registers and initial values */

// STACK POINTER
int SP = 0;
// BASE POINTER
int BP = 1;
// PROGRAM COUNTER
int PC = 0;

// HALT variable. Program continues until it is set to 1.
int HALT = 0;

// Helper function provided by Montagne.
// This function will be helpful to find a variable in a different Activation Record
// some L levels down. For example in the instruction:
// STO R, L, M - you can do stack[base(ir.L, bp) + ir[IR.M] = RF[IR.R] to store the
// content of register into the stack L levels down from the current AR. RF stand
// for register file.
int base(int L, int base)
{
    // find base L levels down
    int BL;
    BL = base;

    while (L > 0)
    {
        BL = stack[BL + 1];
        L--;
    }

    return BL;
}

// 01 – LIT(R, 0, M) Loads a constant value (literal) M into Register R
void LIT(int R, int L, int M)
{
    RF[R] = M;
}

// 02 – RTN(0, 0, 0) Returns from a subroutine and restore the caller environment
void RTN(int R, int L, int M)
{
    SP = BP - 1;
    BP = stack[SP + 3];
    PC = stack[SP + 4];
}

// 03 – LOD(R, L, M) Load value into a selected register from the stack
// location at offset M from L lexicographical levels down
void LOD(int R, int L, int M)
{
    RF[R] = stack[base(L, BP) + M];
}

// 04 – STO(R, L, M) Store value from a selected register in the stack
// location at offset M from L lexicographical levels down
void STO(int R, int L, int M)
{
    stack[base(L, BP) + M] = RF[R];
}

//05 – CAL(0, L, M) Call procedure at code index M (generates new Activation
// Record and pc <- M)
void CAL(int R, int L, int M)
{
    // Space to return value.
    stack[SP + 1] = 0;
    // Static Link (SL)
    stack[SP + 2] = base(L, BP);
    // Dynamic Link (DL)
    stack[SP + 3] = BP;
    // Return Address (RA)
    stack[SP + 4] = PC;

    BP = SP + 1;
    PC = M;
}

// 06 – INC(0, 0, M) Allocate M locals (increment sp by M). First four are
// Functional Value, Static Link (SL), Dynamic Link (DL), and Return Address (RA).
void INC(int R, int L, int M)
{
    SP = SP + M;
}

// 07 – JMP(0, 0, M) Jump to instruction M
void JMP(int R, int L, int M)
{
    PC = M;
}

// 08 – JPC(R, 0, M) Jump to instruction M if R = 0
void JPC(int R, int L, int M)
{
    if (RF[R] == 0)
        PC = M;
}

// 09 – SIO(R, 0, 1) Write a register to the screen.
// 10 – SIO(R, 0, 2) Read in input from the user and store it in a register.
// 11 – SIO(0, 0, 3) End of Program.
void SIO(int R, int L, int M)
{
    switch (M)
    {
        case 1:
        printf("%d\n", RF[R]);
        break;

        case 2:
        scanf("%d", &RF[R]);
        break;

        // Stops the program.
        case 3:
        HALT = 1;
        break;
    }
}

/* Note: i = R, j = L, k = M */

// 12 - NEG() R[i] <- -R[j]
void NEG(int R, int L, int M)
{
    RF[R] = -RF[R];
}

// 13 - ADD() R[i] <- (R[j] + R[k])
void ADD(int R, int L, int M)
{
    RF[R] = RF[L] + RF[M];
}

// 14 - SUB() R[i] <- (R[j] - R[k])
void SUB(int R, int L, int M)
{
    RF[R] = RF[L] - RF[M];
}

// 15 - MUL() R[i] <- (R[j] * R[k])
void MUL(int R, int L, int M)
{
    RF[R] = RF[L] * RF[M];
}

// 16 - DIV() R[i] <- (R[j] / R[k])
void DIV(int R, int L, int M)
{
    RF[R] = RF[L] / RF[M];
}

// 17 - ODD() R[i] <- R[i] mod 2;
void ODD(int R, int L, int M)
{
    RF[R] = RF[R] % 2;
}

// 18 - MOD() R[i] <- (R[j] mod R[k])
void MOD(int R, int L, int M)
{
    RF[R] = RF[L] % RF[M];
}

// 19 - EQL() R[i] <- (R[j] == R[k])
void EQL(int R, int L, int M)
{
    if (RF[L] == RF[M])
        RF[R] = 1;
    else
        RF[R] = 0;
}

// 20 - NEQ() R[i] <- (R[j] != R[k])
void NEQ(int R, int L, int M)
{
    if (RF[L] != RF[M])
        RF[R] = 1;
    else
        RF[R] = 0;
}

// 21 - LSS() R[i] <- (R[j] < R[k])
void LSS(int R, int L, int M)
{
    if (RF[L] < RF[M])
        RF[R] = 1;
    else
        RF[R] = 0;
}

// 22 - LEQ() R[i] <- (R[j] <= R[k])
void LEQ(int R, int L, int M)
{
    if (RF[L] <= RF[M])
        RF[R] = 1;
    else
        RF[R] = 0;
}

// 23 - GTR() R[i] <- (R[j] > R[k])
void GTR(int R, int L, int M)
{
    if (RF[L] > RF[M])
        RF[R] = 1;
    else
        RF[R] = 0;
}

// 24 - GEQ() R[i] <- (R[j] >= R[k])
void GEQ(int R, int L, int M)
{
    if (RF[L] >= RF[M])
        RF[R] = 1;
    else
        RF[R] = 0;
}

int main(int argc, char **argv)
{
    // Array of the instructions. The maximum size is 200 by definition
    // of MAX_CODE_LENGTH (200)
    instruction *text = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");
    instruction IR;
    int line = 0;

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

    // Initializes all indices of the stack to 0.
    stack = calloc(MAX_STACK_HEIGHT, sizeof(int));
    
    // Initializes all registers to 0.
    RF = calloc(NUM_REGISTERS, sizeof(int));

    // Performs the first step of the "Output File" specifications. Fills the
    // Instruction struct defined above and prints out the program in interpreted
    // assembly language with line numbers.
    printf("Line\tOP\tR\tL\tM\n");
    fprintf(output, "Line\tOP\tR\tL\tM\n");
    while (!feof(input))
    {
        // Fills instruction struct.
        fscanf(input, "%d %d %d %d", &text[line].op, &text[line].R, &text[line].L, &text[line].M);

        // Outputs to the screen.
        printf("%d\t%s\t%d\t%d\t%d\n", line, opStrings[text[line].op - 1], text[line].R, text[line].L, text[line].M);

        // Outputs to the output file.
        fprintf(output, "%d\t%s\t%d\t%d\t%d\n", line, opStrings[text[line].op - 1], text[line].R, text[line].L, text[line].M);

        // Increment line #.
        line += 1;
    }

    // Perfroms the actual program.
    // Adjusts registers, data, stack, etc. as per the instruction.
    // Will also print out the execution of the program as displayed
    // in part (2) of the output file in Appendix C.

    // Initial values
    printf("\n\t\t\tpc\tbp\tsp\tregisters\n");
    fprintf(output, "\n\t\t\tpc\tbp\tsp\tregisters\n");
    printf("Initial values\t\t0\t1\t0\t0 0 0 0 0 0 0 0\nStack: ");
    fprintf(output, "Initial values\t\t0\t1\t0\t0 0 0 0 0 0 0 0\nStack: ");

    for (int i = 0; i < MAX_STACK_HEIGHT; i++)
    {
        printf("0 ");
        fprintf(output, "0 ");
    }

    printf("\n\n\t\t\tpc\tbp\tsp\tregisters\n");
    fprintf(output, "\n\n\t\t\tpc\tbp\tsp\tregisters\n");
    while (!HALT)
    {
        // FETCH
        printf("%d ", PC);
        fprintf(output, "%d ", PC);
        IR = text[PC];
        PC = PC + 1;

        // EXECUTE
        switch (IR.op)
        {
            // LIT
            case 1:
            LIT(IR.R, 0, IR.M);
            break;

            // RTN
            case 2:
            RTN(0, 0, 0);
            break;

            // LOD
            case 3:
            LOD(IR.R, IR.L, IR.M);
            break;

            // STO
            case 4:
            STO(IR.R, IR.L, IR.M);
            break;

            // CAL
            case 5:
            CAL(0, IR.L, IR.M);
            break;

            // INC
            case 6:
            INC(0, 0, IR.M);
            break;

            // JMP
            case 7:
            JMP(0, 0, IR.M);
            break;

            // JPC
            case 8:
            JPC(IR.R, 0, IR.M);
            break;
            
            // SIO (M = 1)
            case 9:
            SIO(IR.R, 0, 1);
            break;

            // SIO (M = 2)
            case 10:
            SIO(IR.R, 0, 2);
            break;

            // SIO (M = 3)
            case 11:
            SIO(IR.R, 0, 3);
            break;

            // NEG
            case 12:
            NEG(IR.R, 0, 0);
            break;

            // ADD
            case 13:
            ADD(IR.R, IR.L, IR.M);
            break;

            // SUB
            case 14:
            SUB(IR.R, IR.L, IR.M);
            break;

            // MUL
            case 15:
            MUL(IR.R, IR.L, IR.M);
            break;

            // DIV
            case 16:
            DIV(IR.R, IR.L, IR.M);
            break;

            // ODD
            case 17:
            ODD(IR.R, 0, 0);
            break;

            // MOD
            case 18:
            MOD(IR.R, IR.L, IR.M);
            break;

            // EQL
            case 19:
            EQL(IR.R, IR.L, IR.M);
            break;

            // NEQ
            case 20:
            NEQ(IR.R, IR.L, IR.M);
            break;

            // LSS
            case 21:
            LSS(IR.R, IR.L, IR.M);
            break;

            // LEQ
            case 22:
            LEQ(IR.R, IR.L, IR.M);
            break;

            // GTR
            case 23:
            GTR(IR.R, IR.L, IR.M);
            break;

            // GEQ
            case 24:
            GEQ(IR.R, IR.L, IR.M);
            break;
        }

        printf("%s %d %d %d\t", opStrings[IR.op - 1], IR.R, IR.L, IR.M);
        fprintf(output, "%s %d %d %d\t", opStrings[IR.op - 1], IR.R, IR.L, IR.M);
        printf("\t%d\t %d\t %d\t%d %d %d %d %d %d %d %d\n", PC, BP, SP, RF[0], RF[1], RF[2], RF[3], RF[4], RF[5], RF[6], RF[7]);
        fprintf(output, "\t%d\t %d\t %d\t%d %d %d %d %d %d %d %d\n", PC, BP, SP, RF[0], RF[1], RF[2], RF[3], RF[4], RF[5], RF[6], RF[7]);
        
        printf("Stack: ");
        fprintf(output, "Stack: ");

        for (int i = 1; i <= SP; i++)
        {
            if (i > 1 && (i - 1) % 6 == 0)
            {
                printf("| ");
                fprintf(output, "| ");
            }
            printf("%d ", stack[i]);
            fprintf(output, "%d ", stack[i]);
        }

        printf("\n\n");
        fprintf(output, "\n\n");
    }
    
    // Close files and free allocated memory
    free(stack);
    free(RF);
    free(text);
    fclose(input);
    fclose(output);
    return 0;
}
