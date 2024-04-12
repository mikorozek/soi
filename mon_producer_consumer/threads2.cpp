#include "threads2.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>

void *thread_specialMessage(void *args) {
  std::vector<ProducerConsumerMonitor> *buffers =
      (std::vector<ProducerConsumerMonitor> *)args;

  for (int i = 0; i < 20; i++) {
    Message special_message;
    special_message.message_id = i;
    std::stringstream ss;
    ss << "SPECIAL " << i;
    special_message.message_string = ss.str();
    insert_special(*buffers, special_message);
  }
  return nullptr;
}

void *thread_removeMessage(void *args) {
  ThreadArgs *thread_args = (ThreadArgs *)args;
  ProducerConsumerMonitor *monitor = thread_args->monitor;
  int buffer_id = thread_args->buffer_id;

  usleep(2000000);
  while (true) {
    Message message;
    message = monitor->removeMessage();
    usleep(2000000);
  }

  return nullptr;
}