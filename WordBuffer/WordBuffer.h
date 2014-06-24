#ifndef WORD_BUFFER_H
#define WORD_BUFFER_H

#include <Arduino.h>
#include <new.h>

#define DEFAULT_SIZE 128

class WordBuffer{
public:
  WordBuffer();
  WordBuffer(uint8_t max_size);
  ~WordBuffer();
  boolean insert(word val);
  word remove();
  boolean isEmpty();
  boolean isFull();
  uint8_t getNumElements();
  uint8_t getMaxSize();
private:
  uint8_t max_size;
  uint8_t insertIdx;
  uint8_t removeIdx;
  uint8_t numItems;
  volatile word *arr;
};

#endif //WORD_BUFFER_H