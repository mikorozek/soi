#ifndef BUFFER_H
#define BUFFER_H

#include "message.h"

#define BUFFER_SIZE 10

typedef int semaphore_set_id;

typedef struct {
  Message message_array[BUFFER_SIZE];
  Message special_message_array[BUFFER_SIZE];
  int head;
  int tail;
  int special_top;
  int buffer_id;
  semaphore_set_id sem_set_id;
} Buffer;

#endif