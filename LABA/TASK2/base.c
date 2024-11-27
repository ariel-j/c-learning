#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *map(char *array, int array_length, char (*f)(char))
{
    char *mapped_array = (char *)(malloc(array_length * sizeof(char)));
    if (mapped_array == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < array_length; i++)
    {
        mapped_array[i] = f(array[i]);
    }
    return mapped_array;
}

/* 1. my_get: Ignores input char c, reads and returns a character from stdin using fgetc */
char my_get(char c)
{
    return fgetc(stdin);
}

/* 2. cprt: If c is a number between 0x20 and 0x7E, 
cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('.') 
character. After printing, cprt returns the value of c unchanged. */
char cprt(char c)
{
    if (c >= 0x20 && c <= 0x7E)
    { 
        printf("%c\n", c);
    }
    else
    {
        printf(".\n");
    }
    return c;
}

/* 3. encrypt: Gets a char c and returns its encrypted form by adding 1 to its value. 
If c is not between 0x1F and 0x7E it is returned unchanged */
char encrypt(char c)
{
    if (c >= 0x1F && c <= 0x7E)
    {
        return c + 1;
    }
    return c;
}

/* 4. decrypt: Gets a char c and returns its decrypted form by reducing 1 from its value. 
If c is not between 0x21 and 0x7F it is returned unchanged  */
char decrypt(char c)
{
    if (c >= 0x21 && c <= 0x7F)
    {
        return c - 1;
    }
    return c;
}

/* 5. xprt: xprt prints the value of c in a hexadecimal representation followed by a new line, and returns c unchanged */
char xprt(char c)
{
    printf("%02X\n", (unsigned char)c);
    return c;
}

/* 6. dprt: dprt prints the value of c in a decimal representation followed by a new line, and returns c unchanged */
char dprt(char c)
{
    printf("%d\n", (unsigned char)c);
    return c;
}

int main(int argc, char **argv)
{
    /* TODO: Test your code */
    char arr1[] = {'H','E','Y','!'};
char* arr2 = map(arr1, 4, xprt);
printf("%s\n", arr2);

// task 2.2 

     int base_len = 5;   
    char arr[base_len]; 
    printf("Enter %d characters:\n", base_len);
    char *arr6 = map(arr, base_len, my_get);
    printf("Decimal representation of input:\n");
    char *arr3 = map(arr6, base_len, dprt);
    printf("Hexadecimal representation of input:\n");
    char *arr4 = map(arr3, base_len, xprt);
    printf("Encrypt characters (with out printing)\n");
    char *arr5 = map(arr4, base_len, encrypt);
    printf("Encrypted characters as ASCII (if printable (using 2)):\n");
    map(arr5, base_len, cprt);


    free(arr2);
    free(arr6);
    free(arr3);
    free(arr4);
    free(arr5);

    return 0;
}
