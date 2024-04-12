#ifndef PRODUCER_CONSUMER_MONITOR_H
#define PRODUCER_CONSUMER_MONITOR_H

#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define AMOUNT_OF_BUFFERS 3

class Semaphore {
public:
  Semaphore(int value) {
    if (sem_init(&sem, 0, value) != 0)
      throw "sem_init: failed";
  }
  ~Semaphore() { sem_destroy(&sem); }

  void p();

  void v();

private:
  sem_t sem;
};

class Condition {
  friend class ProducerConsumerMonitor;

public:
  Condition() : w(0) { waitingCount = 0; }

  void wait();

  bool signal();

private:
  Semaphore w;
  int waitingCount; // liczba oczekujacych watkow
};

class ProducerConsumerMonitor {
public:
  ProducerConsumerMonitor(int id) : s(1) { this->m_id = id; }

  void enter();

  void leave();

  void wait(Condition &cond);

  void signal(Condition &cond);

  void insertMessage(const Message &message);

  void insertSpecialMessage(const Message &special_message);

  Message removeMessage();

private:
  Semaphore s;
  Condition full, empty;
  Buffer buffer;
  int m_id;
};

void insert_special(std::vector<ProducerConsumerMonitor> &buffers,
                    const Message &special_message);

#endif
