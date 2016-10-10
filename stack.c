#include "stack.h"

TransmissionItem *getTop() {
  return top;
}

void push(TransmissionItem *newItem) {
  if (top != NULL) {
    newItem->next = top;
  } else {
    top->next = NULL;
  }
  top = newItem;

}

void pop() {
  top = top->next;
}
