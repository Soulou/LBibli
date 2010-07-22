#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inotify.h"

#define BUFFER_SIZE 1000

int main(int argc, char ** argv)
{
    char * buffer = calloc(BUFFER_SIZE,sizeof(char));

    printf("Ecoute de : ");
    scanf("%s",buffer);
    char * path = calloc(strlen(buffer), sizeof(char));
    strcpy(path,buffer);

    listenTo(path);
    free(buffer);
    free(path);
}

