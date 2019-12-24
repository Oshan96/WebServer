#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <limits.h>

#include <pthread.h>

#define SERVER_PORT 9002
#define MAX_CLIENTS 5

#define BUFFER_SIZE 1024

void* handle_client(int);

void writeHeader(int,char*);
char* getRequestMethod(char*);
char* getPath(char*);
char* getFileType(char*);
char* getContentType(char*);

char server_message[256] = "Server reach successful";

int main(int argc, char const *argv[]) {

  /*create server socket (internet, tcp)*/
  int server_socket, client_socket, addr_size;

  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  if(server_socket == -1) {
    printf("Socket creation failed...\n");
    exit(1);
  } else {
    printf("Socket successfully created...\n");
  }

  /*create socket*/
  struct sockaddr_in server_address, client_address;

  memset(&server_address, '0', sizeof(server_address));
  /*configure socket*/
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(SERVER_PORT);
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);

  /*bind socket*/
  int bind_val = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

  if(bind_val == -1) {
    printf("Socket binding failed...\n");
    exit(1);
  } else {
    printf("Socket binding successful...\n");
  }

  /*listen*/
  int listen_val = listen(server_socket, MAX_CLIENTS);

  if(listen_val == -1) {
    printf("Socket listening failed...\n");
    exit(1);
  } else {
    printf("Listening on socket...\n");
  }

  while(1) {
    /*accept client*/
    addr_size = sizeof(struct sockaddr_in);
    int client_socket = accept(server_socket, (struct sockaddr *) &client_address, (socklen_t *) &addr_size);

    if(client_socket < 0) {
      printf("Client accept failed...\n");
      exit(1);
    } else {
      printf("Client accepted...\n");
    }

    /*send data to client*/
    // send(client_socket, server_message, sizeof(server_message), 0);

    handle_client(client_socket);
  }


  close(server_socket);


  return 0;
}

void* handle_client(int client_socket) {

  char buffer[BUFFER_SIZE];
  size_t bytes_read;
  int msg_size = 0;
  char* path;

  /*read the message from client*/
  while((bytes_read = read(client_socket, buffer+msg_size, sizeof(buffer) - msg_size - 1)) > 0) {
    msg_size += bytes_read;

    if(msg_size > BUFFER_SIZE - 1 || buffer[msg_size-1] == '\n') {
      break;
    }
  }

  if(bytes_read == -1) {
    printf("Recieve error...\n");
    exit(1);
  }

  buffer[msg_size - 1] = 0;   //null terminate the msg and remove \n

  printf("Request : %s\n", buffer);
  fflush(stdout);

  path = getPath(buffer);
  path++;

  // if(realpath(buffer, path) == NULL) {
  //   printf("ERROR (bad path) : %s\n", path);
  //   close(client_socket);
  //   return NULL;
  // }

  /*read the file and send to client*/
  FILE *file = fopen(path, "r");
  if(file == NULL) {
    printf("ERROR (File open) : %s\n", path);
    write(client_socket, buffer, bytes_read);

    close(client_socket);
    fclose(file);
    return NULL;
  }

  /*write response headers*/
  writeHeader(client_socket, "HTTP/1.1 200 OK\r\n");

  char* fileType = getFileType(path);
  printf("File Type : %s\n", fileType);

  fileType++;   //remove the dot(.) at the begining of the extention

  char* contentType = getContentType(fileType);
  printf("Content Type : %s\n", contentType);

  writeHeader(client_socket, contentType);

  while((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
    printf("Sending %zu bytes\n", bytes_read);
    write(client_socket, buffer, bytes_read);
  }

  fclose(file);
  close(client_socket);

  printf("Closing client connection...\n");

  return NULL;
}

void writeHeader(int client, char* header) {
  write(client, header, strlen(header));
}

char* getRequestMethod(char* request) {
  char* ptr = strtok(request, " ");
  return ptr;
}

char* getPath(char* request) {

  int count = 0;
  char* delimiter = " ";

  char* ptr = strtok(request, delimiter);

  while (ptr != NULL && count != 1) {
    printf("%s\n", ptr);
    printf("%d\n", count);
    ptr = strtok(NULL, delimiter);
    count++;
  }

  return ptr;
}

char* getContentType(char* fileType) {

  if(strcmp(fileType, "html") == 0) {
    return "Content-Type: text/html\r\n\r\n";
  } else if(strcmp(fileType, "png") == 0) {
    return "Content-Type: image/png\r\n\r\n";
  } else if (strcmp(fileType, "ico") == 0 ) {
    return "Content-Type: image/x-icon\r\n\r\n";
  }

  return "";
}

char* getFileType(char* filePath) {
  return strrchr(filePath, '.');
}
