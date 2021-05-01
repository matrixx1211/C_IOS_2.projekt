// Autor:       Marek Bitomský
// Projekt:     IOS - 2. project
// Datum:       02. 05. 2021
// Škola:       VUT FIT Brno

#ifndef __IOS_PROJ2__
#define __IOS_PROJ2__

/* min a max počet elfů 0<NE<1000 */
#define NEMIN 1
#define NEMAX 999

/* min a max počet sobů 0<NR<20 */
#define NRMIN 1
#define NRMAX 19

/* min a max čas pro elfa nebo soba */
#define TMIN 0
#define TMAX 1000

/* hodnota konvertující čas z mikrosekund */
#define TIMECONVERT 1000 //z us na ms

/* sdílená struktura pro procesy se všema hodnotama */
typedef struct
{
    //argumenty
    int pocet_elfu;
    int pocet_sobu;
    int cas_elfa;
    int cas_soba;

    //proces semafory
    sem_t santa_sem;
    sem_t elf_sem;
    sem_t sob_sem;
    sem_t vanoce_sem;

    //output semafory
    sem_t error_output_sem;
    sem_t output_sem;

    //ostatní
    size_t pocet_radku; //počítadlo řádků
    pid_t *fronta_elfu; //fronta, ve které jsou elfové
    FILE *output_f;     //soubor pro výstup
} shared_t;

/* Testuje, jestli argumenty byly zadány správně */
void args_test(int argc, char const *argv[], shared_t *shared);

/* Testuje, jestli argument je z intervalu <min, max> */
unsigned int arg_test(unsigned int value, unsigned int min, unsigned int max);

/* Inicializuje semafor a zkontroluje jestli se vše povedlo */
void init_sem(shared_t *shared, sem_t *sem_to_init, int pshared, unsigned int value);

/* Inicializuje všechny semafory */
void init_all_sem(shared_t *shared);

/* Proces Santa Claus */
int process_Santa_Clause(shared_t *shared);

/* Proces sob  */
int process_Reindeer(shared_t *shared);

/* Proces ell */
int process_Elf(shared_t *shared);

/* Zničí všechny semafory */
void destroy_all_sem(shared_t *shared);

#endif // __IOS_PROJ2__