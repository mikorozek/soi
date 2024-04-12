#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_MESSAGE_LENGTH 256

typedef unsigned short m_id;

typedef struct {
  m_id message_id;
  char message_string[MAX_MESSAGE_LENGTH];
} Message;

#endif