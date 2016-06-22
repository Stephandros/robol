#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "framework.h"

int main(int argc, char *argv[])
{
    //printf("Komandi:\n 'w' : ODI\n 'a' : SVRTI LEVO\n 'd' : SVRTI DESNO\n 'z' : ZEMI\n 'x' : OSTAVI\n");

    postavi_okolina();

    queue q;
    init_queue(&q);
    enqueue(&q, 1);
    enqueue(&q, 0);
    enqueue(&q, 0);
    enqueue(&q, 3);
    enqueue(&q, 2);
    enqueue(&q, 0);
    enqueue(&q, 0);
    enqueue(&q, 4);
    enqueue(&q, 2);
    enqueue(&q, 0);
    enqueue(&q, 0);
    ANIMATE(&q);

    initEverything(argc, argv);

    return 0;
}

void postavi_okolina()
{
    char okolina[]       = "Okolina";
    char zidovi[]        = "Zidovi";
    char zetoni[]        = "Zetoni";
    char robot[]         = "Robot";
    char nasoka_robot[]  = "Nasoka Robot";
    char kraj[]          = "kraj";

    FILE *fp;
    if((fp = fopen("okolina.env", "r")) == NULL)
    {
        printf("Can't open file!\n");
        return;
    }

    char line[100];

    while(fgets(line, sizeof(line), fp))
    {
        if(strncmp(line, okolina, 7) == 0)
        {
            fgets(line, sizeof(line), fp);
            char * split;
            split = strtok(line, " ");
            int i = atoi(split);
            split = strtok(NULL, " ");
            int j = atoi(split);
            printf("Grid %d %d\n", i, j);
            set_grid(i);
        }
        else if(strncmp(line, zidovi, 6) == 0)
        {
            Barrier_Data barrier_positions[100];
            fgets(line, sizeof(line), fp);
            fgets(line, sizeof(line), fp);
            int count = 0;
            do
            {
                char * split;
                split = strtok(line, " ");
                int mode = atoi(split);
                split = strtok(NULL, " ");
                int index = atoi(split);
                split = strtok(NULL, " ");
                int start = atoi(split);
                split = strtok(NULL, " ");
                int end = atoi(split);

                barrier_positions[count].mode  = mode;
                barrier_positions[count].index = index;
                barrier_positions[count].start = start;
                barrier_positions[count++].end = end;

                printf("Zid %d %d %d %d\n", mode, index, start, end);
                fgets(line, sizeof(line), fp);
            } while(strncmp(line, kraj, 4) != 0);
            set_barrier_positions(barrier_positions, count);
        }
        else if(strncmp(line, zetoni, 6) == 0)
        {
            Indices coin_positions[100];
            fgets(line, sizeof(line), fp);
            fgets(line, sizeof(line), fp);
            int count = 0;
            do
            {
                char * split;
                split = strtok(line, " ");
                int i = atoi(split);
                split = strtok(NULL, " ");
                int j = atoi(split);

                coin_positions[count].i = i;
                coin_positions[count++].j = j;

                printf("Zeton %d %d\n", i, j);
                fgets(line, sizeof(line), fp);
            } while(strncmp(line, kraj, 4) != 0);
            set_coin_positions(coin_positions, count);
        }
        else if(strncmp(line, robot, 5) == 0)
        {
            fgets(line, sizeof(line), fp);
            char * split;
            split = strtok(line, " ");
            int i = atoi(split);
            split = strtok(NULL, " ");
            int j = atoi(split);
            printf("Robot %d %d\n", i, j);
            set_robot_position(i, j);
        }
        else if(strncmp(line, nasoka_robot, 12) == 0)
        {
            fgets(line, sizeof(line), fp);
            int nasoka = atoi(line);
            set_robot_direction(nasoka);
        }
    }

    fclose(fp);
}
