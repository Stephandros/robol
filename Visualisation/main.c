#include <stdio.h>
#include "framework.h"

int main(int argc, char *argv[])
{
    printf("Komandi:\n 'w' : ODI\n 'a' : SVRTI LEVO\n 'd' : SVRTI DESNO\n 'z' : ZEMI\n 'x' : OSTAVI\n");

    queue q;
    init_queue(&q);
    enqueue(&q, 0);
    enqueue(&q, 0);
    enqueue(&q, 0);
    enqueue(&q, 0);
    enqueue(&q, 3);
    enqueue(&q, 2);
    enqueue(&q, 0);
    enqueue(&q, 0);
    enqueue(&q, 4);
    enqueue(&q, 0);
    enqueue(&q, 0);
    ANIMATE(&q);

    initEverything(argc, argv);

    return 0;
}
