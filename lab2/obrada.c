#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#define N 6    // broj razina prekida  

int OZNAKA_CEKANJA[N] = {0};
int PRIORITET[N] = {0};
int TEKUCI_PRIORITET = 0;

int sig[]={ SIGUSR1, SIGUSR2, SIGQUIT, SIGTSTP, SIGINT };

void zabrani_prekidanje()
{
    int i;
    for( i = 0; i < 5; i++ )
        sighold( sig[i] );
    
    return;
}

void dozvoli_prekidanje()
{
    int i;
    for( i = 0; i < 5; i++ )
        sigrelse( sig[i] );
    
    return;
}

void obrada_prekida( int i )
{
    int j;
    char ispis[] = "- - - - - -\n";
    ispis[ 2+(i-1)*2 ] = 'P';
    printf( "%s", ispis );
    for( j = 1; j <= 5; j++)
    {
        sleep(1);
        ispis[ 2+(i-1)*2 ] = j + '0';
        printf( "%s", ispis );
    }
    ispis[ 2+(i-1)*2 ] = 'K';
    printf( "%s", ispis );
}

void prekidna_rutina( int sig )
{
    int n = 1, x, j;

    zabrani_prekidanje();
  
    switch( sig )
    {
        case SIGUSR1:
            n = 1;
            printf( "- X - - - -\n" );
            break;
        case SIGUSR2:
            n = 2;
            printf( "- - X - - -\n" );
            break;
        case SIGQUIT:
            n = 3;
            printf( "- - - X - -\n" );
            break;
        case SIGTSTP:
            n = 4;
            printf( "- - - - X -\n" );
            break;
        case SIGINT:
            n = 5;
            printf( "- - - - - X\n" );
            break;
        default:
            break;
    }

    OZNAKA_CEKANJA[ n ]++;
  
    do
    {
        x = 0;
    
        for( j = TEKUCI_PRIORITET + 1; j < N; ++j )
        {
            if( OZNAKA_CEKANJA[ j ] > 0 )
                x = j;
        }
    
        if( x > 0 )
        {
            OZNAKA_CEKANJA[x]--;
            PRIORITET[x] = TEKUCI_PRIORITET;
            TEKUCI_PRIORITET = x;
            dozvoli_prekidanje();
            obrada_prekida( x );
            zabrani_prekidanje();
            TEKUCI_PRIORITET = PRIORITET[x];
        }
    
    }
    while( x > 0 );
  
    dozvoli_prekidanje();
  
    return;
}

int main ( int argc, char *argv[] )
{
    for( int i = 0; i < 5; ++i )
        sigset ( sig[i], prekidna_rutina );

    printf( "Proces obrade prekida, PID=%d\n", getpid() );
  
    printf( "G 1 2 3 4 5\n" );
    printf( "-----------\n" );
  
    int i = 0;
    while(1)
    {
        i++;
        sleep(1);
        printf( "%d - - - - -\n", i % 10 );
    }

    return 0;
}
