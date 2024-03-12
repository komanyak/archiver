#include <stdio.h>
#include <stdlib.h>

void extractFiles(FILE *archive)
{
  long fileSize;

  while (fread(&fileSize, sizeof(long), 1, archive) == 1)
  {
    char buffer[1024];
    size_t bytesRead;

    fread(buffer, 1, fileSize, archive);

    FILE *extractedFile = fopen("extracted_file.txt", "wb");
    if (extractedFile == NULL)
    {
      perror("Error creating extracted file");
      exit(EXIT_FAILURE);
    }

    fwrite(buffer, 1, fileSize, extractedFile);
    fclose(extractedFile);
  }
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <archive_name>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  FILE *archive = fopen(argv[1], "rb");
  if (archive == NULL)
  {
    perror("Error opening archive");
    exit(EXIT_FAILURE);
  }

  extractFiles(archive);

  fclose(archive);
  printf("Files extracted successfully.\n");

  return 0;
}
