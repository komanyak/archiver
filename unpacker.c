#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_FILENAME_LEN 1024

// Функция извлечения файлов из архива
void extract(const char *archiveFile);

// Функция создания директории
void create_dir(const char *dirPath);

// Функция создания файла
void create_file(FILE *archive, const char *filename, off_t size);

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Использование: %s <archive_file>\n", argv[0]);
    return 1;
  }

  // Вызов функции извлечения файлов из архива
  extract(argv[1]);

  return 0;
}

void extract(const char *archiveFile)
{
  FILE *archive = fopen(archiveFile, "rb");
  if (archive == NULL)
  {
    perror("Ошибка при открытии файла архива");
    exit(EXIT_FAILURE);
  }

  char ch;
  char filename[MAX_FILENAME_LEN];
  off_t size;

  // Чтение архива до конца
  while ((ch = fgetc(archive)) != EOF)
  {
    // Возвращение символа обратно в поток
    ungetc(ch, archive);

    // Чтение имени файла из архива
    fscanf(archive, "|%[^|]|", filename);

    // Создание директории, если ее нет
    create_dir(filename);

    // Чтение информации о файлах в архиве
    while (fscanf(archive, "%[^|]|", filename) != EOF)
    {
      // Преобразование строки в число (размер файла)
      size = atoi(filename);

      // Если размер равен 1, то это директория, иначе - файл
      if (size == 1)
      {
        create_dir(filename);
      }
      else
      {
        // Создание файла и запись в него данных из архива
        create_file(archive, filename, size);
      }
    }
  }

  // Закрытие файла архива
  fclose(archive);
}

void create_dir(const char *dirPath)
{
  struct stat st = {0};

  // Проверка существования директории, и создание, если ее нет
  if (stat(dirPath, &st) == -1)
  {
    mkdir(dirPath, 0777);
  }
}

void create_file(FILE *archive, const char *filename, off_t size)
{
  char fullpath[MAX_FILENAME_LEN];
  snprintf(fullpath, sizeof(fullpath), "%s", filename);

  FILE *file = fopen(fullpath, "wb");
  if (file == NULL)
  {
    perror("Ошибка при создании файла");
    exit(EXIT_FAILURE);
  }

  char ch;
  off_t i;

  // Запись данных из архива в созданный файл
  for (i = 0; i < size; ++i)
  {
    ch = fgetc(archive);
    fputc(ch, file);
  }

  // Закрытие файла
  fclose(file);
}
