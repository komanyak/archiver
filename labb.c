#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define MAX_DEPTH 100

// Структура для хранения информации о директории
typedef struct
{
  char name[256];
  int depth;
} DirectoryInfo;

// Функция для создания информационной части архива
void make_info(FILE *out, const char *dir);

// Функция для восстановления структуры файлов и папок из информационной части
void restore_structure(FILE *info, const char *rootPath);

int main()
{
  const char *dirname = "/home/komanyak/Документы/OS_Lab/Lab1/archiver/test/";
  const char *infoFile = "out.txt";            // Путь к файлу с информационной частью
  const char *rootPath = "restored_structure"; // Корневая папка для восстановления структуры

  FILE *out = fopen(infoFile, "w");
  if (!out)
  {
    perror("Failed to create information file");
    exit(EXIT_FAILURE);
  }

  make_info(out, dirname);
  fclose(out);

  FILE *info = fopen(infoFile, "r");
  if (!info)
  {
    perror("Failed to open information file");
    exit(EXIT_FAILURE);
  }

  restore_structure(info, rootPath);
  fclose(info);

  return 0;
}

// Функция для создания информационной части архива
void make_info(FILE *out, const char *dir)
{
  DIR *dp;
  struct dirent *entry;
  struct stat statbuf;

  DirectoryInfo stack[MAX_DEPTH];
  int stack_size = 0;

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
      if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
        continue;

      // Записываем информацию о директории в информационную часть
      fprintf(out, "1|%ld|%d|%s|\n", statbuf.st_size, stack_size, entry->d_name);

      // Добавляем директорию в стек
      strncpy(stack[stack_size].name, entry->d_name, sizeof(stack[stack_size].name));
      stack[stack_size].depth = stack_size;
      stack_size++;
    }
    else
    {
      // Записываем информацию о файле в информационную часть
      fprintf(out, "0|%ld|%d|%s|\n", statbuf.st_size, stack_size, entry->d_name);
    }
  }

  closedir(dp);

  // Обработка поддиректорий
  for (int i = 0; i < stack_size; i++)
  {
    // Поднимаемся на уровень выше
    chdir("..");

    // Переходим в директорию из стека
    chdir(stack[i].name);

    // Рекурсивно обрабатываем содержимое директории
    make_info(out, ".");
  }
}

// Функция для восстановления структуры файлов и папок из информационной части

void restoreStructure(const char *infoFile, const char *rootPath)
{
  FILE *info = fopen(infoFile, "r");
  if (!info)
  {
    perror("Failed to open information file");
    exit(EXIT_FAILURE);
  }

  char line[256];
  char currentPath[256] = ""; // Путь к текущей директории

  while (fgets(line, sizeof(line), info))
  {
    int isDir, fileSize, depth;
    char itemName[256];

    // Разбор строки информации
    if (sscanf(line, "%d|%d|%d|%255[^\n]", &isDir, &fileSize, &depth, itemName) != 4)
    {
      fprintf(stderr, "Invalid format in information file\n");
      exit(EXIT_FAILURE);
    }

    // Обновление текущего пути в соответствии с уровнем вложенности
    while (depth > 0)
    {
      char *lastSlash = strrchr(currentPath, '/');
      if (lastSlash)
      {
        *lastSlash = '\0'; // Убираем последний слэш
      }
      else
      {
        break; // Достигнут корень, прерываем цикл
      }
      depth--;
    }

    // Построение пути элемента
    char itemPath[256];
    snprintf(itemPath, sizeof(itemPath), "%s/%s", currentPath, itemName);

    // Создание папки или файла
    if (isDir)
    {
      printf("Creating directory: %s\n", itemPath);
      mkdir(itemPath, 0755);
      strcat(currentPath, "/");
      strcat(currentPath, itemName); // Обновляем текущий путь
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

      // Чтение содержимого файла и запись в соответствии с fileSize
      char buffer[1024];
      int bytesRead;
      while (fileSize > 0)
      {
        int bytesToRead = (fileSize < sizeof(buffer)) ? fileSize : sizeof(buffer);
        bytesRead = fread(buffer, 1, bytesToRead, info);
        if (bytesRead < 0)
        {
          perror("Failed to read from file");
          fclose(file);
          exit(EXIT_FAILURE);
        }
        if (fwrite(buffer, 1, bytesRead, file) != bytesRead)
        {
          perror("Failed to write to file");
          fclose(file);
          exit(EXIT_FAILURE);
        }
        fileSize -= bytesRead;
      }

      fclose(file);
    }
  }

  fclose(info);
}

int main()
{
  const char *infoFile = "out.txt";            // Путь к файлу с информационной частью
  const char *rootPath = "restored_structure"; // Корневая папка для восстановления структуры

  restoreStructure(infoFile, rootPath);

  return 0;
}