#include "initialization.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>


void initializeBuffers(Buffer* buffer_array[], int memory_segment_ids[]) {
  for (int i = 0; i < AMOUNT_OF_BUFFERS; i++){
	memory_segment_ids[i] = shmget(IPC_PRIVATE, sizeof(Buffer), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (memory_segment_ids[i] == -1) {
		perror("shmget");
		exit(-1);
	}
	buffer_array[i] = (Buffer*)shmat(memory_segment_ids[i], NULL, 0);
	if (buffer_array[i] == (void*) -1) {
		perror("shmat");
		exit(-1);
	}
	buffer_array[i]->head = 0;
	buffer_array[i]->tail = 0;
	buffer_array[i]->special_top = -1;
	buffer_array[i]->sem_set_id = -1;
	buffer_array[i]->buffer_id = i;
  }
}
/* 				REMINDER				
   0 - semaphore full 					
   1 - semaphore empty
   2 - semaphore mutex					*/

void initializeSemaphores(Buffer* buffer_array[]) {
  for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
	buffer_array[i]->sem_set_id = semget(IPC_PRIVATE, AMOUNT_OF_BUFFER_SEMAPHORES, 0666 | IPC_CREAT);
	union semun sem_union;
	sem_union.value = 0;
	if (semctl(buffer_array[i]->sem_set_id, FULL_SEMAPHORE, SETVAL, sem_union) == -1) {
		perror("ERROR INITIALIZING FULL SEMAPHORE VALUE");
		exit(-1);
	}
	sem_union.value = BUFFER_SIZE;
	if (semctl(buffer_array[i]->sem_set_id, EMPTY_SEMAPHORE, SETVAL, sem_union) == -1) {
		perror("ERROR INITIALIZING EMPTY SEMAPHORE VALUE");
		exit(-1);
	}
	sem_union.value = 1;
	if (semctl(buffer_array[i]->sem_set_id, MUTEX_SEMAPHORE, SETVAL, sem_union) == -1) {
		perror("ERROR INITIALIZING MUTEX SEMAPHORE VALUE");
		exit(-1);
	}
  }
}