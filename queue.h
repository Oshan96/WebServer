struct node {
  struct node * next;
  int * client_socket;
};

typedef struct node node;

void enqueue(int*);
int* dequeue();
