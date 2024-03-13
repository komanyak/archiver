#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_FILENAME_LEN 1024

// Функция для создания архива
void archive(const char *sourceDir, const char *archiveFile);

// Функция для записи информации о директории в файл info.txt
void dir_info(FILE *info, const char *path);

// Функция для записи информации о файле или директории в файл info.txt
void file_info(FILE *info, const char *path, const char *filename, off_t depth, int isDir);

// Функция для создания архивного файла на основе файла info.txt
void create_archive(const char *archiveFile, FILE *info);

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "Использование: %s <source_directory> <archive_file>\n", argv[0]);
    return 1;
  }

  // Запуск архиватора
  archive(argv[1], argv[2]);

  return 0;
}

// Функция archive объединяет функциональность архивации в одну
void archive(const char *sourceDir, const char *archiveFile)
{
  FILE *info = fopen("info.txt", "w"); // Открытие файла info.txt для записи
  if (info == NULL)
  {
    perror("Ошибка при открытии файла info.txt");
    exit(EXIT_FAILURE);
  }

  // Запись информации о директории в файл info.txt
  dir_info(info, sourceDir);
  fclose(info); // Закрытие файла info.txt

  // Создание архивного файла на основе файла info.txt
  create_archive(archiveFile, info);

  // Удаление временного файла info.txt
  remove("info.txt");
}

// Функция для записи информации о директории в файл info.txt
void dir_info(FILE *info, const char *path)
{
  fprintf(info, "|%s|", path); // Запись информации о директории в файл

  struct dirent *pDirent;
  DIR *pDir;
  struct stat statbuf;

  pDir = opendir(path); // Открытие директории
  if (pDir == NULL)
  {
    perror("Не удалось открыть директорию");
    exit(EXIT_FAILURE);
  }

  chdir(path); // Изменение текущей директории на указанную

  while ((pDirent = readdir(pDir)) != NULL)
  { // Обход содержимого директории
    lstat(pDirent->d_name, &statbuf);
    if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)
      continue;

    // Запись информации о файле или директории в файл info.txt
    if (S_ISDIR(statbuf.st_mode))
    {
      file_info(info, path, pDirent->d_name, 0, 1);
      dir_info(info, pDirent->d_name); // Рекурсивный вызов для поддиректории
    }
    else
    {
      file_info(info, path, pDirent->d_name, 0, 0);
    }
  }

  chdir("..");    // Возврат на уровень выше
  closedir(pDir); // Закрытие директории
}

// Функция для записи информации о файле или директории в файл info.txt
void file_info(FILE *info, const char *path, const char *filename, off_t depth, int isDir)
{
  fprintf(info, "%ld|%d|%s|", depth, isDir, filename); // Запись информации в файл

  if (!isDir)
  {
    char fullpath[MAX_FILENAME_LEN];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, filename);

    struct stat statbuf;
    lstat(fullpath, &statbuf);

    fprintf(info, "%ld|", statbuf.st_size); // Запись размера файла
  }
}

// Функция для создания архивного файла на основе файла info.txt
void create_archive(const char *archiveFile, FILE *info)
{
  FILE *archive = fopen(archiveFile, "wb"); // Открытие архивного файла для записи
  if (archive == NULL)
  {
    perror("Ошибка при открытии файла архива");
    exit(EXIT_FAILURE);
  }

  fseek(info, 0, SEEK_SET); // Установка указателя файла info.txt в начало
  char ch;
  while ((ch = fgetc(info)) != EOF)
  {
    fputc(ch, archive); // Копирование содержимого файла info.txt в архив
  }

  fclose(archive); // Закрытие архивного файла
}
