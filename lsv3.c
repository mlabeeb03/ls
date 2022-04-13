//	v1	recieves none or multiple dir names
//	v2	do not show hidden files
//	v3 	long listing

#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

int longList;

void showLongList(char * fileName){
	printf("abc");
	struct stat info;
	int rv = lstat(fileName, &info);
	if(rv == -1){
		perror("stat failed");
		exit(1);
	}

	printf("%o\t", info.st_mode);
    printf("%ld\t", info.st_nlink);
    printf("%d\t", info.st_uid);
    printf("%d\t", info.st_gid);
    printf("%ld\t", info.st_size);
    printf("%ld\t", info.st_mtime);
    printf("%s\n", fileName);
}

void do_ls(char * dir){
	struct dirent * entry;
	DIR * dp = opendir(dir);
	if(dp == NULL){
		fprintf(stderr, "Can not open directory:%s\n", dir);
		return;
	}
	errno = 0;
	while((entry = readdir(dp)) != NULL){
		if(entry == NULL && errno != 0){
			perror("readdir failed");
			exit(errno);
		}
		else{
			if(entry->d_name[0] == '.')
				continue;
			if(longList)
				showLongList(dir);
			else
				printf("%s\n", entry->d_name);
		}
	}
	closedir(dp);
}

int main(int argc, char* argv[]){
	if(argc == 1)
		do_ls(".");
	else{
		if(!strcmp(argv[1], "-l")){
			longList = 1;

			if(argc == 2)
				do_ls(".");
			else{
				int i = 1;
				while(++i < argc){
				printf("Directory listing of %s:\n", argv[i]);
				do_ls(argv[i]);
				}	
			}

			longList = 0;
		}
		else{
			int i = 0;
			while(++i < argc){
				printf("Directory listing of %s:\n", argv[i]);
				do_ls(argv[i]);
			}	
		}
	}	
	return 0;
}
