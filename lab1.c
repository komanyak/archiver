#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// Структура для хранения информации о файлах
struct FileInfo {
    char name[256];
    size_t size;
};

// Рекурсивная функция для архивации файлов
void archiveDirectory(FILE *archive, const char *dirname) {
    DIR *dir;
    struct dirent *entry;
    struct stat info;

    // Открываем директорию для чтения
    dir = opendir(dirname);
    if (!dir) {
        perror("opendir");
        return;
    }

    // Перебираем содержимое директории
    while ((entry = readdir(dir)) != NULL) {
        char path[256];
        // Формируем полный путь к файлу или директории
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        // Пропускаем "." и ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Получаем информацию о файле
        if (lstat(path, &info) == 0) {
            if (S_ISDIR(info.st_mode)) {
                // Если это директория, рекурсивно архивируем ее содержимое
                archiveDirectory(archive, path);
            } else if (S_ISREG(info.st_mode)) {
                // Если это файл, архивируем его
                struct FileInfo fileinfo;
                memset(&fileinfo, 0, sizeof(struct FileInfo));
                strncpy(fileinfo.name, entry->d_name, sizeof(fileinfo.name) - 1);
                fileinfo.size = info.st_size;

                // Записываем информацию о файле в архив
                fwrite(&fileinfo, sizeof(struct FileInfo), 1, archive);

                // Записываем содержимое файла в архив
                FILE *file = fopen(path, "rb");
                if (file) {
                    char buffer[1024];
                    size_t bytesRead;
                    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                        fwrite(buffer, 1, bytesRead, archive);
                    }
                    fclose(file);
                }
            }
        }
    }

    // Закрываем директорию
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <папка для архивации> <архив>\n", argv[0]);
        return 1;
    }

    const char *dirname = argv[1];
    const char *archiveName = argv[2];

    // Открываем файл архива для записи
    FILE *archive = fopen(archiveName, "wb");
    if (!archive) {
        perror("fopen");
        return 1;
    }

    // Архивируем содержимое папки
    archiveDirectory(archive, dirname);

    // Закрываем файл архива
    fclose(archive);

    printf("Архивация завершена: %s -> %s\n", dirname, archiveName);

    return 0;
}
