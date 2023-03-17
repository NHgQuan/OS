#include <stdio.h>
#include <stdlib.h>
#include "dataStructure.h"
#include <stdbool.h>

float round2point(float var)
{
    /// @brief round to two decimal point value
    float value = (int)(var * 100 + .5);
    return (float)value / 100;
}

void saveANSInt(int result)
{
    FILE *fptr;

    fptr = fopen("./ANS.txt", "w");

    if (fptr == NULL)
    {
        printf("Error!");
        exit(1);
    }

    fprintf(fptr, "%d", result);
    fclose(fptr);
}

void saveANSFloat(float result)
{
    FILE *fptr;

    fptr = fopen("./ANS.txt", "w");

    if (fptr == NULL)
    {
        printf("Error!");
        exit(1);
    }

    fprintf(fptr, "%.2f", result);
    fclose(fptr);
}

boolean fileExists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return T;
    }
    return F;
}

char *load_file(char const *path)
{
    char *buffer = 0;
    long length;
    FILE *f = fopen(path, "rb"); // was "rb"

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = (char *)malloc((length + 1) * sizeof(char));
        if (buffer)
        {
            fread(buffer, sizeof(char), length, f);
        }
        fclose(f);
    }
    buffer[length] = '\0';
    // for (int i = 0; i < length; i++) {
    //     printf("buffer[%d] == %c\n", i, buffer[i]);
    // }
    // printf("buffer = %s\n", buffer);

    return buffer;
}

char *readANSFile()
{
    if (!fileExists("./ANS.txt"))
        saveANSInt(0);
    return load_file("./ANS.txt");
}

void calculator(char *firstOperand, char *secondOperand, char *operator)
{
    float floatFirstOperand = atof(firstOperand);
    float floatSecondOperand = atof(secondOperand);
    int intFirstOperand = (int)(floatFirstOperand < 0 ? (floatFirstOperand - 0.5) : (floatFirstOperand + 0.5));
    int intSecondOperand = (int)(floatSecondOperand < 0 ? (floatSecondOperand - 0.5) : (floatSecondOperand + 0.5));
    switch (*operator)
    {
    case '+':
    {
        int result = intFirstOperand + intSecondOperand;
        printf("%d", result);
        saveANSInt(result);
        break;
    }

    case '-':
    {
        int result = intFirstOperand - intSecondOperand;
        printf("%d", result);
        saveANSInt(result);
        break;
    }

    case '*':
    {
        int result = intFirstOperand * intSecondOperand;
        printf("%d", result);
        saveANSInt(result);
        break;
    }

    case '%':
    {
        int result = intFirstOperand % intSecondOperand;
        printf("%d", result);
        saveANSInt(result);
        break;
    }

    case '/':
    {
        if(intSecondOperand == 0)
        {
            printf("MATH ERROR\n");
            return;
        }
        float result = round2point(intFirstOperand * 1.0 / intSecondOperand);
        printf("%.2f", result);
        saveANSFloat(result);
        break;
    }
    }
}

char *readOperation()
{
    char *line = malloc(100), *linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if (line == NULL)
        return NULL;

    for (;;)
    {
        c = fgetc(stdin);
        if (c == EOF)
            break;

        if (--len == 0)
        {
            len = lenmax;
            char *linen = realloc(linep, lenmax *= 2);

            if (linen == NULL)
            {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if ((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    removeCharStr(&linep, lenStr(linep) - 1);
    return linep;
}

boolean isOperator(const char *operator)
{
    if (compareStr(operator, "+") || compareStr(operator, "-") || compareStr(operator, "*") || compareStr(operator, "/") || compareStr(operator, "%"))
        return T;
    return F;
}

boolean checkSyntax(char *operation, char **elementOfOperation)
{
    /// @brief check Syntax and contain elements of operation into char* array
    char *firstOperand = separateFirstWord(&operation, ' ');
    
    if (!lenStr(operation))
    {
        printf("SYNTAX ERROR\n");
        free(firstOperand);
        return F;
    }
    char *operator= separateFirstWord(&operation, ' ');
    if (!lenStr(operation))
    {
        printf("SYNTAX ERROR\n");
        free(firstOperand);
        free(operator);
        return F;
    }
    char *secondOperand = separateFirstWord(&operation, ' ');

    if (compareStr(firstOperand, "ANS"))
    {
        free(firstOperand);
        firstOperand = readANSFile();
        if (!(!lenStr(operation) && isNumberStr(secondOperand) && isOperator(operator)))
        {
            printf("SYNTAX ERROR\n");
            free(firstOperand);
            free(secondOperand);
            free(operator);
            return F;
        }
    }
    else
    if (compareStr(secondOperand, "ANS"))
    {
        free(secondOperand);
        secondOperand = readANSFile();
        if (!(!lenStr(operation) && isNumberStr(firstOperand) && isOperator(operator)))
        {
            printf("SYNTAX ERROR\n");
            free(firstOperand);
            free(secondOperand);
            free(operator);
            return F;
        }
    }
    else
    if (!(!lenStr(operation) && isNumberStr(firstOperand) && isNumberStr(secondOperand) && isOperator(operator)))
    {
        printf("SYNTAX ERROR\n");
        free(firstOperand);
        free(secondOperand);
        free(operator);
        return F;
    }
    elementOfOperation[0] = firstOperand;
    elementOfOperation[1] = secondOperand;
    elementOfOperation[2] = operator;
    return T;
}

int main()
{
    while (T)
    {
        printf("\e[1;1H\e[2J");
        printf("<< ");
        char *operation = readOperation();

        if (compareStr(operation, "EXIT"))
            break;
        char **elementOfOperation = (char **)malloc(3 * sizeof(char *));
        if (checkSyntax(operation, elementOfOperation))
        {
            calculator(elementOfOperation[0], elementOfOperation[1], elementOfOperation[2]);
        }
        
        free(elementOfOperation);
        getchar();
    }
}