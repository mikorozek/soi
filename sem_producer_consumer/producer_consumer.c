#include "producer_consumer.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>



int semaphore_wait(int semaphore_id, int semaphore_type) {
	struct sembuf operations[1];
	operations[0].sem_num = semaphore_type;
	operations[0].sem_op = -1;
	operations[0].sem_flg = 0;

	return semop(semaphore_id, operations, 1);
}


int semaphore_post(int semaphore_id, int semaphore_type) {
	struct sembuf operations[1];
	operations[0].sem_num = semaphore_type;
	operations[0].sem_op = 1;
	operations[0].sem_flg = 0;

	return semop(semaphore_id, operations, 1);
}


void insertMessage(Buffer* buffer, Message message){
	semaphore_wait(buffer->sem_set_id, EMPTY_SEMAPHORE);
	semaphore_wait(buffer->sem_set_id, MUTEX_SEMAPHORE);
	buffer->message_array[buffer->tail] = message;
	buffer->tail = (buffer->tail + 1) % BUFFER_SIZE; 
	printf("Message inserted into buffer %d\n", buffer->buffer_id);
	semaphore_post(buffer->sem_set_id, MUTEX_SEMAPHORE);
	semaphore_post(buffer->sem_set_id, FULL_SEMAPHORE);
}


void insertSpecial(Buffer* buffer_array[], Message special_message) {
	for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
		Buffer *current_buffer = buffer_array[i];
		semaphore_wait(current_buffer->sem_set_id, MUTEX_SEMAPHORE);
		if (current_buffer->special_top == BUFFER_SIZE - 1) {
			for (int j = 0; j < current_buffer->special_top; j++) {
				current_buffer->special_message_array[j] = current_buffer->special_message_array[j + 1];
			}
			current_buffer->special_message_array[current_buffer->special_top] = special_message;
			semaphore_post(current_buffer->sem_set_id, MUTEX_SEMAPHORE);
			continue;
		}
		current_buffer->special_top += 1;
		current_buffer->special_message_array[current_buffer->special_top] = special_message;
		semaphore_post(current_buffer->sem_set_id, MUTEX_SEMAPHORE);
		semaphore_post(current_buffer->sem_set_id, FULL_SEMAPHORE);
	}
}


Message removeMessage(Buffer* buffer) {
	Message message_to_consume;
	semaphore_wait(buffer->sem_set_id, FULL_SEMAPHORE);
	semaphore_wait(buffer->sem_set_id, MUTEX_SEMAPHORE);
	if (buffer->special_top >= 0) {
		message_to_consume = buffer->special_message_array[buffer->special_top];
		buffer->special_top -= 1;
		printf("Message removed: %s; Buffer: %d\n", message_to_consume.message_string, buffer->buffer_id);
		semaphore_post(buffer->sem_set_id, MUTEX_SEMAPHORE);
		return message_to_consume;
	}
	message_to_consume = buffer->message_array[buffer->head];
	memset(&(buffer->message_array[buffer->head]), 0, sizeof(Message));
	buffer->head = (buffer->head + 1) % BUFFER_SIZE;
	printf("Message removed: %s; Buffer: %d\n", message_to_consume.message_string, buffer->buffer_id);
	semaphore_post(buffer->sem_set_id, MUTEX_SEMAPHORE);
	semaphore_post(buffer->sem_set_id, EMPTY_SEMAPHORE);
	return message_to_consume;
}
