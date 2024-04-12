#include "producer_consumer.h"
#include "initialization.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>


Buffer* buffer_array[AMOUNT_OF_BUFFERS];
int memory_segment_ids[AMOUNT_OF_BUFFERS];
pid_t main_process_id;


void signal_handler(int sig) {
	if (getpid() == main_process_id){
		for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
			union semun ignored_argument;
			semctl(buffer_array[i]->sem_set_id, 3, IPC_RMID, ignored_argument);
			shmdt(buffer_array[i]);
			shmctl(memory_segment_ids[i], IPC_RMID, 0);
		}
	}
	exit(sig);
}


int main() {
	main_process_id = getpid();
	signal(SIGINT, signal_handler);
	initializeBuffers(buffer_array, memory_segment_ids);
	printf("Buffer initialization successful\n");
	initializeSemaphores(buffer_array);
	printf("semaphore initialization successful\n");
	pid_t special_message_pid = fork();
	if (special_message_pid == 0) {
		printf("Special message pid: %d\n", getpid());
		for (int i = 0; i < 20; i++) {
			Message message;
			message.message_id = 0;
			sprintf(message.message_string, "SPECIAL MESSAGE %d", i);
			insertSpecial(buffer_array, message);
		}
		exit(0);
	}
	sleep(2);
	for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
		pid_t consuming_process_id = fork();
		if (consuming_process_id == 0) {
			printf("Consuming process pid: %d\n", getpid());
			while (1) {
				sleep(1);
				Message message = removeMessage(buffer_array[i]);
			}
		}
	}
	while (wait(NULL) > 0);
	for (int i = 0; i < AMOUNT_OF_BUFFERS; i++){
		union semun ignored_argument;
		semctl(buffer_array[i]->sem_set_id, 3, IPC_RMID, ignored_argument);
		shmdt(buffer_array[i]);
		shmctl(memory_segment_ids[i], IPC_RMID, 0);
	}
}