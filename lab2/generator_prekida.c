#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int pid = 0;
int signali[]={ SIGUSR1, SIGUSR2, SIGQUIT, SIGTSTP, SIGINT };

void prekidna_rutina( int sig )
{
    // posalji SIGKILL procesu 'pid'
    kill(pid, SIGKILL);
    exit(0);
}

int main(int argc, char *argv[])
{
    if ( argc < 2 )
    {
        fprintf ( stderr, "Koristenje: %s PID\n", argv[0] );
        return 1;
    }

    pid = atoi( argv[1] );

    sigset( SIGINT, prekidna_rutina );
  
    int signal;

    while(1)
    {
        // spavaj 2-7 sekundi
        sleep( ( rand() % 6 ) + 2 );

        // slucajno odaberi jedan signal od njih 5
        signal = ( rand() % 5 );

        // posalji odabrani signal procesu 'pid' funkcijom kill
        kill( pid, signali[signal] );
    }
  
    return 0;
}
