// Autor:       Marek Bitomský
// Projekt:     IOS - 2. project
// Datum:       02. 05. 2021
// Škola:       VUT FIT Brno

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <semaphore.h> //veškeré funkce pro práci se semafory
#include <unistd.h>    //funkce fork() pro vytvoření procesů
#include <sys/types.h> //sytémové typy jako pid_t
#include <sys/wait.h>  //funkce wait()
#include <time.h>      //funkce time()
#include <sys/mman.h>  //funkce mmap()

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
        shared->pocet_elfu = arg_test(atoi(argv[1]), NEMIN, NEMAX);

        //nastaví NR ve struktuře a otestuje hodnotu
        shared->pocet_sobu = arg_test(atoi(argv[2]), NRMIN, NRMAX);

        //nastaví TE ve struktuře a otestuje hodnotu
        shared->cas_elfa = arg_test(atoi(argv[3]), TMIN, TMAX);

        //nastaví TR ve struktuře a otestuje hodnotu
        shared->cas_soba = arg_test(atoi(argv[4]), TMIN, TMAX);
    }
    else
    {
        //vypíše error a ukončí se s kódem 1
        fprintf(stderr, "PROJ2: Očekávaný počet argumentů -> 4, zadáno -> %d.\n", argc - 1);
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
        destroy_all_sem(shared);
        exit(EXIT_FAILURE);
    }
}

void init_all_sem(shared_t *shared)
{
    //všechny mají hodnotu 1 -> tudiž jsou odemčené
    init_sem(shared, &(shared->santa_sem), 1, 1);
    init_sem(shared, &(shared->elf_sem), 1, 0);
    init_sem(shared, &(shared->sob_sem), 1, 0);
    init_sem(shared, &(shared->output_sem), 1, 1);
    init_sem(shared, &(shared->error_output_sem), 1, 0);
    init_sem(shared, &(shared->vanoce_sem), 1, 0);
}

void destroy_all_sem(shared_t *shared)
{
    //semafory pro práci procesy Santy/elfů/sobů
    sem_destroy(&(shared->santa_sem));
    sem_destroy(&(shared->elf_sem));
    sem_destroy(&(shared->sob_sem));
    sem_destroy(&(shared->vanoce_sem));
    //semafory pro práci s výstupem
    sem_destroy(&(shared->error_output_sem));
    sem_destroy(&(shared->output_sem));
}

void process_Santa_Clause(shared_t *shared)
{
    pid_t santa = fork();
    if (santa == -1)
    {
        fprintf(stderr, "PROJ2: Chyba při vytváření Santy.\n");
        destroy_all_sem(shared);
        exit(EXIT_FAILURE);
    }

    if (santa == 0)
    {
        sem_wait(&shared->santa_sem);
        sem_wait(&shared->output_sem);
        fprintf(shared->output_f, "%ld: Santa: going to sleep\n", ++shared->pocet_radku);
        fflush(shared->output_f);
        sem_post(&shared->output_sem);
        sem_post(&shared->elf_sem);
        sem_post(&shared->sob_sem);
        while (true) //čeká na elfy/soby
        {
            sem_wait(&shared->santa_sem);
            if (shared->cinnost == 1) //v případě elfů
            {
                //sem_wait(&shared->output_sem);
                fprintf(shared->output_f, "%ld: Santa: helping elves\n", ++shared->pocet_radku);
                fprintf(shared->output_f, "%ld: Santa: going to sleep\n", ++shared->pocet_radku);
                fflush(shared->output_f);
                //sem_post(&shared->output_sem);
                shared->cinnost = 0;
            }
            if (shared->cinnost == 2) //v případě sobů
            {
                if (shared->pocet_sobu == 0)
                {
                    //sem_wait(&shared->elf_sem);
                    //sem_wait(&shared->output_sem);
                    fprintf(shared->output_f, "%ld: Santa: closing workshop\n", ++shared->pocet_radku);
                    fprintf(shared->output_f, "%ld: Santa: Christmas started\n", ++shared->pocet_radku);
                    fflush(shared->output_f);
                    //sem_post(&shared->output_sem);
                    shared->cinnost = 1;
                    //sem_post(&shared->santa_sem);
                    //sem_post(&shared->elf_sem);
                    exit(EXIT_SUCCESS);
                }
                shared->pocet_sobu--;
                shared->cinnost = 0;
            }
        }
        exit(EXIT_SUCCESS);
    }
}

void process_Elf(shared_t *shared)
{
    for (int i = 0; i < shared->pocet_elfu; i++)
    {
        pid_t elf = fork();
        if (elf == -1)
        {
            fprintf(stderr, "PROJ2: Nepodařilo se vytvořit elfa.\n");
            destroy_all_sem(shared);
            exit(EXIT_FAILURE);
        }
        if (elf == 0)
        {
            sem_wait(&shared->elf_sem);
            sem_wait(&shared->output_sem);
            fprintf(shared->output_f, "%ld: Elf %d: started\n", ++shared->pocet_radku, i + 1);
            fflush(shared->output_f);
            sem_post(&shared->output_sem);
            usleep((rand() % shared->cas_elfa) * TIMECONVERT); //doba, po kterou elf pracuje sám
            shared->cinnost=1;
            sem_post(&shared->santa_sem);
            sem_post(&shared->elf_sem);
            exit(EXIT_SUCCESS);
        }
    }
}

int main(int argc, char const *argv[])
{
    //vytvoření sdílené paměti
    shared_t *shared = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    shared->output_f = fopen("proj2.out", "w");

    //kontrola argumentů
    args_test(argc, argv, shared);

    //vytvoření semaforů
    init_all_sem(shared);

    //volání procesů
    process_Santa_Clause(shared);
    process_Elf(shared);

    //zničí všechny semafory
    destroy_all_sem(shared);

    //uvolní sdílenou paměť
    if (munmap(shared, sizeof(shared)) == -1)
        fprintf(stderr, "PROJ2: Nepovedlo se vyčistit sdílenou paměť.\n");
    return 0;
}
