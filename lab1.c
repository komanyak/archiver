#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
/*Функция получения размера файла*/
off_t fsize(const char *filename)
{
	off_t _file_size;    
	struct stat _fileStatbuff;
	int fd = open(filename, O_RDONLY);
	if(fd == -1)
	{
		printf("Ошибка при открытии файла %s\n", filename);
		return -1;
	}
	if ((fstat(fd, &_fileStatbuff) != 0) || (!S_ISREG(_fileStatbuff.st_mode))) 
	{
		printf("Ошибка при получении информации о файле %s\n", filename);
		return -1;
	}
	else
	{
		_file_size = _fileStatbuff.st_size;
	}
	close(fd);
	return _file_size;
}
/*Процедура записи в начало информационной части название папки*/
void dir_info(char* path)
{
	FILE* info = fopen("info.txt","a");
	char separator = '|';
	fputc(separator, info);
	fprintf(info,"%s", path);
	fputc(separator, info);
	fclose(info);
}
/*Процедура записи размера и названия файла в информационную часть*/
void file_info(char* pwd,char* path, char* filename, int depth, int dir)
{
	char pwd_file_info[1024];
	strcpy(pwd_file_info, pwd);
	strcat(pwd_file_info, "/info.txt");
	struct stat statbuf;
	off_t size;
	lstat(path, &statbuf);
	FILE* info = fopen(pwd_file_info,"a");
	char separator = '|';
	if(info == NULL && !S_ISDIR(statbuf.st_mode))
	{
		printf("Ошибка при открытии файла\n");
		return;
	}
	fprintf(info, "%ld", depth);				//Уровень иерархии
	fputc(separator,info);
	fprintf(info, "%ld", dir);					//Флаг папки или файла
	fputc(separator,info);
	if(dir == 1)							//Папка - название папки
	{
		fprintf(info,"%s",filename);
		fputc(separator,info);
	}
	if(dir == 0)						//Файл - размер файла и название
	{
		if((size = fsize(path)) == -1)
			return;
		fprintf(info, "%ld", size);
		fputc(separator,info);
		fprintf(info, "%s", filename);
		fputc(separator,info);
	}
	fclose(info);
}
/*Функция обхода папки и записи в информационную часть необходимые данные*/
int get_file_info(char* pwd,char* argv,int depth, int flag)
{
	struct dirent *pDirent;
	DIR *pDir;
	struct stat statbuf;
	pDir = opendir(argv);
	if(pDir == NULL)
	{
		printf("Cannot open directory '%s'\n", argv);
		return 1;
	}
	//Получить название папки
	if(flag == 1)
		dir_info(argv);
	chdir(argv);
    	while ((pDirent = readdir(pDir)) != NULL) 
	{
		lstat(pDirent->d_name, &statbuf);
		if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)  			continue;
		char BufStr[1024];
		getcwd(BufStr,sizeof(BufStr));
        if(strcmp(BufStr, pwd) == 0)
			continue;
		if(S_ISDIR(statbuf.st_mode))
		{
			file_info(pwd,BufStr,pDirent->d_name,depth,1);
			get_file_info(pwd,pDirent->d_name, depth + 1,0);
		}
		else
		{
			strcat(BufStr, "/");
			strcat(BufStr, pDirent->d_name);
			printf("[*] file: \t%s\n", BufStr);
			file_info(pwd,BufStr,pDirent->d_name,depth,0);
		}
    	}
	chdir("..");
	closedir (pDir);
	return 0;
}
/*Процедура создания файла*/
void create_file_paker(char* pwd_file_packer,char* argv,FILE* f, FILE* final)
{
	struct dirent *pDirent;
	DIR *pDir;
	struct stat statbuf;
	pDir = opendir(argv);
	char ch;
	chdir(argv);
    	while ((pDirent = readdir(pDir)) != NULL) 
	{
		lstat(pDirent->d_name, &statbuf);
		if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0) 			continue;
		char BufStr[1024];
		getcwd(BufStr,sizeof(BufStr));
		if(S_ISDIR(statbuf.st_mode))
			create_file_paker(pwd_file_packer, pDirent->d_name,f,final);
		else
		{
			strcat(BufStr, "/");
			strcat(BufStr, pDirent->d_name);
			f = fopen(BufStr, "rb");
			final = fopen(pwd_file_packer, "ab");
			while((ch = getc(f))!= EOF)
			{
				putc(ch,final);
			}
			fclose(final);
			fclose(f);
	}
    }
chdir("..");
closedir(pDir);
}
/*Функция создания файла-архива*/
int packer(char* pwd, char* argv,int flag)
{
	char pwd_file_packer[1024], pwd_file_info[1024];
	strcpy(pwd_file_packer, pwd);
	strcpy(pwd_file_info, pwd);
	strcat(pwd_file_info, "/info.txt");
	strcat(pwd_file_packer, "/file.paker");
	get_file_info(pwd,argv, 0, 1);
	char ch;
    	FILE* info = fopen(pwd_file_info,"rb");
	FILE* f;
	FILE* final = fopen(pwd_file_packer, "ab");
    	if(info == NULL || final == NULL)
	{
		printf("Ошибка при открытии/создании файла\n");
		return 1;
	}
	int info_fsize = fsize(pwd_file_info);
	fprintf(final, "%ld", fsize(pwd_file_info));
	char* buf_info = (char*)malloc(info_fsize * sizeof(char));
	if(buf_info == NULL)
	{
		printf("Не удалось выделить память\n");
		return 1;
	}
	fread(buf_info, info_fsize * sizeof(char),1,info);
	fwrite(buf_info, info_fsize * sizeof(char),1,final);
    	if(remove("info.txt") == -1)
	{
		printf("Не удалось удалить info.txt");
		return 1;
	}
	fclose(info);
	fclose(final);
	free(buf_info);
	create_file_paker(pwd_file_packer,argv,f,final);
    	return 0;
}