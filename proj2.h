// Author:      Marek Bitomský
// Project:     IOS - 2. project
// Date:        02. 05. 2021
// School:      VUT FIT Brno

#ifndef __IOS_PROJ2__
#define __IOS_PROJ2__

/* min and max number of elves 0<NE<1000 */
#define NEMIN 1
#define NEMAX 999

/* min and max number of reindeer 0<NR<20 */
#define NRMIN 1
#define NRMAX 19

/* min and max time for elves or reindeers */
#define TMIN 0
#define TMAX 1000

/* structure with all arguments which are passed */
typedef struct args
{
    int number_elf;
    int number_reindeer;
    int time_elf;
    int time_reindeer;
} args_t;

/* tests if arguments were passed correctly */
void args_test(int argc, char const *argv[], args_t *p_pa);

/* tests if every argument value is between min and max */
unsigned int arg_test(unsigned int value, unsigned int min, unsigned int max);

#endif // __IOS_PROJ2__