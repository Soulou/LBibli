#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sqlite3.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "inotify.h"

#define BUFFER_SIZE 1000
#define DB_NAME "LBibli.db"

int main(int argc, char ** argv)
{
    char * db_path = strcat(getenv("HOME"), "/.config/LBibli/");
    
    DIR * db_dir = opendir(db_path);
    if(db_dir == NULL)
    {
        switch(errno)
        {
            case ENOENT:
                printf("Creation du dossier de config\n");
                if(mkdir(db_path, S_IWUSR|S_IRUSR|S_IXUSR|
                                  S_IRGRP|S_IXGRP|
                                  S_IROTH|S_IXOTH) < 0)
                {
                    printf("Erreur durant la creation du dossier\
                            de confituration\n");
                    return EXIT_FAILURE;
                }
                break;
            default :
                printf("Erreur inconnue\n");
                return EXIT_FAILURE;
        }
    }
    else
    {
        printf("Dossier %s deja present.\n",db_path);
    }

    sqlite3 * db;
    int handle_db = sqlite3_open(strcat(db_path,DB_NAME),
                                 &db);
    if(handle_db == SQLITE_OK)
    {
        printf("Database créée/ouverte avec succès.\n");
    }
    else
    {
        perror(sqlite3_errmsg(db));
    }

    char * buffer = calloc(BUFFER_SIZE,sizeof(char));

    printf("Ecoute de : ");
    scanf("%s",buffer);
    char * path = calloc(strlen(buffer), sizeof(char));
    strcpy(path,buffer);

    listenTo(path);
    free(buffer);
    free(path);
}

