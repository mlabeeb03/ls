//	v1	recieves none or multiple dir names
//	v2	do not show hidden files
//	v3 	long listing	
//	v4	display in columns

#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

int longList;

int columnsOnScreen;

int lenOfLongestFile;
int totalNumOfFiles;

// calculate the lenOfLongestFile and totalNumOfFiles
void setLenAndNum(char * dir){
	lenOfLongestFile = 0;
	totalNumOfFiles = 0;
	int s;
	int i;
	struct dirent * entry;
	DIR * dp = opendir(dir);
	while((entry = readdir(dp)) != NULL){
		if(entry == NULL && errno != 0){
			perror("readdir failed");
			exit(errno);
		}
		else{
			if(entry->d_name[0] == '.')
				continue;
			totalNumOfFiles++;
			s = 0;
			i = 0;
			while(entry->d_name[i++] != '\0'){
				s++;
			}
			if(s>lenOfLongestFile)
				lenOfLongestFile = s;
		}
	}
	s += 3;		//just for margin
	closedir(dp);}

void showLongList(char * fileName){
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
    printf("%s\n", fileName);}

void do_ls(char * dir){
	struct dirent * entry;
	DIR * dp = opendir(dir);
	if(dp == NULL){
		fprintf(stderr, "Can not open directory:%s\n", dir);
		return;
	}
	errno = 0;

	setLenAndNum(dir);

	char fileNames[totalNumOfFiles][lenOfLongestFile];	//array to store all file names
	int i = 0;	//index to iterate through filenames

	int numOfCols = columnsOnScreen/lenOfLongestFile;
	int numOfRows = (totalNumOfFiles + numOfCols -1)/ numOfCols;


	while((entry = readdir(dp)) != NULL){
		if(entry == NULL && errno != 0){
			perror("readdir failed");
			exit(errno);
		}
		else{
			if(entry->d_name[0] == '.')
				continue;
			if(longList)
				showLongList(entry->d_name);
			else			
				strcpy(fileNames[i++], entry->d_name);	//copying names to the filenames array
		}
	}

	int j = 0;
	while(j < totalNumOfFiles){
		printf("%s", fileNames[j++]);

		if(numOfRows == 1)
			printf("\033[%dC", lenOfLongestFile);

		else if(j % numOfRows == 0)
			printf("\033[%dA\033[%dG", numOfRows-1, (j/numOfRows) * lenOfLongestFile);	//move back to row 1 and appropriate column
		
		else
			printf("\033[1B\033[%dG", (j/numOfRows) * lenOfLongestFile);	//move down 1 row and appropriate columns
	}

	printf("\033[%dB \033[1G", totalNumOfFiles%numOfRows+1);	//move the curser to the end of file listing

	closedir(dp);
}

int main(int argc, char* argv[]){

	columnsOnScreen = atoi(getenv("COLUMNS"));	//num of columns available on screen

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
