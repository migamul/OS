#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>

#define MAX 10

sem_t stolice_isklj;
sem_t klijent_sem;
sem_t frizerka_sem;
sem_t cekaona;

int broj_stolica_s;
int broj_stolica_z;
int gotovo = 0;

void *frizerka( void *nesto )
{
    int vrijeme_friziranja;
    sem_wait(&frizerka_sem);

    while( !gotovo )
    {
        vrijeme_friziranja = ( rand() % 8 ) + 2;
        sleep(vrijeme_friziranja);
        sem_post(&klijent_sem);
    }
}

void *klijent( void *x )
{
    int i = *(int*)x;
    srand( (unsigned int) (i*time(NULL)));

    printf("Klijent(%d): Zelim na frizuru\n", i);

    if( sem_trywait(&cekaona) == -1 )
    {
        printf("Klijent(%d): Nema mjesta u salonu (%d), probat cu sutra\n", i, broj_stolica_z);
        return 0;
    }
    else
    {
        broj_stolica_s--;
        broj_stolica_z++;
        printf("Klijent(%d): Ulazim u cekaonu (%d)\n", i, broj_stolica_z);
        sem_wait(&stolice_isklj);
	sem_post(&cekaona);

	if( (broj_stolica_z - 1) == 0 && sem_trywait(&frizerka_sem) == -1 )
        {
            printf("Klijent(%d): Budim frizerku\n", i);
        }
        broj_stolica_s++;
        broj_stolica_z--;

        sem_post(&frizerka_sem);
        printf("Klijent(%d): Frizerka mi radi frizuru\n", i);
	sem_wait(&klijent_sem);
        sem_post(&stolice_isklj);
        printf("Klijent(%d): Frizura gotova - odlazim\n", i);
    }
}

int main( int argc, char *argv[] )
{
    pthread_t frizerka_id;
    pthread_t klijent_id[MAX];
    int broj[MAX];

    if( argc != 2)
    {
        printf("Morate unijeti argument koji odgovara broju stolica!\n");
        exit(-1);
    }

    broj_stolica_s = atoi( argv[1] );
    broj_stolica_z = 0;
    int i;

    for( i = 0; i < MAX; i++ )
	broj[i] = i;

    sem_init(&stolice_isklj, 0, 1);
    sem_init(&klijent_sem, 0, 0);
    sem_init(&frizerka_sem, 0, 0);
    sem_init(&cekaona, 0, broj_stolica_s);

    if(pthread_create(&frizerka_id, NULL, frizerka, NULL) != 0)
        exit(-1);

    for( i = 0; i < MAX; ++i)
    {
        broj[i] = i;
        if(pthread_create(&klijent_id[i], NULL, klijent, (void *)&broj[i]) != 0)
            exit(1);
        sleep( (rand() % 8) + 2 );
    }

    for( i = 0; i < MAX; i++ )
        pthread_join(klijent_id[i], NULL);


    gotovo = 1;
    sem_post(&frizerka_sem);
    pthread_join(frizerka_id, NULL);
    return 0;
}

