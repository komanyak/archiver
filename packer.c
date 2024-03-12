#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void createArchive(FILE *archive, const char *directory)
{
  DIR *dir;
  struct dirent *entry;

  dir = opendir(directory);
  if (dir == NULL)
  {
    perror("Error opening directory");
    exit(EXIT_FAILURE);
  }

  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_REG)
    {
      char filePath[256];
      sprintf(filePath, "%s/%s", directory, entry->d_name);

      FILE *file = fopen(filePath, "rb");
      if (file == NULL)
      {
        perror("Error opening file");
        exit(EXIT_FAILURE);
      }

      fseek(file, 0, SEEK_END);
      long fileSize = ftell(file);
      fseek(file, 0, SEEK_SET);

      fwrite(&fileSize, sizeof(long), 1, archive);

      char buffer[1024];
      size_t bytesRead;

      while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
      {
        fwrite(buffer, 1, bytesRead, archive);
      }

      fclose(file);
    }
    else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
      char subDirPath[256];
      sprintf(subDirPath, "%s/%s", directory, entry->d_name);

      createArchive(archive, subDirPath);
    }
  }

  closedir(dir);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s <archive_name> <directory>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  FILE *archive = fopen(argv[1], "wb");
  if (archive == NULL)
  {
    perror("Error creating archive");
    exit(EXIT_FAILURE);
  }

  createArchive(archive, argv[2]);

  fclose(archive);
  printf("Archive created successfully.\n");

  return 0;
}
