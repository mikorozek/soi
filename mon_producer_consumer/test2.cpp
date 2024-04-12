#include "producer_consumer_monitor.h"
#include "threads2.h"
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
  pthread_t threads[4];
  ThreadArgs args[3];

  for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
    args[i] = {&buffers[i], i};
    pthread_create(&threads[i], NULL, thread_removeMessage, (void *)&args[i]);
  }
  pthread_create(&threads[3], NULL, thread_specialMessage, (void *)&buffers);

  for (int i = 0; i < 4; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
