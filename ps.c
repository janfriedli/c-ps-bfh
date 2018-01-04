#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

char *trimwhitespace(char *str)
{
  char *end;

  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  *(end+1) = 0;

  return str;
}



void removeSubstring(char *s,const char *toremove)
{
  while( s=strstr(s,toremove) )
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
}

void printdir(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    printf("PID COMMMAND RSS\n");
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
            {
              continue;
            }

            if (statbuf.st_uid == getuid()) {
              // Pid
              printf("%s ",entry->d_name);

              // Command
              char filePath[12];
              strcpy(filePath, dir);
              strcat(filePath, "/");
              strcat(filePath, entry->d_name);
              strcat(filePath, "/status");
              //printf("%s\n", filePath);

              FILE *fp;
              char buff[255];
              fp = fopen(filePath, "r");
              fgets(buff, 255, (FILE*)fp);
              removeSubstring(buff, "Name:");
              printf("%s\n", trimwhitespace(buff));
              fclose(fp);

            }
        }
    }
    chdir("..");
    closedir(dp);
}

int main()
{
  printdir("/proc",1);
  exit(0);
}
