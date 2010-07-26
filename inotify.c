#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/inotify.h>
#include <signal.h>
#include <dirent.h>

#include "callback.h"

#define DEPTH 2

int listenTo(char * path)
{
	int fd_inotify = inotify_init();
	if(fd_inotify < 0)
	{
		perror("Error : inotify_init");
		return EXIT_FAILURE;
	}

	int watch = inotify_add_watch(fd_inotify, path , IN_CREATE|IN_DELETE);
	if(watch < 0)
	{
		perror("Error : inotify_add_watch");
		return EXIT_FAILURE;
	}
    
    addWatches(fd_inotify, path);
  
	fd_set fds;
  
	for(;;)
	{
		FD_ZERO(&fds);
		FD_SET(fd_inotify, &fds);
		if(select(fd_inotify+1, &fds, NULL, NULL, 0) <= 0) continue;
    
		size_t carlu;
		char buffer[2048];
		struct inotify_event * event_inotify;
    
		carlu = read(fd_inotify, buffer, sizeof(buffer));
		if(carlu <= 0)
		{
			perror("Error : read");
			return EXIT_FAILURE;
		}
    
		event_inotify = (struct inotify_event *)buffer;
    
		if(event_inotify->len && (event_inotify->mask & IN_DELETE))
		{
			onDeleteFile(event_inotify->name);
		}
		else if(event_inotify->len && (event_inotify->mask & IN_CREATE))
		{
			onCreateFile(event_inotify->name);
		}
	}
    return 0;
}

int addWatches(int fd_inotify, char * path)
{
    DIR * folder;
    if((folder = opendir(path)) < 0)
    {
        perror("Error : opendir(path)");
        return EXIT_FAILURE;
    }

    char * fullpath;
    struct dirent * item;
    while((item = readdir(folder)) != NULL)
    {
        if(item->d_type == DT_DIR && item->d_name[0] != '.')
        {
            fullpath = calloc(
                strlen(path) + strlen("/") + strlen(item->d_name) + 1,
                sizeof(char));
            strcat(fullpath, path);
            strcat(fullpath, "/");
            strcat(fullpath,item->d_name);
            printf("fullpath : %s\n",fullpath);
	        int watch = inotify_add_watch(fd_inotify,
                    fullpath, IN_DELETE|IN_CREATE);
        	if(watch < 0)
	        {
        		perror("Error : inotify_add_watch");
        		return EXIT_FAILURE;
        	}    
            free(fullpath);
        }
    }
}
