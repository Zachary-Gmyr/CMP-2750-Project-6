// Project 6 - Zachary Gmyr
// (description): 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHMKEY 859047
#define BUFF_SZ sizeof(int)*2

void help(char * programName) {
	printf("Usage: %s [number list]\n",programName);
	printf("(description): give a sequence of 2+ numbers, this program returns a pair from this sequence which sums to '19'\n");
	exit(EXIT_SUCCESS);
}

// given a number sequence & pair, updates pair with two numbers from the sequence that add to '19'. If a pair is found '1' is returned, otherwise '0' is returned.
int findSum(int* sequence,int size ,int* pair) {
	int i,j;
	for (i=0;i<(size-1);i++) {
		for (j=i+1;j<size;j++) {
			if (sequence[i] + sequence[j] == 19) {	
				pair[0] = sequence[i];
				pair[1] = sequence[j];
				return 1;
			}
		}
	}
	return 0;
}

int main(int argc, char ** argv) {
	int option;
	while ( (option=getopt(argc,argv,"h")) != -1) {
		if (option == 'h') help(argv[0]);
	}

	if (argc < 3) {
		fprintf(stderr,"ERROR: two arguments expected, for help use '-h'...\n");
		exit(EXIT_FAILURE);
	}

	int shmid = shmget(SHMKEY,BUFF_SZ,0777 | IPC_CREAT);
	if (shmid == -1) {
		fprintf(stderr,"ERROR: shmget failed...");
		exit(EXIT_FAILURE);
	}
	
	int *pint = (int *)(shmat(shmid,0,0));
	pint[0] = -200;
	pint[1] = -200;

	pid_t pid = fork();
	int status;

	if (pid == -1) {
		// failed fork
		fprintf(stderr,"ERROR: fork failed...");
		shmdt(pint);
		shmctl(shmid,IPC_RMID,NULL);
		exit(EXIT_FAILURE);
	}
	else if (pid == 0) {
		// child code here

		// allocate dynamic memory for number sequence
		int* numSequence;
		numSequence = (int *)malloc(sizeof(int)*(argc-1));
		if (numSequence == NULL) {
			fprintf(stderr,"ERROR: failed to allocate dynamic memory");
			shmdt(pint);
			shmctl(shmid,IPC_RMID,NULL);
			_exit(EXIT_FAILURE);
		}
		
		// store args in dynamic array
		int i,seqSize=0;
		for (i=1; i<argc; i++) {
			numSequence[seqSize++] = atoi(argv[i]);
		}
		
		// default pair should be {-1,-1} if no sum found
		int pair[2] = {-1,-1};

		// search if sum of '19' exists, store in 'pair'
		int foundSum = findSum(numSequence,seqSize,pair);

		if (foundSum)
			printf("CHILD: pair found...\n");
		else
			printf("CHILD: pair not found...\n");

		// attach to shared memory & verify -200 -200
		int child_shmid = shmget(SHMKEY,BUFF_SZ,0777);
		if (child_shmid == -1) {
			fprintf(stderr,"ERROR: shmget failed in child...");
			shmdt(pint);
			shmctl(shmid,IPC_RMID,NULL);
			free(numSequence);
			_exit(EXIT_FAILURE);
		}
		int *cint = (int *)(shmat(child_shmid,0,0));
		
		if (cint[0] != -200 || cint[1] != -200) {
			fprintf(stderr,"ERROR: issue with creation of shared memory");
			shmdt(pint);
			shmctl(shmid,IPC_RMID,NULL);
			free(numSequence);
			_exit(EXIT_FAILURE);
		}

		// update shared memory with pair
		// ... if no pair was found, default is {-1,-1}
		cint[0] = pair[0];
		cint[1] = pair[1];

		// cleanup
		free(numSequence);
		shmdt(cint);

		_exit(EXIT_SUCCESS);
	}
	else {
		// parent code here
		pid = wait(&status);
		
		// if child failed then stop here
		if (status != 0) {
			fprintf(stderr,"PARENT: child encountered error");
			exit(EXIT_FAILURE);
		}

		if (pint[0] == -200 && pint[1] == -200) {
			printf("PARENT: shared memory holds -200 -200, error may have occured with child in updating shared memory.\n");
		}
		else if (pint[0] == -1 && pint[1] == -1) {
			printf("PARENT: shared memroy holds -1 -1, no pair sum of '19' was found.\n");
		}
		else {
			printf("PARENT: pair found by child: %d %d\n",pint[0],pint[1]);
		}
	}
	shmdt(pint);
	shmctl(shmid,IPC_RMID,NULL);
}

