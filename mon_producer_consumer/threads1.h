#include "producer_consumer_monitor.h"

typedef struct {
  ProducerConsumerMonitor *monitor;
  int buffer_id;
} ThreadArgs;

void *thread_insertMessage(void *args);

void *thread_specialMessage(void *args);

void *thread_removeMessage(void *args);