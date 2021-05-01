// Autor:       Marek Bitomský
// Projekt:     IOS - 2. project
// Datum:       02. 05. 2021
// Škola:       VUT FIT Brno

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <semaphore.h> //veškeré funkce pro práci se semafory
#include <sys/types.h> //sytémové typy jako pid_t
#include <sys/wait.h>  //funkce wait()
#include <sys/mman.h>  //funkce mmap()
#include <unistd.h>    //funkce fork() pro vytvoření procesů
#include <time.h>      //funkce time()

#include "proj2.h"

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

        //nastavení počátečních hodnot
        shared->pocet_radku = 0;
        shared->pocet_ve_fronte = 0;
        shared->nacteno_elfu = shared->pocet_elfu;
        shared->nacteno_sobu = shared->pocet_sobu;
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

    init_sem(shared, &(shared->vanoce_sem), 1, 0);
    init_sem(shared, &(shared->dovolenka_sem), 1, 0);
    init_sem(shared, &(shared->zaprahnout_sem), 1, 0);

    init_sem(shared, &(shared->output_sem), 1, 1);
}

void destroy_all_sem(shared_t *shared)
{
    //semafory pro práci procesy Santy/elfů/sobů
    sem_destroy(&(shared->santa_sem));
    sem_destroy(&(shared->elf_sem));
    sem_destroy(&(shared->sob_sem));

    sem_destroy(&(shared->vanoce_sem));
    sem_destroy(&(shared->dovolenka_sem));
    sem_destroy(&(shared->zaprahnout_sem));

    //semafory pro práci s výstupem
    sem_destroy(&(shared->output_sem));
}

void print_text(shared_t *shared, int type, int ID, char *text)
{
    sem_wait(&shared->output_sem);

    if (type == SANTA)
        fprintf(shared->output_f, "%ld: Santa: %s\n", ++shared->pocet_radku, text);
    if (type == ELF)
        fprintf(shared->output_f, "%ld: Elf %d: %s\n", ++shared->pocet_radku, ID, text);
    if (type == SOB)
        fprintf(shared->output_f, "%ld: RD %d: %s\n", ++shared->pocet_radku, ID, text);
    fflush(shared->output_f);

    sem_post(&shared->output_sem);
}

void proces_Santa_Claus(shared_t *shared)
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
        print_text(shared, SANTA, 0, "going to sleep");
        sem_post(&shared->elf_sem);
        sem_post(&shared->sob_sem);
        while (true) //čeká na elfy/soby
        {
            sem_wait(&shared->santa_sem);
            if (shared->cinnost == 1) //v případě elfů
            {
                shared->nacteno_elfu--;
                print_text(shared, SANTA, 0, "helping elves");
                print_text(shared, SANTA, 0, "going to sleep");
                shared->cinnost = 0;
                sem_post(&shared->elf_sem);
                if (shared->nacteno_elfu >= 0)
                    sem_post(&shared->sob_sem);
            }
            if (shared->cinnost == 2) //v případě sobů
            {
                sem_post(&shared->vanoce_sem);

                sem_post(&shared->sob_sem);
                if (shared->nacteno_sobu >= 0)
                    sem_post(&shared->elf_sem);

                if (shared->nacteno_sobu == 0)
                {
                    print_text(shared, SANTA, 0, "closing workshop");
                    sem_post(&shared->zaprahnout_sem);
                    sem_post(&shared->dovolenka_sem);
                    break;
                }
                shared->cinnost = 0;
            }
        }
        sem_wait(&shared->vanoce_sem);
        print_text(shared, SANTA, 0, "Christmas started");
        exit(EXIT_SUCCESS);
    }
}

void proces_elf(shared_t *shared)
{
    for (int i = 1; i <= shared->pocet_elfu; i++)
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
            print_text(shared, ELF, i, "started");
            srand(time(NULL));
            usleep((rand() % (shared->cas_soba + 1)) * TIMECONVERT); //doba, po kterou elf pracuje sám
            print_text(shared, ELF, i, "need help");
            /* if (shared->fronta_elfu < 3)
            {
                p
            } */
            shared->cinnost = 1;
            sem_post(&shared->santa_sem);
            /* sem_wait(&shared->dovolenka_sem);
            print_text(shared, ELF, i, "taking holidays");
            sem_post(&shared->dovolenka_sem); */
            exit(EXIT_SUCCESS);
        }
    }
}

void proces_sob(shared_t *shared)
{
    for (int i = 1; i <= shared->pocet_sobu; i++)
    {
        pid_t sob = fork();
        if (sob == -1)
        {
            fprintf(stderr, "PROJ2: Nepodařilo se vytvořit soba.\n");
            destroy_all_sem(shared);
            exit(EXIT_FAILURE);
        }
        if (sob == 0)
        {
            sem_wait(&shared->sob_sem);
            print_text(shared, SOB, i, "rstarted");
            shared->nacteno_sobu--;
            srand(time(NULL));
            usleep((rand() % (shared->cas_elfa + 1 - TMAX / 2)) * TIMECONVERT); //doba, po kterou elf pracuje sám
            print_text(shared, SOB, i, "return home");
            shared->cinnost = 2;
            sem_post(&shared->santa_sem);
            /* sem_wait(&shared->vanoce_sem);
            print_text(shared, SOB, i, "get hitched");
            sem_post(&shared->vanoce_sem); */
            exit(EXIT_SUCCESS);
        }
    }
}

int main(int argc, char const *argv[])
{
    //vytvoření sdílené paměti
    shared_t *shared = mmap(NULL, sizeof(shared_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (shared == MAP_FAILED)
    {
        fprintf(stderr, "PROJ2: Nelze vytvořit sdílenou paměť.\n");
        exit(EXIT_SUCCESS);
    }
    shared->output_f = fopen("proj2.out", "w");

    //kontrola argumentů
    args_test(argc, argv, shared);

    //vytvoření semaforů
    init_all_sem(shared);

    //volání procesů
    proces_Santa_Claus(shared);
    proces_elf(shared);
    proces_sob(shared);

    //zničí všechny semafory
    destroy_all_sem(shared);

    fclose(shared->output_f);

    //uvolní sdílenou paměť
    if (munmap(shared, sizeof(shared_t)) == -1)
        fprintf(stderr, "PROJ2: Nepovedlo se vyčistit sdílenou paměť.\n");

    return 0;
}
