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

      printf("%*s%s/\n", depth * 4 + 1, "", entry->d_name);
      // printf("1|%ld|%ld|%s|\n", statbuf.st_size, depth, entry->d_name);
      make_info(out, entry->d_name, depth + 1);
    }
    else
    {
      const char *extension = strrchr(entry->d_name, '.');
      if (extension && strcmp(extension, ".gg") == 0)
      {
        // Это файл с расширением .gg, обработайте его соответствующим образом
        printf("%*s%s is a GG file\n", depth * 4 + 1, "", entry->d_name);
      }

      // записать в out "isDir(0 или 1)|file_size|file_name|"
      printf("%*s%s\n", depth * 4 + 1, "", entry->d_name);
      // printf("0|%ld|%ld|%s|\n", statbuf.st_size, depth, entry->d_name);

      fprintf(out, "0|%ld|%ld|%s|\n", statbuf.st_size, depth, entry->d_name);
    }
  }

  chdir("..");
  closedir(dp);
}

void restoreStructure(const char *infoFile, const char *rootPath)
{
  FILE *info = fopen(infoFile, "r");
  if (!info)
  {
    perror("Failed to open information file");
    exit(EXIT_FAILURE);
  }
  int currentDepth = 0;
  char line[256];
  while (fgets(line, sizeof(line), info))
  {
    int isDir, fileSize, depth;
    char itemName[256];

    // Разбор строки информации
    if (sscanf(line, "%d|%d|%d|%255[^|\n]", &isDir, &fileSize, &depth, itemName) != 4)
    {
      fprintf(stderr, "Invalid format in information file\n");
      exit(EXIT_FAILURE);
    }

    // Построение пути элемента
    char itemPath[256];
    snprintf(itemPath, sizeof(itemPath), "%s/%s", rootPath, itemName);

    // Создание папки или файла
    if (isDir)
    {
      printf("Creating directory: %s\n", itemPath);
      mkdir(itemPath, 0755);
    }
    else
    {
      printf("Creating file: %s\n", itemPath);
      FILE *file = fopen(itemPath, "wb");
      if (!file)
      {
        perror("Failed to create file");
        exit(EXIT_FAILURE);
      }

      // Здесь можно добавить чтение содержимого файла и запись в соответствии с fileSize
      // ...

      fclose(file);
    }
  }

  fclose(info);
}

int main()
{

  char dirname[] = "/home/komanyak/Документы/OS_Lab/Lab1/archiver/test/";
  FILE *out;
  out = fopen("out.txt", "w");
  // Обзор каталога /home
  printf("\nDirectory scan of %s:\n", dirname);
  make_info(out, dirname, 0);
  printf("done.\n");

  const char *infoFile = "/home/komanyak/Документы/OS_Lab/Lab1/archiver/out.txt";            // Путь к файлу с информационной частью
  const char *rootPath = "/home/komanyak/Документы/OS_Lab/Lab1/archiver/unpack"; // Корневая папка для восстановления структуры

  restoreStructure(infoFile, rootPath);

  exit(0);
}
