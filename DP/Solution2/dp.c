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
	struct semid_ds * buf;
	unsigned short * array;
	#if defined(__linux__)
		struct seminfo * __buf;
	#endif
};

int N = 0;
int semid;
union semun arg;
struct sembuf sops;
pid_t pid;
int count = 0;

int left(int i){
	return i;
}

int right(int i){
	return (i+(N-1))%N;
}


void putFork(int no){
	if(no == N-1){
		sops.sem_num = left(no);
		sops.sem_op = 1;
		semop(semid,&sops,1);
		
		//printf("Philosopher %d Has Taken His Left Fork %d\n",no,left(no));
		
		sops.sem_num = right(no);
		sops.sem_op = 1;
		semop(semid,&sops,1);
		
		//printf("Philosopher %d Has Taken His Right Fork %d\n",no,right(no));
	}else{
		sops.sem_num = right(no);
		sops.sem_op = 1;
		semop(semid,&sops,1);
		
		//printf("Philosopher %d Has Put His Right Fork %d Back \n",no,right(no));
		
		sops.sem_num = left(no);
		sops.sem_op = 1;
		semop(semid,&sops,1);
		
		//printf("Philosopher %d Has Put His Left Fork %d Back \n",no,left(no));
	}
}

void takeFork(int no){
	if(no != N-1){
		sops.sem_num = left(no);
		sops.sem_op = -1;
		semop(semid,&sops,1);
		
		//printf("Philosopher %d Has Taken His Left Fork %d\n",no,left(no));
		
		sops.sem_num = right(no);
		sops.sem_op = -1;
		semop(semid,&sops,1);
		
		//printf("Philosopher %d Has Taken His Right Fork %d\n",no,right(no));
	}else{
		sops.sem_num = right(no);
		sops.sem_op = -1;
		semop(semid,&sops,1);
		
		//printf("Philosopher %d Has Taken His Right Fork %d\n",no,right(no));
		
		sops.sem_num = left(no);
		sops.sem_op = -1;
		semop(semid,&sops,1);
		
		//printf("Philosopher %d Has Taken His Left Fork %d\n",no,left(no));
	}
}

void eat(int no){
	srand(time(0)); 
	int time = rand()%3;
	printf("Philosopher %d Is In Eating State\n",getpid());
	sleep(1);//Eating
}

void thinking(int no){
	srand(time(0)); 
	int time = rand()%5;
	printf("Philosopher %d Is In Thinking State\n",getpid());
	sleep(2);//Thinking
}

void philosopher(int no){
	while(1){
		thinking(no);
		takeFork(no);
		eat(no);
		putFork(no);
		//count++;
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
			
			sops.sem_flg = SEM_UNDO;
	
	 
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
	/*
	arg.val = 1;
	semctl(semid,0,SETVAL,arg);
	semctl(semid,1,SETVAL,arg);
	semctl(semid,2,SETVAL,arg);
	semctl(semid,3,SETVAL,arg);
	semctl(semid,4,SETVAL,arg);
	*/
	return 0;
}
