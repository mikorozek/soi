#include "threads1.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>

void *thread_insertMessage(void *args) {
  ThreadArgs *thread_args = (ThreadArgs *)args;
  ProducerConsumerMonitor *monitor = thread_args->monitor;
  int buffer_id = thread_args->buffer_id;
  int a = 0;
  while (true) {
    Message message;
    message.message_id = 0;
    std::stringstream ss;
    ss << "Message " << a;
    message.message_string = ss.str();
    monitor->insertMessage(message);
    a += 1;
  }
  return nullptr;
}

void *thread_specialMessage(void *arg) {
  std::vector<ProducerConsumerMonitor> *buffers =
      (std::vector<ProducerConsumerMonitor> *)arg;

  while (true) {
    Message special_message{1, "SPECIAL"};
    insert_special(*buffers, special_message);
    printf("SPECIAL MESSAGE INSERTED\n");
    usleep(3000000);
  }
  return nullptr;
}

void *thread_removeMessage(void *args) {
  ThreadArgs *thread_args = (ThreadArgs *)args;
  ProducerConsumerMonitor *monitor = thread_args->monitor;
  int buffer_id = thread_args->buffer_id;

  while (true) {
    Message message;
    message = monitor->removeMessage();

    usleep(1000000);
  }
  return nullptr;
}