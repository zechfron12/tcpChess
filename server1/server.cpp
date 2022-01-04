#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <json-c/json.h>

#include "fileFunctions.h"

#define SIZE 1024

short SocketCreate(void)
{
  short hSocket;
  printf("Create the socket\n");
  hSocket = socket(AF_INET, SOCK_STREAM, 0);
  return hSocket;
}

int BindCreatedSocket(int hSocket)
{
  int iRetval = -1;
  int ClientPort = 90190;
  struct sockaddr_in remote = {0};
  /* Internet address family */
  remote.sin_family = AF_INET;
  /* Any incoming interface */
  remote.sin_addr.s_addr = htonl(INADDR_ANY);
  remote.sin_port = htons(ClientPort); /* Local port */
  iRetval = bind(hSocket, (struct sockaddr *)&remote, sizeof(remote));
  return iRetval;
}

void createConfigFile(int id)
{
  /*Creating a json object*/
  int board[64] = {
      -1, -1, -3, -4, -5, -3, -2, -1,
      -6, -6, -6, -6, -6, -6, -6, -6,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      6, 6, 6, 6, 6, 6, 6, 6,
      1, 2, 3, 4, 5, 3, 2, 1};

  int spritePositions[64] = {
      0, 0, 2, 3, 4, 5, 6, 7,
      8, 9, 10, 11, 12, 13, 14, 15,
      64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64,
      48, 49, 50, 51, 52, 53, 54, 55,
      56, 57, 58, 59, 60, 61, 62, 63};
  int q[64] = {
      64, 64, 64, 64, 64, 64, 64, 64, 
      64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64};

  json_object *classicConfig = json_object_new_object();

  /*Creating a json string*/
  json_object *jstatus = json_object_new_int(1);

  /*Creating a json integer*/
  json_object *jturn = json_object_new_int(1);
  json_object *jplayerId = json_object_new_int(id);

  /*Creating a json boolean*/
  json_object *jneedUpdate = json_object_new_int(1);
  json_object *jisGameOver = json_object_new_int(0);

  /*Creating a json array*/
  json_object *jboard = json_object_new_array();
  json_object *jspritePositions = json_object_new_array();
  json_object *jqPositions = json_object_new_array();

  /*Creating json strings*/
  for (int i = 0; i < 64; i++)
  {
    json_object *jpos = json_object_new_int(board[i]);
    json_object_array_add(jboard, jpos);

    jpos = json_object_new_int(spritePositions[i]);
    json_object_array_add(jspritePositions, jpos);

    jpos = json_object_new_int(q[i]);
    json_object_array_add(jqPositions, jpos);
  }

  /*Form the json object*/
  /*Each of these is like a key value pair*/
  json_object_object_add(classicConfig, "turn", jturn);
  json_object_object_add(classicConfig, "playerId", jplayerId);
  json_object_object_add(classicConfig, "board", jboard);
  json_object_object_add(classicConfig, "spritePositions", jspritePositions);
  json_object_object_add(classicConfig, "qPositions", jqPositions);
  json_object_object_add(classicConfig, "status", jstatus);
  json_object_object_add(classicConfig, "needUpdate", jneedUpdate);
  json_object_object_add(classicConfig, "isGameOver", jisGameOver);

  /* Now printing the json object in the file */
  json_object_to_file("send.json", classicConfig);
  // char buffer[SIZE];
  // strcpy(buffer, json_object_to_json_string(classicConfig));
  // writeToFile(buffer, "send.json");
}

int main(int argc, char *argv[])
{
  json_object *parsed_json;
  json_object *jneedUpdate;
  int needUpdate;

  int socket_desc, sock, clientLen, read_size;
  struct sockaddr_in server, client;
  char client_message[SIZE] = {0};
  char message[SIZE] = {0};

  pid_t childpid;
  int playerId = 0;

  // Create socket
  socket_desc = SocketCreate();
  if (socket_desc == -1)
  {
    printf("Could not create socket");
    return 1;
  }
  printf("Socket created\n");
  // Bind
  if (BindCreatedSocket(socket_desc) < 0)
  {
    // print the error message
    perror("bind failed.");
    return 1;
  }
  printf("bind done\n");
  // Listen
  listen(socket_desc, 3);
  // Accept and incoming connection
  while (1)
  {
    printf("Waiting for incoming connections...\n");
    clientLen = sizeof(struct sockaddr_in);
    // accept connection from an incoming client
    sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&clientLen);
    if (sock < 0)
    {
      perror("accept failed");
      return 1;
    }
    printf("Connection accepted\n");
    memset(client_message, '\0', sizeof client_message);
    memset(message, '\0', sizeof message);

    playerId++;
    createConfigFile(playerId);
    printf("The player with the id : %d is on \n", playerId);
    if ((childpid = fork()) == 0)
    {
      while (1)
      {
        // parsed_json = json_object_from_file("send.json");

        writeFromFile(message, "send.json");
        printf("Message : %s\n", message);
        // Send some data
        if (send(sock, message, strlen(message), 0) < 0)
        {
          printf("Send failed");
          return 1;
        }
        bzero(message, SIZE);

        // Receive a reply from the client
        if (recv(sock, client_message, SIZE, 0) < 0)
        {
          printf("recv failed");
          break;
        }
        // printf("Client reply : %s\n", client_message);
        parsed_json = json_tokener_parse(client_message);
        json_object_object_get_ex(parsed_json, "needUpdate", &jneedUpdate);
        needUpdate = json_object_get_int(jneedUpdate);
        if(needUpdate == 1){
          json_object_to_file("send.json",parsed_json);
        }
        bzero(client_message, SIZE);

        sleep(2);
      }
    }
    close(sock);
    sleep(1);
  }
  return 0;
}