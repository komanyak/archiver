#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

void printdir(char *dir, int depth)
{
  DIR *dp;              // Указатель на структуру DIR, представляющую поток директории
  struct dirent *entry; // Указатель на структуру dirent, представляющую информацию о файле/директории
  struct stat statbuf;  // Структура для хранения информации о файле/директории

  if ((dp = opendir(dir)) == NULL)
  {
    // Попытка открыть директорию
    fprintf(stderr, "cannot open directory: %s\n", dir);
    return;
  }

  chdir(dir); // Смена текущей директории на указанную

  while ((entry = readdir(dp)) != NULL)
  {
    // Чтение следующего элемента из потока директории
    lstat(entry->d_name, &statbuf); // Получение информации о файле/директории

    if (S_ISDIR(statbuf.st_mode))
    {
      // Если это директория
      // Находит каталог, но игнорирует . и ..
      if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
        continue;

      printf("%*s%s/\n", depth, "", entry->d_name); // Вывод названия директории с отступом
      // Рекурсивный вызов с новым отступом
      printdir(entry->d_name, depth + 4);
    }
    else
    {
      // Если это файл
      printf("%*s%s\n", depth, " ", entry->d_name); // Вывод названия файла с отступом
    }
  }

  chdir("..");  // Возврат к предыдущей директории
  closedir(dp); // Закрытие потока директории
}

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

void archiver(char *fname, char *dir, int depth)
{
  DIR *dp;
  struct dirent *entry;
  struct stat statbuf;
  FILE *out;
  out = fopen("file.out", "w");
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

      fprintf(out, "1|%ld|%s|\n", statbuf.st_size, entry->d_name);
      // printf("%*s%s/\n", depth, "", entry->d_name);
      printf("1|%ld|%s|\n", statbuf.st_size, entry->d_name);
      archiver(out, entry->d_name, depth + 4);
    }
    else
    {

      // записать в out "isDir(0 или 1)|file_size|file_name|"
      // printf("%*s%s\n", depth, " ", entry->d_name);
      printf("0|%ld|%s|\n", statbuf.st_size, entry->d_name);
      fprintf(out, "0|%ld|%s|\n", statbuf.st_size, entry->d_name);
    }
  }

  chdir("..");
  closedir(dp);
}

int main()
{
  // Обзор каталога /home
  printf("Directory scan of /home:\n");
  printdir("/home/komanyak/Документы/OS_Lab/Lab1/archiver/", 0);
  printf("done.\n");

  exit(0);
}
