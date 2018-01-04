#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
/**
* A simple C implementation of the `ps x -o pid,comm,rss` command.
* Created by Jan Friedli
* Date 04.01.2018
*/

/**
* This function removes all the whitespace of string
* It does return it
*/
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

/**
* This function removes a substring from a given string
* It works with it by reference
*/
void removeSubstring(char *s,const char *toremove)
{
  while( s=strstr(s,toremove) )
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
}

/**
* Build the path of the status file
* It does return it
*/
char *getStatusFilePath(char *name, char *dir)
{
  char *filePath = malloc(12);
  strcpy(filePath, dir);
  strcat(filePath, "/");
  strcat(filePath, name);
  strcat(filePath, "/status");

  return filePath;
}

/**
* This function checks if  a given patten is on the current line
* and retrieves its value
* It does return it
*/
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

/**
* This function does go trough all subfolders in a given folder which have the
* the same owner as the user who is running the script.
* For every folder it gets the status file and reads the important info from it
* It does print the result to the stdout
*/
void ps(char *dir)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    // handle directory error
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

            // only handle the user owned dirs
            if (statbuf.st_uid == getuid()) {
              FILE *fp;
              char buff[256];
              char *path = getStatusFilePath(entry->d_name, dir);
              fp = fopen(path, "r");
              free(path);

              // file error handling
              if(fp == NULL)
              {
                  printf("Error opening file\n");
                  exit(1);
              }

              // init results
              int rssAnon = 0;
              int rssFile = 0;
              int rssShmem = 0;
              char command[256] = "";

              // iterate over each line of the file
              while (fgets(buff, 255, (FILE*)fp) != NULL) {
                // retrieve the name
                if(strstr(buff, "Name:") != NULL) {
                  removeSubstring(buff, "Name:");
                  strcat(command,trimwhitespace(buff));
                }

                // handle zombie processes
                if(strstr(buff, "State:") != NULL) {
                  if(strstr(buff, "Z (zombie)") != NULL) {
                    strncpy(command, command,10);
                    command[5] = 0;
                    strcat(command, " <defunct>");
                  }
                }

                // handle the rss values which need to be summed up
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

              // Print the results
              printf("%s ",entry->d_name);
              printf("%s ", command);
              printf("%d\n", rssAnon + rssFile + rssShmem);

              // finally close file
              fclose(fp);
            }
        }
    }

    // final steps
    chdir("..");
    closedir(dp);
}

/**
* Main entry point
*/
int main()
{
  ps("/proc");
  exit(0);
}
