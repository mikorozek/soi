#ifndef INITIALIZATION_H
#define INITIALIZATION_H
#include "buffer.h"
#include <string.h>
#define AMOUNT_OF_BUFFER_SEMAPHORES 3
#define AMOUNT_OF_BUFFERS 3
#define FULL_SEMAPHORE 0
#define EMPTY_SEMAPHORE 1
#define MUTEX_SEMAPHORE 2


union semun {
  int value;
  struct semid_ds *buf;
  unsigned short *array;
};

void initializeMessageArray(Message *message_array);
void initializeSpecialMessageArray(Message* special_message_array);
void initializeBuffers(Buffer *buffer_array[], int memory_segment_ids[]);
void initializeSemaphores(Buffer *buffer_array[]);

#endif