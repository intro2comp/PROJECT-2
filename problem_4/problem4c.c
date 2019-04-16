#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

int sigArray[64] = {0};

void timeoutHandler()
{

    printf("Child process has been signaled\n");
    printf("Results will be corrupted\n");
    printf("Exiting program\n");

    signal(SIGINT, SIG_DFL);

    kill(getpid(), SIGINT);

}

void handler(int signo)
{
    if(signo == SIGINT)
    {
	if(sigArray[signo] == 1)
	{
	    printf("\nSIGINT blocked\n");
	}
	else
	{
	    printf("\nCaught SIGINT, interrupting process %d\n", getpid());

	    signal(SIGINT, SIG_DFL);
	    kill(getpid(), SIGINT);
	}
    }

    if(signo == SIGQUIT)
    {
	if(sigArray[signo] == 1)
	{
	    printf("\nSIGQUIT blocked\n");
	}
	else
	{
	    printf("\nCaught SIGQUIT, quitting process %d\n", getpid());
	    signal(SIGQUIT, SIG_DFL);
	    kill(getpid(), SIGQUIT);
	}
    }

    if(signo == SIGTSTP)
    {
	if(sigArray[signo] == 1)
	{
	    printf("\nSIGTSTP blocked\n");
	}
	else
	{
	    printf("\nCaught SIGTSTP, stopping %d\n", getpid());
	    signal(SIGTSTP, SIG_DFL);
	    kill(getpid(), SIGTSTP);
	}
    }

    if(signo == SIGABRT)
    {
	if(sigArray[signo] == 1)
	{
	    printf("\nSIGABRT blocked\n");
	}
	else
	{
	    printf("\nCaught SIGABRT, aborting process %d\n", getpid());
	    signal(SIGABRT, SIG_DFL);
	    kill(getpid(), SIGABRT);
	}
    }

    if(signo == SIGTERM)
    {
	if(sigArray[signo] == 1)
	{
	    printf("\nSIGTERM blocked\n");
	}
	else
	{
	    printf("\nCaught SIGTERM, terminated %d\n", getpid());
	    signal(SIGTERM, SIG_DFL);
	    kill(getpid(), SIGTERM);
	}
    }
    if(signo == SIGUSR1)
    {
	if(sigArray[signo] == 1)
	{
	    printf("\nSIGUSR1 blocked\n");
	}
	else
	{
	    printf("\nCaught SIGUSR1, exiting %d\n", getpid());
	    exit(0);
	}
    }
}

int main()
{
    int sigBlock;
    printf("What signal to be blocked? \n");
    scanf("%d", &sigBlock);
    sigArray[sigBlock] = 1;

    if(signal(SIGINT, handler) == SIG_ERR)
    {
	printf("\nERROR: SIGINT\n");
    }
    if(signal(SIGQUIT, handler) == SIG_ERR)
    {
	printf("\nERROR: SIGQUIT\n");
    }
    if(signal(SIGTSTP, handler) == SIG_ERR)
    {
	printf("\nERROR: SIGTSTP\n");
    }
    if(signal(SIGABRT, handler) == SIG_ERR)
    {
	printf("\nERROR: SIGABRT\n");
    }
    if(signal(SIGTERM, handler) == SIG_ERR)
    {
	printf("\nERROR: SIGTERM\n");
    }
    if(signal(SIGUSR1, handler) == SIG_ERR)
    {
	printf("\nERROR: SIGUSR1\n");
    }

    signal(SIGALRM, &timeoutHandler);

    clock_t begin = clock();
    printf("I am the parent process %d.\n",getpid());
    sleep(10);

    int fd1[2]; int fd2[2]; int fd3[2];
    int fd4[2]; int fd5[2]; int fd6[2];
    int fd7[2]; int fd8[2]; int fd9[2];
    int fd10[2]; int fd11[2]; int fd12[2];

    pipe(fd1); pipe(fd2); pipe(fd3);
    pipe(fd4); pipe(fd5); pipe(fd6);
    pipe(fd7); pipe(fd8); pipe(fd9);
    pipe(fd10); pipe(fd11); pipe(fd12);

    int count = 0;

    FILE *fp;
    fp = fopen("test.txt", "r");

    if (fp == NULL)
    {
        printf("something is incorrect: %s", strerror(errno));
        exit(1);
    }

    int place = 0;
    while(!feof(fp))
    {
        place++;
        fscanf(fp, "%d", &place);
        count++;
    }
    count--;

    int numList[count];

    fseek(fp, 0, SEEK_END);
    unsigned long length = (unsigned long)ftell(fp);
    if (length > 0)
    {
        rewind(fp);
    }
    else
    {
        printf("Empty file\n");
        return 0;
    }
    int i;
    for(i = 0 ; i < count; i++)
    {
        fscanf(fp, "%d", &numList[i]);
    }

    fclose(fp);

    FILE *file2=fopen("result_c.txt","w");



    int rem = count % 4;
    int partP = count / 4;
    int pt1 = 0;
    int pt2 = partP;
    int pt3 = partP * 2;
    int pt4 = partP * 3;
    int pt5 = (partP * 4) + rem - 1;

    pid_t pid = fork();
