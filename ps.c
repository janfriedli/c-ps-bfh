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

char *getStatusFilePath(char *name, char *dir)
{
  char *filePath = malloc(12);
  strcpy(filePath, dir);
  strcat(filePath, "/");
  strcat(filePath, name);
  strcat(filePath, "/status");

  return filePath;
}

int checkForRssLine(char *line, char *name)
{
  int value = -1;
  if(strstr(line, name) != NULL) {
    removeSubstring(line, name);
    removeSubstring(line, "kB");
    value = atoi(trimwhitespace(line));
  }

  return value;
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

              FILE *fp;
              char buff[255];
              char *path = getStatusFilePath(entry->d_name, dir);
              fp = fopen(path, "r");
              free(path);

              if(fp == NULL)
              {
                  printf("Error opening file\n");
                  exit(1);
              }
              int rssAnon = 0;
              int rssFile = 0;
              int rssShmem = 0;
              char command[256] = "";

              // iterate over each line of the file
              while (fgets(buff, 255, (FILE*)fp) != NULL) {

                if(strstr(buff, "Name:") != NULL) {
                  removeSubstring(buff, "Name:");
                  strcat(command,trimwhitespace(buff));
                }

                if(strstr(buff, "State:") != NULL) {
                  if(strstr(buff, "Z (zombie)") != NULL) {
                    strncpy(command, command,10);
                    command[5] = 0;
                    strcat(command, " <defunct>");
                  }
                }

                int tmpAnon = checkForRssLine(buff, "RssAnon:");

                if (tmpAnon > -1) {
                    rssAnon = tmpAnon;
                }

                int tmpRssFile = checkForRssLine(buff, "RssFile:");

                if (tmpRssFile > -1) {
                    rssFile = tmpRssFile;
                }

                int tmpShmem = checkForRssLine(buff, "RssShmem:");

                if (tmpShmem > -1) {
                    rssShmem = tmpShmem;
                }
              }

              // command name
              printf("%s ", command);
              // Rss
              printf("%d\n", rssAnon + rssFile + rssShmem);
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
