#include <WordBuffer.h>
WordBuffer::WordBuffer(){
  WordBuffer::max_size = DEFAULT_SIZE;
  arr = new word[max_size];
  insertIdx = 0;
  removeIdx = 0;
  numItems = 0;
}

WordBuffer::WordBuffer(uint8_t max_size = DEFAULT_SIZE){
  WordBuffer::max_size = max_size;
  arr = new word[max_size];
  insertIdx = 0;
  removeIdx = 0;
  numItems = 0;
}

WordBuffer::~WordBuffer(){
  delete[] arr;
}
boolean WordBuffer::insert(word val){
  if(!isFull()){
    insertIdx %= max_size;
    arr[insertIdx++] = val;
    numItems++;
  }else{
    return -1;
  }
}

word WordBuffer::remove(){
  if(numItems > 0){
    numItems--;
    removeIdx %= max_size;
    return arr[removeIdx++];
  }
  else{
    return -1;
  }
}

boolean WordBuffer::isEmpty(){
  return numItems == 0;
}

boolean WordBuffer::isFull(){
  return numItems == max_size;
}

uint8_t WordBuffer::getNumElements(){
  return numItems;
}

uint8_t WordBuffer::getMaxSize(){
  return max_size;
}

