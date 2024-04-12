#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_SIZE 10
#define MAX_MESSAGE_LENGTH 256
#include <deque>
#include <iostream>
#include <string>

typedef struct {
  unsigned short message_id;
  std::string message_string;
} Message;

class Buffer {
public:
  Buffer(size_t maxSize = BUFFER_SIZE) : maxBufferSize(maxSize) {}

  size_t getMaxSize() const { return maxBufferSize; }

  int getMessageCount() const { return messageQueue.size(); }

  int getSpecialMessageCount() const { return specialMessageStack.size(); }

  void insertMessage(const Message &message) {
    messageQueue.push_back(message);
  }

  void insertSpecialMessage(const Message &special_message) {
    if (specialMessageStack.size() == maxBufferSize) {
      specialMessageStack.pop_front();
    }
    specialMessageStack.push_back(special_message);
  }

  Message removeMessage() {
    Message removed_message;
    if (specialMessageStack.size() > 0) {
      removed_message = specialMessageStack.back();
      specialMessageStack.pop_back();
      return removed_message;
    }
    removed_message = messageQueue.front();
    messageQueue.pop_front();
    return removed_message;
  }

private:
  std::deque<Message> messageQueue;
  std::deque<Message> specialMessageStack;
  size_t maxBufferSize;
};

#endif