#include "producer_consumer_monitor.h"
#include "threads1.h"
#include <chrono>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>

int main() {
  std::vector<ProducerConsumerMonitor> buffers;
  for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
    ProducerConsumerMonitor buffer = ProducerConsumerMonitor(i);
    buffers.emplace_back(buffer);
  }
  pthread_t threads[7];
  ThreadArgs args[6];

  for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
    args[i] = {&buffers[i], i};
    pthread_create(&threads[i], NULL, thread_insertMessage, (void *)&args[i]);
  }

  for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
    args[i + 3] = {&buffers[i], i};
    pthread_create(&threads[i + 3], NULL, thread_removeMessage,
                   (void *)&args[i + 3]);
  }

  pthread_create(&threads[6], NULL, thread_specialMessage, (void *)&buffers);

  for (int i = 0; i < 7; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}