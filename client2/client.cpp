#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "Chess/play.h"
#include "fileFunctions.h"

#define SIZE 1024
// Create a Socket for server communication

short SocketCreate(void)
{
 short hSocket;
 printf("Create the socket\n");
 hSocket = socket(AF_INET, SOCK_STREAM, 0);
 return hSocket;
}
// try to connect with server
int SocketConnect(int hSocket)
{
 int iRetval = -1;
 int ServerPort = 90190;
 struct sockaddr_in remote = {0};
 remote.sin_addr.s_addr = inet_addr("127.0.0.1"); // Local Host
 remote.sin_family = AF_INET;
 remote.sin_port = htons(ServerPort);
 iRetval = connect(hSocket, (struct sockaddr *)&remote, sizeof(struct sockaddr_in));
 return iRetval;
}
// Send the data to the server and set the timeout of 20 seconds
int SocketSend(int hSocket, char *Rqst, short lenRqst)
{
 int shortRetval = -1;
 shortRetval = send(hSocket, Rqst, lenRqst, 0);
 return shortRetval;
}
// receive the data from the server
int SocketReceive(int hSocket, char *Rsp, short RvcSize)
{
 int shortRetval = -1;
 shortRetval = recv(hSocket, Rsp, RvcSize, 0);
 // printf("Response %s\n", Rsp);
 return shortRetval;
}
// main driver program
int main(int argc, char *argv[])
{
 int hSocket, read_size;
 struct sockaddr_in server;
 char SendToServer[SIZE] = {0};
 char server_reply[SIZE] = {0};
 // Create socket
 hSocket = SocketCreate();
 if (hSocket == -1)
 {
  printf("Could not create socket\n");
  return 1;
 }
 printf("Socket is created\n");
 // Connect to remote server
 if (SocketConnect(hSocket) < 0)
 {
  perror("connect failed.\n");
  return 1;
 }
 printf("Sucessfully conected with server\n");

 pid_t childpid;
 json_object *parsed_json;
 json_object *jturn;
 json_object *jplayerId;
 json_object *jneedUpdate;
 int turn;
 int playerId;
 int it = 0;

 if ((childpid = fork()) == 0)
 {
  while (1)
  {
   if (it == 0)
   {
    sleep(2);
    it++;
   }
   parsed_json = json_object_from_file("send.json");
   json_object_object_get_ex(parsed_json, "turn", &jturn);
   turn = json_object_get_int(jturn);

   jplayerId = json_object_from_file("id.json");
   playerId = json_object_get_int(jplayerId);


   read_size = SocketReceive(hSocket, server_reply, SIZE);
   writeToFile(server_reply, "recv.json");
   bzero(server_reply, SIZE);

   writeFromFile(SendToServer, "send.json");
   SocketSend(hSocket, SendToServer, strlen(SendToServer));
   bzero(SendToServer, SIZE);
   sleep(1);
  }
  close(hSocket);
  shutdown(hSocket, 0);
  shutdown(hSocket, 1);
  shutdown(hSocket, 2);
 }
 else
 {

  read_size = SocketReceive(hSocket, server_reply, SIZE);
  printf("Server Response : %s\n\n", server_reply);
  writeToFile(server_reply, "recv.json");
  bzero(server_reply, SIZE);

  parsed_json = json_object_from_file("recv.json");
  json_object_object_get_ex(parsed_json, "playerId", &jplayerId);
  json_object_to_file("id.json", jplayerId);

  jneedUpdate = json_object_new_int(0);
  json_object_object_add(parsed_json, "needUpdate", jneedUpdate);
  json_object_to_file("send.json", parsed_json);
  json_object_to_file("recv.json", parsed_json);

  // printf("The turn is : %d\n", turn);
  // printf("Enter the Message: ");
  // scanf("%s", SendToServer);
  // Send data to the server

  jneedUpdate = json_object_new_int(0);
  json_object_object_add(parsed_json, "needUpdate", jneedUpdate);
  json_object_to_file("send.json", parsed_json);
  json_object_to_file("recv.json", parsed_json);

  writeFromFile(server_reply, "send.json");
  SocketSend(hSocket, server_reply, strlen(server_reply));
  bzero(server_reply, SIZE);

  play p(json_object_get_int(jplayerId), parsed_json);
 }
 return 0;
}