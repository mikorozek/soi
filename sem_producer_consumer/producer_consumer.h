#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H
#include "initialization.h"

int semaphore_wait(int semaphore_id, int semaphore_type);
int semaphore_post(int semaphore_id, int semaphore_type);
void insertMessage(Buffer* buffer, Message message);
void insertSpecial(Buffer* buffer_array[], Message special_message);
Message removeMessage(Buffer* buffer);

#endif