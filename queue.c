#include <stdlib.h>
#include "queue.h"

node * head = NULL;
node * tail = NULL;

void enqueue(int * client_socket) {
  node * new_node = (node*) malloc(sizeof(node));
  new_node -> client_socket = client_socket;
  new_node -> next = NULL;

  if(tail == NULL) {
    head = new_node;
  } else {
    tail -> next = new_node;
  }

  tail = new_node;
}

int* dequeue() {
  if(head == NULL) {
    return NULL;
  } else {
    int * res = head -> client_socket;
    node * temp = head;
    head = head -> next;
    if(head == NULL) {
      tail = NULL;
    }

    free(temp);

    return res;
  }
}
