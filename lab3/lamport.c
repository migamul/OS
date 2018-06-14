#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#define N 100

int *trazim;
int *broj;
int *stolovi;

int broj_dretvi = 0;
int broj_stolova = 0;
int brojac = 0;

#define MAX(a, b) (((a)<(b))?(b):(a))

bool usporedi( int i, int j )
{
    return ( broj[j] < broj[i] ) || ( broj[j] == broj[i] && j < i );
}

int odaberi_stol( int brojac )
{
   int i;

   int *slobodni_stolovi = malloc(sizeof(int)*brojac);
   int k = 0;

   for( i = 0; i < broj_stolova; ++i )
   {
       if( stolovi[i] == 0)
       {
            slobodni_stolovi[k++] = i;
       }
   }

   return slobodni_stolovi[rand() % brojac];
}

int ima_slobodnih()
{
    int stanje = 0;
    int i;

    for( i = 0; i < broj_stolova; ++i)
    {
        if( stolovi[i] == 0)
        {
            stanje = 1;
        }
    }

    return stanje;
}

void ispis_stanja()
{
    int i;

    printf("stanje: ");
    for( i = 0; i < broj_stolova; ++i)
    {
	if(stolovi[i] == 0) printf("- ");
        else printf("%d ", stolovi[i]);
    }
    printf("\n");
}

void udi_u_KO( int i )
{
    trazim[i] = 1;
    int j;

    broj[i] = broj[0];
    for( j = 1; j < broj_dretvi; ++j )
    {
	broj[i] = MAX( broj[i], broj[j] );
    }
    broj[i]++;
    trazim[i] = 0;

    for( j = 0; j < broj_dretvi; ++j)
    {
        while( trazim[j] != 0 );
        while( broj[j] != 0 && usporedi( i, j ) );
    }
}

void izadi_iz_KO( int i )
{
    broj[i] = 0;
}

void* dretva( void *x )
{
    int i = *( (int*)x );
    srand( (unsigned int) (i*time(NULL)));
    while( ima_slobodnih() )
    {
        int odabrani = odaberi_stol( brojac );
        printf("Dretva %d: odabirem stol %d, ", i+1, odabrani+1);
	ispis_stanja();
        //printf("\n");
	
        sleep(1);
        udi_u_KO(i);

        if( stolovi[odabrani] == 0)
        {
            printf("Dretva %d: rezerviram stol %d, ", i + 1, odabrani + 1);
            stolovi[odabrani] = i + 1;
	    --brojac;
	    ispis_stanja();
            //printf("\n");	    
        }
        else
        {
            printf("Dretva %d: neuspjela rezervacija stola %d, ", i + 1, odabrani + 1);
	    ispis_stanja();
            //printf("\n");    
        }
        izadi_iz_KO(i);	
	sleep(1);
    }
}

void izlaz( int sig )
{
    exit(0);
}

int main( int argc, char *argv[] )
{
    if( argc == 3)
    {
        broj_dretvi = atoi( argv[1] );
        broj_stolova = atoi( argv[2] );
	brojac = broj_stolova;
	int i;

	trazim = malloc(sizeof(int)*broj_dretvi);
	broj = malloc(sizeof(int)*broj_dretvi);
	stolovi = malloc(sizeof(int)*broj_stolova);

	for( i = 0; i < broj_dretvi; ++i )
	{
	    trazim[i] = 0;
	    broj[i] = 0;
	}
	for( i = 0; i < broj_dretvi; ++i )
	{
	    stolovi[i] = 0;
	}
    }
    else
        exit(1);

    sigset( SIGINT, izlaz );

    pthread_t thr_id[N];
    int id[N], i;

    for( i = 0; i < broj_dretvi; ++i)
    {
        id[i] = i;
        if( pthread_create( &thr_id[i], NULL, dretva, &id[i]) != 0 )
        {
            printf("Greska pri stvaranju dretve!\n");
            exit(1);
        }
    }

    for( i = 0; i < broj_dretvi; ++i)
    {
       pthread_join( thr_id[i], NULL );
    }
	
    printf("Svi stolovi rezervirani\n");

    return 0;
}
