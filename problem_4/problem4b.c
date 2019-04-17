#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>


struct scNode{
	struct scNode* nextChild;
	int slowChildPID;
};


int rsum1, rsum2, comp, rMin, rMax, sig, sig2, pid2, opid;

struct scNode* slowChildren;
int numchildren;


void sig_handler(int signum, siginfo_t* info, void* junk){
	int rVal;
	switch(signum){
		case SIGUSR1:
			rVal = info->si_value.sival_int;
			if(rMin > rVal){
				rMin = rVal;
			}
			break;
		case SIGUSR2:
			rVal = info->si_value.sival_int;
			if(rMax < rVal){
				rMax = rVal;
			}
			break;
		case SIGPIPE:
			rVal = info->si_value.sival_int;
			if(comp == 1){
				rsum1 = rVal;
				comp = 2;
				if(pid2){
					kill(pid2, SIGBUS);
				} else{
					sig2 = 1;
				}
			} else if(comp == 2){
				rsum2 = rVal;
			}
			break;
		case SIGBUS:
			sig = 1;
			break;
		case SIGALRM:
			kill(opid, SIGSEGV);
			union sigval retInfo = {0};
			sigqueue(getppid(), SIGPIPE, retInfo);
			kill(0,SIGKILL);
			exit(-1);
			break;
		case SIGSEGV:
			numchildren++;
			struct scNode* newNode = malloc(sizeof(struct scNode));
			newNode->slowChildPID = info->si_pid;
			if(slowChildren == NULL){
				newNode->nextChild = NULL;
				slowChildren = newNode;
			}else{
				newNode->nextChild = slowChildren;
				slowChildren = newNode;
			}
			break;
	}
}



int main(int argc, char* argv[]){


	if(argc != 2){
		printf("Program usage: <FileToBeRead>\n");
		exit(-1);
	}

	int inputFile = open(argv[1], O_RDONLY);
	if(inputFile < 0){
		printf("Error opening file '%s'\n", argv[1]);
		exit(-1);
	}


	rMin = 32767;
	rMax = -1;
	sig = 1;
	sig2 = 0;
	pid2 = 0;
	slowChildren = NULL;
	numchildren = 0;


	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);
	sigaddset(&mask, SIGPIPE);
	sigaddset(&mask, SIGBUS);
	sigaddset(&mask, SIGALRM);
	sigaddset(&mask, SIGSEGV);


	sigset_t maskSIGBUS;
	sigset_t oldmask;
	sigemptyset(&maskSIGBUS);
	sigaddset(&maskSIGBUS,SIGBUS);

	struct sigaction sigSetupArgs;
	memset(&sigSetupArgs, 0, sizeof(struct sigaction));
	sigSetupArgs.sa_flags = SA_SIGINFO|SA_RESTART;
	sigSetupArgs.sa_sigaction = sig_handler;
	sigSetupArgs.sa_mask = mask;

	sigaction(SIGUSR1, &sigSetupArgs, NULL);
	sigaction(SIGUSR2, &sigSetupArgs, NULL);
	sigaction(SIGPIPE, &sigSetupArgs, NULL);
	sigaction(SIGBUS, &sigSetupArgs, NULL);
	sigaction(SIGALRM, &sigSetupArgs, NULL);
	sigaction(SIGSEGV, &sigSetupArgs, NULL);

	int* narray = (int*)malloc(sizeof(int)*10);
	int readbIndex = 0;
	char inchar = 0;
	char readb[6] = {0,0,0,0,0,0};
  int arraySize = 10;
	int arrayIndex = 0;
	int eofc;
	int inputi;


	while((eofc = read(inputFile, &inchar, 1))!=0){
		switch(inchar){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':

				if(readbIndex > 4){
					printf("Error: file contains a non integer.\n");
					exit(-1);
				}
				readb[readbIndex] = inchar;
				readbIndex++;
				break;
			case '\n':
				inputi = atoi(readb);
				if(arrayIndex >= arraySize){
					arraySize *= 10;
					narray = realloc(narray, sizeof(int)*arraySize);
				}
				narray[arrayIndex] = inputi;
				arrayIndex++;
				memset(readb, 0, 6);
				readbIndex = 0;
				break;
			default:
				printf("Error: file has a not integer type\n");
				exit(-1);
		}
	}
	close(inputFile);
	arrayIndex--;
	narray = realloc(narray, sizeof(int)*(arrayIndex+1));



	FILE* output = fopen("Prob4PtB_Output.txt", "w");
	fprintf(output, "Hi I'm process %d and my parent is %d\n", getpid(), getppid());
	fflush(output);


	double numLevels = ceil(log((double)arraySize)/log(2));
	int currentl = numLevels;


	opid = getpid();
	int left = 0;
	int right = arrayIndex;

	while(right-left > 1){
		comp = 1;
		rsum1 = 0;
		rsum2 = 0;

		currentl--;
		sigprocmask(SIG_BLOCK, &maskSIGBUS, &oldmask);
		int pid1 = fork();
		if(pid1 == 0){
			sigprocmask(SIG_UNBLOCK, &maskSIGBUS, &oldmask);
			alarm(3+6*currentl);
			sig = 1;
			right = (right+left)/2;
			continue;
		}


		int old_sig = sig;
		sig = 0;

		pid2 = fork();
		if(pid2 == 0){
			left = (right+left)/2 + 1;
			alarm(3+6*currentl);
			sigprocmask(SIG_UNBLOCK, &maskSIGBUS, &oldmask);
			continue;
		} else{
			sig = old_sig;
		}
		sigprocmask(SIG_UNBLOCK, &maskSIGBUS, &oldmask);
		if(getpid() != opid){

			wait(pid1);
			kill(pid2, SIGBUS);
			wait(pid2);

			sigprocmask(SIG_BLOCK, &maskSIGBUS, &oldmask);
			while(sig == 0){
				sigsuspend(&oldmask);
			}
			sigprocmask(SIG_UNBLOCK, &maskSIGBUS, NULL);


			union sigval retInfo = {rMin};
			int parentPID = getppid();
			sigqueue(parentPID, SIGUSR1, retInfo);

			retInfo.sival_int = rMax;
			sigqueue(parentPID, SIGUSR2, retInfo);

			retInfo.sival_int = rsum1+rsum2;
			sigqueue(parentPID, SIGPIPE, retInfo);

			fprintf(output, "hi I'm process %d and my parent is %d\n", getpid(), getppid());
			fflush(output);
			free(narray);
			exit(1);
		} else{
			wait(pid1);
			kill(pid2, SIGBUS);
			wait(pid2);
			break;
		}
	}
	if(right - left <= 1){
		int tmin = 32767;
		int tmax = -1;
		int tsum = 0;
		int i;
		for(i = left; i <= right; i++){
			tmin = (tmin < narray[i]) ? tmin : narray[i];
			tmax = (tmax > narray[i]) ? tmax : narray[i];
			tsum += narray[i];
		}

		sigprocmask(SIG_BLOCK, &maskSIGBUS, &oldmask);
		while(sig == 0){
			sigsuspend(&oldmask);
		}
		sigprocmask(SIG_UNBLOCK, &maskSIGBUS, NULL);


		union sigval retInfo = {tmin};
		int parentPID = getppid();
		sigqueue(parentPID, SIGUSR1, retInfo);

		retInfo.sival_int = tmax;
		sigqueue(parentPID, SIGUSR2, retInfo);

		retInfo.sival_int = tsum;
		sigqueue(parentPID, SIGPIPE, retInfo);

		fprintf(output, "Hi I'm process %d and my parent is %d\n", getpid(), getppid());
		fflush(output);
		free(narray);
		exit(1);
	}

	fprintf(output, "Max = %d\nMin = %d\nSum = %d\n", rMax, rMin, rsum1+rsum2);
	fflush(output);
	fprintf(output, "There are' %d misbehaving children\n", numchildren);
	free(narray);
	fclose(output);
	return 0;

}
