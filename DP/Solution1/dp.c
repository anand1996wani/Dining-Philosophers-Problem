#include <stdio.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <time.h>

#define key (2001)


union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	#if defined(__linux__)
		struct seminfo * __buf;
	#endif
};

int N = 0;
int semid;


union semun arg;
struct sembuf sops[2];
pid_t pid;
int count = 0;

int left(int i){
	return i;
}

int right(int i){
	return (i+(N-1))%N;
}


void putFork(int no){
		sops[0].sem_num = left(no);
		sops[0].sem_op = 1;
		
		sops[1].sem_num = right(no);
		sops[1].sem_op = 1;
		
		semop(semid,sops,2);
		
		//printf("Philosopher %d Has Put Down His Left Fork %d And Right Fork %d\n",no,left(no),right(no));
		
		//printf("Philosopher %d Has Taken His Right Fork %d\n",no,right(no));
}

void takeFork(int no){
		sops[0].sem_num = left(no);
		sops[0].sem_op = -1;
		
		
		sops[1].sem_num = right(no);
		sops[1].sem_op = -1;
		
		semop(semid,sops,2);
		
		//printf("Philosopher %d Has Taken His Left Fork %d And Right Fork %d\n",no,left(no),right(no));
		printf("Philosopher %d Is In Eating State\n",getpid());
		
		//printf("Philosopher %d Has Taken His Right Fork %d\n",no,right(no));
}

void thinking(int no){	
	printf("Philosopher %d Is In Thinking State\n",getpid());
	sleep(2);//Thinking
}

void philosopher(int no){
	while(1){
		thinking(no);
		takeFork(no);
		sleep(3);	//Eating
		putFork(no);
	}
}


int main(int argc,char *argv[]){
	if(argc < 2 ){
		printf("Please Tell Number Of Philosophers In Command line Argument\n");
		exit(0);
	}else{
		N = atoi(argv[1]);
		if( N < 2){
			printf("Number Of Philosophers Must Be Greater Than Or Equal To 2\n");
			exit(0);
		}else{
			printf("Dinning Philosopher Solution\n");
			printf("For Terminating Program Using Ctrl - c\n");
			semid = semget(key,N,IPC_CREAT | 0666);
			if(semid==-1){
				printf("Sem Error\n");
				exit(1);
			}
			
			arg.array = (unsigned short *)malloc((N)*sizeof(unsigned short));
			
			
			for(int i = 0;i < N;i++){
				arg.array[i] = 1;
			}
			
			semctl(semid,0,SETALL,arg);
			
			sops[0].sem_flg = SEM_UNDO;
			sops[1].sem_flg = SEM_UNDO;
	 
	 		for(int i = 0;i < N;i++){
	 			pid = fork();
	 			if(pid < 0){
	 				printf("Fork Error\n");
	 				exit(1);
	 			}else if(pid == 0){
	 				int philosopherNo = i;
	 				philosopher(philosopherNo);
	 				exit(0);
	 			}else{
	 				continue;
	 			}
	 		}
			while(wait(NULL)!=-1);
		}
	}
	return 0;
}
