#include <stdio.h>
#include "framework.h"

int main(int argc, char *argv[])
{
    printf("Komandi:\n 'w' : ODI\n 'a' : SVRTI LEVO\n 'd' : SVRTI DESNO\n 'z' : ZEMI\n 'x' : OSTAVI\n");
    initEverything(argc, argv);
    return 0;
}
