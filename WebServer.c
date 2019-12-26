#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <limits.h>
# include <unistd.h>

#include <pthread.h>

#define SERVER_PORT 9002
#define MAX_CLIENTS 5

#define BUFFER_SIZE 1024

void* handle_client(int);

void writeHeader(int,char*);
char* getRequestMethod(char*);
char* getPath(char*);
char* getFileType(char*);
char* getMIMEType(char*);
char* getContentType(char*);

char server_message[256] = "Server reach successful";

FILE * error_404;

int main(int argc, char const *argv[]) {

  error_404 = fopen("error_404.html", "r");

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
  if(strcmp(path,"/") == 0) {
    path = "index.html";
  } else {
    path++;
  }

  /*read the file and send to client*/
  FILE *file = fopen(path, "r");
  if(file == NULL) {
    printf("ERROR (File open) : %s\n", path);
    writeHeader(client_socket, "HTTP/1.1 404 Not Found\r\n");
    writeHeader(client_socket, getContentType("html"));

    while((bytes_read = fread(buffer, 1, BUFFER_SIZE, error_404)) > 0) {
      write(client_socket, buffer, bytes_read);
    }

    close(client_socket);
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

  free(contentType);    //free allocated memory to the pointer

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

char* getMIMEType(char* fileType) {
  if(strcmp(fileType, "html") == 0 || strcmp(fileType, "htm") == 0) {
    return "text/html";
  } else if(strcmp(fileType, "png") == 0) {
    return "image/png";
  } else if(strcmp(fileType, "jpg") == 0 || strcmp(fileType, "pjp") == 0 || strcmp(fileType, "jpeg") == 0 || strcmp(fileType, "jfif") == 0 || strcmp(fileType, "pjpeg") == 0 ) {
    return "image/jpeg";
  } else if(strcmp(fileType, "ico") == 0 || strcmp(fileType, "cur") == 0) {
    return "image/x-icon";
  } else if(strcmp(fileType, "bmp") == 0) {
    return "image/bmp";
  } else if(strcmp(fileType, "gif") == 0) {
    return "image/gif";
  } else if(strcmp(fileType, "svg") == 0) {
    return "image/svg+xml";
  } else if(strcmp(fileType, "webp") == 0) {
    return "image/webp";
  } else if(strcmp(fileType, "apng") == 0) {
    return "image/apng";
  } else if(strcmp(fileType, "tif") == 0 || strcmp(fileType, "tiff") == 0) {
    return "image/tiff";
  } else if(strcmp(fileType, "mp2") == 0 || strcmp(fileType, "mp3") == 0 || strcmp(fileType, "mpga") == 0) {
    return "audio/mpeg";
  } else if(strcmp(fileType, "mpe") == 0 || strcmp(fileType, "mpeg") == 0 || strcmp(fileType, "mpg") == 0) {
    return "video/mpeg";
  } else if(strcmp(fileType, "mp4") == 0) {
    return "video/mp4";
  } else if(strcmp(fileType, "wav") == 0) {
    return "audio/x-wav";
  } else if(strcmp(fileType, "zip") == 0) {
    return "application/zip";
  } else if(strcmp(fileType, "doc") == 0) {
    return "application/msword";
  } else if(strcmp(fileType, "xls") == 0) {
    return "application/vnd.ms-excel";
  } else if(strcmp(fileType, "pdf") == 0) {
    return "application/pdf";
  } else if(strcmp(fileType, "csv") == 0) {
    return "text/csv";
  }

  return "text/plain";
}

char* getContentType(char* fileType) {
  char * cont = "Content-Type: ";
  char * mime = getMIMEType(fileType);
  char * ret = "\r\n\r\n";

  char * result = (char *) malloc(strlen(cont) + strlen(mime) + strlen(ret));

  strcpy(result, cont);
  strcat(result, mime);
  strcat(result, ret);

  return result;
}

char* getFileType(char* filePath) {
  return strrchr(filePath, '.');
}
