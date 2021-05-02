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

/* definuje typ, jestli se jedná o Santu/elfa/soba */
#define SANTA 1
#define ELF 2
#define SOB 3

/* počet elfů, kteří čekají ve frontě */
#define FRONTA 3

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

    //semafory pro začátek Vánoc/dovolené elfů
    sem_t vanoce_sem;
    sem_t dovolenka_sem;
    sem_t zaprahnout_sem;

    //output semafor
    sem_t output_sem;

    //ostatní
    size_t pocet_radku;  //počítadlo řádků
    FILE *output_f;      //soubor pro výstup
    int cinnost;         //cinnost Santy
    int *elf_ID;          //fronta, ve které jsou elfové
    int pocet_ve_fronte; //počet elfů ve frontě
    int nacteno_elfu;    //počet načtených elfů
    int nacteno_sobu;    //počet načtených sobů
} shared_t;

/* Testuje, jestli argumenty byly zadány správně */
void args_test(int argc, char const *argv[], shared_t *shared);

/* Testuje, jestli argument je z intervalu <min, max> */
unsigned int arg_test(unsigned int value, unsigned int min, unsigned int max);

/* Inicializuje semafor a zkontroluje jestli se vše povedlo */
void init_sem(shared_t *shared, sem_t *sem_to_init, int pshared, unsigned int value);

/* Inicializuje všechny semafory */
void init_all_sem(shared_t *shared);

/* Vypíše text podle typu vybere Santa/elf/sob a když má ID tak i ID */
void print_text(shared_t *shared, int type, int ID, char *text);

/* Proces Santa Claus */
void proces_Santa_Claus(shared_t *shared);

/* Proces sob  */
void proces_sob(shared_t *shared);

/* Proces ell */
void proces_elf(shared_t *shared);

/* Zničí všechny semafory */
void destroy_all_sem(shared_t *shared);

#endif // __IOS_PROJ2__