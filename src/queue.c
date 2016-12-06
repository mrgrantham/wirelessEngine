#include "queue.h"
#include "stdlib.h"
#include "stdio.h"


uint8_t getFront() {
	if (front == NULL) {

		return 0xff;
	} else {
		return front->val;
	}
}

uint8_t getBack() {
  return (back == NULL) ? 0xff : back->val;
}

uint8_t getNext(uint8_t reset) {
  // printf("---GETTING NEXT---\n");
  static TransmissionItem *queueTraverser;
  static uint8_t first = 1;
  if (first) {
    queueTraverser = front;
    first = 0;
  }

  if(!reset) {
    uint8_t temp;
    if (queueTraverser != NULL) {
      temp = queueTraverser->val;
      queueTraverser = queueTraverser->behind;
    } else {
    	temp = 0xff;
    }
    return temp;
  } else {
	  queueTraverser = front;
  }
  return 0xff;
}

void push(uint8_t val) {
	//printf("pushing\n");
	queueSize++;
	TransmissionItem *tmp = malloc(sizeof(TransmissionItem));
	tmp->behind = NULL;
	tmp->val = val;
	if (back != NULL) {
		back->behind = tmp;
		tmp->ahead = back;
	} else { // if queue is empty
		tmp->ahead = NULL;
		front=tmp;
	}
	back = tmp;
	//printf("push\n");
}


void pop() {
  if (front != NULL) {
    TransmissionItem *tmp = front;
    if (front == back) {
    	back = NULL;
    	free(front);
    	front = NULL;
    } else {
        front = tmp->behind;
        free(tmp);
    }

  }
  if(queueSize != 0) {
	  queueSize--;
  }

  //printf("pop\n");
}





int32_t getQueueSize() {
	return queueSize;
}
