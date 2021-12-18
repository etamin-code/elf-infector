#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(int ac, char ** av, char ** env)
{
    printf("> started.\n");

    while(1)
    {
        printf(".");
        fflush(stdout);
        sleep(1);
    }

    printf("> done.\n");
    return 0;
}
