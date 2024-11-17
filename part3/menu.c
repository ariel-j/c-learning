#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "base.h"

#define MAX_ARRAY_LEN 5

char my_get(char c);
char cprt(char c);
char encrypt(char c);
char decrypt(char c);
char xprt(char c);
char dprt(char c);

struct fun_desc
{
    char *name;
    char (*fun)(char);
};

struct fun_desc menu[] = {
    {"Get String", my_get},
    {"Print Decimal (dprt)", dprt},
    {"Print Hex (xprt)", xprt},
    {"Print Character (cprt)", cprt},
    {"Encrypt", encrypt},
    {"Decrypt", decrypt},
    {NULL, NULL}};

char *map(char *array, int array_length, char (*f)(char));

int main()
{
    char *carray = (char *)malloc(MAX_ARRAY_LEN * sizeof(char));
    memset(carray, 0, MAX_ARRAY_LEN); // checked the web for a solution to a safe initiating of the array

    int menu_size = sizeof(menu) / sizeof(menu[0]) - 1;

    while (1)
    {
        printf("Select a function by:\n");
        for (int i = 0; i < menu_size; i++)
        {
            printf("%d) %s\n", i + 1, menu[i].name);
        }

        printf("Enter the number of your  choice: ");
        int choice;
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > menu_size)
        {
            printf("Not within bounds, exiting...\n");
            break;
        }

        if (choice >= 1 && choice <= menu_size)
        {
            printf("Within bounds\n");
            carray = map(carray, MAX_ARRAY_LEN, menu[choice - 1].fun);
        }
        else
        {
            printf("Not within bounds\n");
            break;
        }

        char dummy;
        printf("Press Enter to continue, or Ctrl+D to exit.\n");
        while ((dummy = getchar()) != '\n' && dummy != EOF); 
    }

    free(carray); 
    return 0;
}

char *map(char *array, int array_length, char (*f)(char))
{
    char *mapped_array = (char *)malloc(array_length * sizeof(char));
    for (int i = 0; i < array_length; i++)
    {
        mapped_array[i] = f(array[i]);
    }
    return mapped_array;
}

char my_get(char c) {
    return fgetc(stdin);
}

char cprt(char c) {
    if (c >= 0x20 && c <= 0x7E) {
        putchar(c);
    } else {
        putchar('.');
    }
    putchar('\n');
    return c;
}

char encrypt(char c) {
    if (c >= 0x21 && c <= 0x7F) {
        return c + 1;
    }
    return c;
}

char decrypt(char c) {
    if (c >= 0x21 && c <= 0x7F) {
        return c - 1;
    }
    return c;
}

char xprt(char c) {
    printf("%02x\n", c);
    return c;
}

char dprt(char c) {
    printf("%d\n", c);
    return c;
}
