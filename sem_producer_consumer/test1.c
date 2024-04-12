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
	printf("buffer initialization successful\n");
	initializeSemaphores(buffer_array);
	printf("semaphore initialization successful\n");
	pid_t special_message_pid = fork();
	if (special_message_pid == 0) {
		printf("Special message pid: %d\n", getpid());
		while (1) {
			sleep(3);
			Message message = {.message_id = 11, .message_string = "SPECIAL"};
			insertSpecial(buffer_array, message);
			printf("SPECIAL MESSAGE INSERT\n");
		}
	}
	for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
		pid_t consuming_process_id = fork();
		if (consuming_process_id == 0) {
			printf("Consuming pid: %d\n", getpid());
			while (1) {
				sleep(1);
				Message message = removeMessage(buffer_array[i]);
			}
		}
	}
	for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
		pid_t producing_process_id = fork();
		if (producing_process_id == 0) {
			printf("Producing pid: %d\n", getpid());
			int a = 0;
			while (1) {
				Message message;
				message.message_id = a;
				sprintf(message.message_string, "Message number %d", a);
				insertMessage(buffer_array[i], message);
				a += 1;
			}
		}
	}
	while (wait(NULL) > 0);
	return 0;
}