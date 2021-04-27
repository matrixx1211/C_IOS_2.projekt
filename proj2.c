// Autor:       Marek Bitomský
// Projekt:     IOS - 2. project
// Datum:       02. 05. 2021
// Škola:       VUT FIT Brno

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <semaphore.h>
#include <unistd.h>

//I'm not sure
#include <sys/types.h>
#include <signal.h>

#include "proj2.h"
/*#include "santa.h"
#include "elf.h"
#include "reindeer.h"*/

unsigned int arg_test(unsigned int value, unsigned int min, unsigned int max)
{
    //pokud je hodnota v rozsahu s koncovými body, tak se vrátí hodnota
    if ((min <= value) && (value <= max))
        return value;

    //pokud byla zadaná hodnota mimo rozsah
    fprintf(stderr, "PROJ2: Hodnota argumenta není z intervalu <%d, %d>.\n", min, max);

    //ukončí program s chybovým kódem 1
    exit(EXIT_FAILURE);
}

void args_test(int argc, char const *argv[], shared_t *shared)
{
    //první test kolik bylo zadáno argumetů
    if (argc == 5)
    {
        //nastaví NE ve struktuře a otestuje hodnotu
        shared->number_elf = arg_test(atoi(argv[1]), NEMIN, NEMAX);

        //nastaví NR ve struktuře a otestuje hodnotu
        shared->number_reindeer = arg_test(atoi(argv[2]), NRMIN, NRMAX);

        //nastaví TE ve struktuře a otestuje hodnotu
        shared->time_elf = arg_test(atoi(argv[3]), TMIN, TMAX);

        //nastaví TR ve struktuře a otestuje hodnotu
        shared->time_reindeer = arg_test(atoi(argv[4]), TMIN, TMAX);
    }
    else
    {
        //vypíše error a ukončí se s kódem 1
        fprintf(stderr, "PROJ2: Očekávaný počet argumentů -> %d, zadáno -> %d.\n", 5, argc);
        exit(EXIT_FAILURE);
    }
}

void init_sem(shared_t *shared, sem_t *sem_to_init, int pshared, unsigned int value)
{
    //inicializace
    if (sem_init(sem_to_init, pshared, value) == -1)
    {
        fprintf(stderr, "PROJ2: Nepovedlo se inicializovat semafor.\n");
        //TODO: uvolnění zdrojů //! možná tu bude ještě něco navíc
        destroy_sem(shared);
        exit(EXIT_FAILURE);
    }
}

void init_all_sem(shared_t *shared)
{
    init_sem(shared, &(shared->santa_sem), 1, 0);
    init_sem(shared, &(shared->elf_sem), 1, 1);
    init_sem(shared, &(shared->reindeer_sem), 1, 1);
    init_sem(shared, &(shared->output_sem), 1, 1);
}

void process_Santa_Clause(shared_t *shared)
{
    //deklarace procesu pro Santu
    pid_t santa;

    //vytvoření procesu Santy
    santa = fork();

    //pokud se nepovede vytvořit proces,
    //tak vypíše chybu, uvolní zdroje a ukončí se s kódem 1
    if (santa == -1)
    {
        fprintf(stderr, "PROJ2: Neočekávaná chyba při vytváření procesu.\n");
        //TODO: uvolnění zdrojů //! možná tu bude ještě něco navíc
        destroy_sem(shared);
        exit(EXIT_FAILURE);
    }
}

void process_Reindeer(shared_t *shared)
{
    //deklarace procesu pro soba
    pid_t reindeer;

    //vytvoření n procesů soba podle zadaných argumentů
    for (int i = 0; i < shared->number_reindeer; i++)
    {
        //vytvoří proces a vrátí PID, parent vrátí child PID a child vrátí 0 v případě, že se povede, jinak parent vrátí -1
        reindeer = fork();

        //pokud se nepovede vytvořit proces,
        //tak vypíše chybu, uvolní zdroje a ukončí se s kódem 1
        if (reindeer == -1)
        {
            fprintf(stderr, "PROJ2: Neočekávaná chyba při vytváření procesu.\n");
            //TODO: uvolnění zdrojů //! možná tu bude ještě něco navíc
            destroy_sem(shared);
            exit(EXIT_FAILURE);
        }

        //!DEBUG
        if (reindeer != 0)
            printf("PROCESS ID: %d, reindeer byl vytvořen.\n", reindeer);
    }
}

void process_Elf(shared_t *shared)
{
    //deklarace procesu pro elfa
    pid_t elf;

    //vytvoření n procesů elfa podle zadaných argumentů
    for (int i = 0; i < shared->number_elf; i++)
    {
        //vytvoří proces a vrátí PID, parent vrátí child PID a child vrátí 0 v případě, že se povede, jinak parent vrátí -1
        elf = fork();

        //pokud se nepovede vytvořit proces,
        //tak vypíše chybu, uvolní zdroje a ukončí se s kódem 1
        if (elf == -1)
        {
            fprintf(stderr, "PROJ2: Neočekávaná chyba při vytváření procesu.\n");
            //TODO: uvolnění zdrojů //! možná tu bude ještě něco navíc
            destroy_sem(shared);
            exit(EXIT_FAILURE);
        }

        //!DEBUG
        if (elf != 0)
            printf("PROCESS ID: %d, elf byl vytvořen.\n", elf);
    }
}

void destroy_sem(shared_t *shared)
{
    //zničí semafor output_sem
    sem_destroy(&(shared->output_sem));

    //zničí semafor santa_sem
    sem_destroy(&(shared->santa_sem));

    //zničí semafor elf_sem
    sem_destroy(&(shared->elf_sem));

    //zničí semafor reindeer_sem
    sem_destroy(&(shared->reindeer_sem));
}

int main(int argc, char const *argv[])
{
    //structure with passed argumets on cmd line
    shared_t shared;

    //testuje jestli předané parametry byli zadány správně
    args_test(argc, argv, &shared);

    //inicializuje semafory
    init_all_sem(&shared);

    //deklarace souboru pro výstup
    FILE *output_f;

    //otevření souboru
    output_f = fopen("proj2.out", "wx");
    (void)output_f;

    //vytvoří proces Santa Claus
    process_Santa_Clause(&shared);

    //vytvoří proces sob
    process_Reindeer(&shared);

    //vytvoří proces elf
    process_Elf(&shared);

    //zničí všechny semafory
    destroy_sem(&shared);

    return 0;
}
