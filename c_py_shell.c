/*
Main file
*/

#include <stdio.h>

int main(void)
{
    printf("    C Python Shell 2025\n\n");
    printf("    Created by Fedya, Danya, Staryi\n");
    printf("    For help - help()\n");


    char buf[512];          // all user input goes here!!! always ends with \n\0
    while(1){
        printf(">>> ");
        fgets(buf, sizeof(buf), stdin);
        printf("%s", buf);
    }
    return 0;
}
