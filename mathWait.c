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
			exit(EXIT_FAILURE);
		}
		
		// store args in dynamic array
		int i,seqSize=0;
		for (i=1; i<argc; i++) {
			numSequence[seqSize++] = atoi(argv[i]);
		}
		
		// default pair should be {-1,-1} if no sum found
		int pair[2] = {-1,-1};



		free(numSequence);
		_exit(EXIT_SUCCESS);
	}
	else {
		// parent code here
		pid = wait(&status);
		
		//DEBUG
		printf("PARENT: child exited with %d\n",status);
	}
	shmdt(pint);
	shmctl(shmid,IPC_RMID,NULL);

	//DEBUG
	printf("Ending...\n");
}

/* STEP 3:
 * - Parent should wait while child performs task...
 * - Child puts arguments & stores them in dynamic array
 * - Find one pair of numbers in this array that sums to '19'
 *     ... if no pair exists set default pair to '-1' '-1'
 * - Attach to shared memory region from before
 * - VALIDATE that shared memory was created properly by checking '-200' values
 *     ... exit in error if not true
 * - Copy pair of #'s to shared memory & detach, then terminate
 * - If no pair was found, write instead '-1' '-1'
 */

/* STEP 4:
 * - Check shared memory space after child terminates
 * - If '-200' '-200' then error
 * - If '-1' '-1' then no pair was found
 * - If anything else then simply report the pair
 */
