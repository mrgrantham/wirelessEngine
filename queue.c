#include "queue.h"
#include "stdlib.h"


uint8_t getFront() {
  return front->val;
}

uint8_t getBack() {
  return back->val;
}

uint8_t getNext(uint8_t reset) {
  static TransmissionItem *queueTraverser;
  static uint8_t first = 1;
  if (first) {
    queueTraverser = front;
    first = 0;
  }

  if(!reset) {
    uint8_t temp = queueTraverser->val;
    queueTraverser = queueTraverser->ahead;
    return temp;
  } else {
    queueTraverser = front;
  }
  return -1;
}

void push(uint8_t val) {
  TransmissionItem *tmp = malloc(sizeof(TransmissionItem));
  tmp->behind = NULL;
  tmp->val = val;
  if (back != NULL) {
    tmp->ahead = back;
  } else { // if queue is empty
    tmp->ahead = NULL;
    front=tmp;
  }
  back = tmp;

}


void pop() {
  if (front) {
    TransmissionItem *tmp = front;
    front = tmp->behind;
    free(tmp);
  } else {
    free(back);
  }
}
