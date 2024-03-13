#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>



// char block[1024];
// int in, out;
// int nread;
// in = open("file.in", O_RDONLY);
// out = open("file.out", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
// while((nread = read(in, block, sizeof(block))) > 0)
//  write(out, block, nread);
// exit(0);

// int c;
// FILE *in, *out;
// in = fopen("file.in", "r");
// out = fopen("file.out", "w");
// while((c = fgetc(in)) != EOF) fputc(c, out);
// exit(0);

void make_info(FILE *out, char *dir, int depth)
{
  DIR *dp;
  struct dirent *entry;
  struct stat statbuf;
  
  if ((dp = opendir(dir)) == NULL)
  {

    fprintf(stderr, "cannot open directory: %s\n", dir);
    return;
  }

  chdir(dir);

  while ((entry = readdir(dp)) != NULL)
  {

    lstat(entry->d_name, &statbuf);

    if (S_ISDIR(statbuf.st_mode))
    {
      // записать в out "isDir(0 или 1)|dir_size|dir_name|"
      if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
        continue;
     
      fprintf(out, "1|%ld|%ld|%s|\n", statbuf.st_size, depth, entry->d_name);
    
      printf("%*s%s/\n", depth*4+1, "", entry->d_name);
      //printf("1|%ld|%ld|%s|\n", statbuf.st_size, depth, entry->d_name);
      make_info(out, entry->d_name, depth + 1);
    }
    else
    {
      if(strcmp(strrchr(entry->d_name, '.'), ".gg") == 0)
      { 
        // arch_func
        printf("%*s%s is a TXT file\n", depth*4+1, "", entry->d_name);
      }

      // записать в out "isDir(0 или 1)|file_size|file_name|"
      printf("%*s%s\n", depth*4+1, "", entry->d_name);
      // printf("0|%ld|%ld|%s|\n", statbuf.st_size, depth, entry->d_name);

      fprintf(out, "0|%ld|%ld|%s|\n", statbuf.st_size, depth, entry->d_name);

    }
  }

  chdir("..");
  closedir(dp);
}

int main()
{

  char dirname[] = "/home/komanyak/Документы/OS_Lab/Lab1/archiver/test/";
  FILE *out;
  out = fopen("1.txt", "ab");
  // Обзор каталога /home
  printf("\nDirectory scan of %s:\n", dirname);
  make_info(out ,dirname, 0);
  printf("done.\n");

  exit(0);
}


