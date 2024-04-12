#include "producer_consumer_monitor.h"
#include <iostream>
#include <stdio.h>

void insert_special(std::vector<ProducerConsumerMonitor> &buffers,
                    const Message &special_message) {
  for (int i = 0; i < AMOUNT_OF_BUFFERS; i++) {
    buffers[i].insertSpecialMessage(special_message);
  }
}

void Semaphore::p() {
  if (sem_wait(&sem) != 0)
    throw "sem_wait: failed";
}

void Semaphore::v() {
  if (sem_post(&sem) != 0)
    throw "sem_post: failed";
}

void Condition::wait() { w.p(); }

bool Condition::signal() {
  if (waitingCount) {
    --waitingCount;
    w.v();
    return true;
  } // if
  else
    return false;
}

void ProducerConsumerMonitor::enter() { s.p(); }

void ProducerConsumerMonitor::leave() { s.v(); }

void ProducerConsumerMonitor::wait(Condition &cond) {
  ++cond.waitingCount;
  leave();
  cond.wait();
}

void ProducerConsumerMonitor::signal(Condition &cond) {
  if (cond.signal())
    enter();
}

void ProducerConsumerMonitor::insertMessage(const Message &message) {
  enter();
  while (buffer.getMessageCount() == BUFFER_SIZE) {
    wait(full);
  }
  buffer.insertMessage(message);
  if (buffer.getMessageCount() == 1 && buffer.getSpecialMessageCount() == 0) {
    signal(empty);
  }
  printf("I; B: %d\n", this->m_id);
  leave();
}

void ProducerConsumerMonitor::insertSpecialMessage(
    const Message &special_message) {
  enter();
  buffer.insertSpecialMessage(special_message);
  if (buffer.getSpecialMessageCount() == 1 && buffer.getMessageCount() == 0) {
    signal(empty);
  }
  leave();
}

Message ProducerConsumerMonitor::removeMessage() {
  enter();
  while (buffer.getMessageCount() == 0 &&
         buffer.getSpecialMessageCount() == 0) {
    wait(empty);
  }
  Message removed_message = buffer.removeMessage();
  if (buffer.getMessageCount() == (int)buffer.getMaxSize() - 1) {
    signal(full);
  }
  printf("R; B: %d; M: \"%s\"\n", this->m_id,
         removed_message.message_string.c_str());
  leave();
  return removed_message;
}