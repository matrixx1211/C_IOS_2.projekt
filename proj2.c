// Author:      Marek Bitomský
// Project:     IOS - 2. project
// Date:        02. 05. 2021
// School:      VUT FIT Brno

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "proj2.h"

unsigned int arg_test(unsigned int value, unsigned int min, unsigned int max)
{
    //if value is between min and max with edge value, return arg value
    if ((min <= value) && (value <= max))
        return value;
    //if not prints error and exits with return code 1
    fprintf(stderr, "PROJ2: Unexpected argument value passed.\n");
    exit(1);
}

void args_test(int argc, char const *argv[], args_t *p_pa)
{
    //args count test
    if (argc == 5)
    {
        //sets NE with value testing
        p_pa->number_elf = arg_test(atoi(argv[1]), NEMIN, NEMAX);
        //sets NR with value testing
        p_pa->number_reindeer = arg_test(atoi(argv[2]), NRMIN, NRMAX);
        //sets TE with value testing
        p_pa->time_elf = arg_test(atoi(argv[3]), TMIN, TMAX);
        //sets TR with value testing
        p_pa->time_reindeer = arg_test(atoi(argv[4]), TMIN, TMAX);
    }
    else
    {
        fprintf(stderr, "PROJ2: Unexpected error with passed arguments.\n");
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    //structure with passed argumets on cmd line
    args_t passed_args;

    //tests passed arguments
    args_test(argc, argv, &passed_args);

    return 0;
}
