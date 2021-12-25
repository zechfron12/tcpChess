#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void writeToFile(char *message, const char *fileName)
{
  fclose(fopen(fileName, "w"));
  FILE *recvFile = fopen(fileName, "w");
  int results = fputs(message, recvFile);

  if (results == EOF)
  {
    perror("Failed to write do error.\n");
  }
  fclose(recvFile);
}

void writeFromFile(char *message, const char *fileName)
{
  FILE *pFile;
  long lSize;
  char *buffer;
  size_t result;

  pFile = fopen(fileName, "r");
  if (pFile == NULL)
  {
    fputs("File error", stderr);
    exit(1);
  }

  // obtain file size:
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);

  // allocate memory to contain the whole file:
  buffer = (char *)malloc(sizeof(char) * lSize);
  if (buffer == NULL)
  {
    fputs("Memory error", stderr);
    exit(2);
  }

  // copy the file into the buffer:
  result = fread(buffer, 1, lSize, pFile);
  if (result != lSize)
  {
    fputs("Reading error", stderr);
    exit(3);
  }

  strcpy(message, buffer);

  // terminate
  fclose(pFile);
  free(buffer);
}