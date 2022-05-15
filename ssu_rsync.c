#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void scanSrc();
void linkToDst(int count);
void printLog(char *filename, int size);
void printUsage();
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);
int rOption = 0;
int tOption = 0;
int mOption = 0;
int checkfirst = 0;

FILE *lfp;
char *fname = "ssu_rsync_log"; //로그 파일 이름

char path[200]; //실행시 디렉토리 경로 저장(학번 디렉토리경로)
char srcpath[200];
char dstpath[200];
char name[20];
char first[20];
char second[20];
char option[3];

typedef struct saveSrc {
	char name[200];
	time_t saveMtime;
	time_t saveAtime;
}saveSrc;

saveSrc src[1024];
saveSrc toCompare[1024];

static int filter(const struct dirent *dirent)
{
	if(!(strncmp(dirent->d_name, ".", 1)))
		return 0;
	else
		return 1;
}

int main(int argc, char *argv[])
{

	char *ptr;
    struct stat statbuf;
	struct timeval start, end;
	double diffTime;

	gettimeofday(&start, NULL);

	getcwd(path, 200);

	if(argc >= 4 || argc < 2) {
		printUsage();
		exit(1);
	}

	if(!strncmp(argv[1], "-r",2)) {
		rOption = 1;
		exit(0);
	}
	else if(!strncmp(argv[1], "-t", 2)) {
		tOption = 1;
		exit(0);
	}
	else if(!strncmp(argv[1], "-m", 2)) {
		mOption = 1;
		exit(0);
	}
	else {
		if(argv[1][0] == '/') {
			strcpy(srcpath, argv[1]);
			ptr = strtok(argv[1], "/");
			while((ptr = strtok(NULL, "/")) != NULL) {
				strcpy(first, ptr);
			}
		}
		else {
			sprintf(srcpath, "%s/%s", path, argv[1]);
			strcpy(first, argv[1]);
		}
		
		if(argv[2][0] == '/') {
			strcpy(dstpath, argv[2]);
			ptr = strtok(argv[2], "/");
			while((ptr = strtok(NULL, "/")) != NULL)
				strcpy(second, ptr);
		}
		else {
			sprintf(dstpath, "%s/%s", path, argv[2]);
			strcpy(second,argv[2]);
		}
	}



	if((lfp = fopen(fname, "a+")) == NULL) { //로그 파일 생성 및 열기
		fprintf(stderr, "file open error for %s\n", fname);
		exit(1);
	}

	stat(srcpath, &statbuf);

	if(access(srcpath, F_OK) < 0) {
		printUsage();
		exit(1);
	}

	if(S_ISDIR(statbuf.st_mode)) {
		if(access(srcpath, X_OK) < 0) {
			printUsage();
			exit(1);
		}
	}

	if(access(srcpath, R_OK) < 0) {
		printUsage();
		exit(1);
	}

	if(access(dstpath, F_OK) < 0) {
		printUsage();
		exit(1);
	}

	stat(dstpath, &statbuf);
	if(!S_ISDIR(statbuf.st_mode)) {
		printUsage();
		exit(1);
	}
	
	
	if(access(dstpath, X_OK) < 0) {
		printUsage();
		exit(1);
	}

	if(access(dstpath, R_OK) < 0) {
		printUsage();
		exit(1);
	}


	scanSrc();

	fclose(lfp);

	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	
	return 0;
}

void scanSrc() 
{
	struct dirent **srclist;
	struct stat statbuf;
	int count;
	int i=0;

	stat(srcpath, &statbuf);
	if(S_ISDIR(statbuf.st_mode)) {
		if((count = scandir(srcpath, &srclist, filter, alphasort)) < 0) {
			fprintf(stderr, "scandir error for %s\n", srcpath);
			exit(1);
		}

		chdir(srcpath);
		for(i=0;i<count;i++) {
			strcpy(src[i].name, srclist[i]->d_name);
			stat(srclist[i]->d_name, &statbuf);
			src[i].saveMtime = statbuf.st_mtime;
			src[i].saveAtime = statbuf.st_atime;
			sprintf(toCompare[i].name, "%s/%s", srcpath, srclist[i]->d_name);
			toCompare[i].saveMtime = statbuf.st_mtime;
			toCompare[i].saveAtime = statbuf.st_atime;
		}

		chdir(path);
	
		linkToDst(count);
	}
	else {
		linkToDst(-1);
	}

	return;
}

void linkToDst(int count)
{
	struct dirent **dstlist;
	struct stat statbuf;
	struct stat dstatbuf;
	struct utimbuf timebuf;
	FILE *dfp;
	FILE *sfp;
	int i;
	int size;
	char dstbuf[1024]; //src가 디렉토리일때 사용
	char filename[20]; //src가 파일일때 사용
	char buf[200];
	char copyFile[2000];
	char *ptr;

	if(count == -1) {
		strcpy(buf, srcpath);
		ptr = strtok(buf, "/");
		while((ptr = strtok(NULL, "/"))!=NULL) {
			memset(filename, 0, sizeof(filename));
			strcpy(filename, ptr);
		}
		memset(dstbuf, 0, sizeof(dstbuf));
		strcpy(name, filename);
		sprintf(dstbuf, "%s/%s", dstpath, filename);
		stat(srcpath, &statbuf);
		stat(dstbuf, &dstatbuf);
		if(access(dstbuf, F_OK) < 0) {
			if((sfp = fopen(srcpath, "r")) == NULL) {
				fprintf(stderr, "open error for %s\n", toCompare[i].name);
				exit(1);
			}
			if((dfp = fopen(dstbuf, "w+")) == NULL) {
				fprintf(stderr, "open error in dst for %s\n", dstbuf);
				exit(1);
			}
			while(fgets(buf, sizeof(buf), sfp) != NULL) {
				fprintf(dfp, "%s", buf);
				memset(buf, 0, sizeof(buf));
			}
			fseek(dfp, 0, SEEK_END);
			size = ftell(dfp);
			printLog(name, size);
			fclose(sfp);
			fclose(dfp);
			timebuf.actime = statbuf.st_atime;
			timebuf.modtime = statbuf.st_mtime;
			utime(dstbuf, &timebuf);

		}
		else {
			if((statbuf.st_mtime != dstatbuf.st_mtime) || (statbuf.st_atime != dstatbuf.st_atime)) {
				if((sfp = fopen(srcpath, "r")) == NULL) {
					fprintf(stderr, "open error for %s\n", toCompare[i].name);
					exit(1);
				}
				if((dfp = fopen(dstbuf, "w+")) == NULL) {
					fprintf(stderr, "open error in dst for %s\n", dstbuf);
					exit(1);
				}
				while(fgets(buf, sizeof(buf), sfp) != NULL) {
					fprintf(dfp, "%s", buf);
					memset(buf, 0, sizeof(buf));
				}
				fseek(dfp, 0, SEEK_END);
				size = ftell(dfp);
				fclose(sfp);
				fclose(dfp);
				printLog(name, size);
				timebuf.actime = statbuf.st_atime;
				timebuf.modtime = statbuf.st_mtime;
				utime(dstbuf, &timebuf);
	     	 }
    	}
	}
	else {
		for(i=0; i<count; i++) {
			memset(dstbuf, 0, sizeof(dstbuf));
			sprintf(dstbuf, "%s/%s", dstpath, src[i].name);
			if(access(dstbuf, F_OK) < 0) {
				if((sfp = fopen(toCompare[i].name, "r")) == NULL) {
					fprintf(stderr, "open error for %s\n", toCompare[i].name);
					exit(1);
				}
				if((dfp = fopen(dstbuf, "w+")) == NULL) {
					fprintf(stderr, "open error in dst for %s\n", dstbuf);
					exit(1);
				}
				while(fgets(buf, sizeof(buf), sfp) != NULL) {
					fprintf(dfp, "%s", buf);
					memset(buf, 0, sizeof(buf));
				}
				fseek(dfp, 0, SEEK_END);
				size = ftell(dfp);
				printLog(src[i].name, size);
				fclose(sfp);
				fclose(dfp);
				timebuf.actime = src[i].saveAtime;
				timebuf.modtime = src[i].saveMtime;
				utime(dstbuf, &timebuf);
			}
			else {
				stat(dstbuf, &dstatbuf);
				if((dstatbuf.st_mtime != toCompare[i].saveMtime) || (dstatbuf.st_atime != toCompare[i].saveAtime)) {
					if((sfp = fopen(toCompare[i].name, "r")) == NULL) {
						fprintf(stderr, "open error for %s\n", toCompare[i].name);
						exit(1);
					}
					if((dfp = fopen(dstbuf, "w+")) == NULL) {
						fprintf(stderr, "open error in dst for %s\n", dstbuf);
						exit(1);
					}
					while(fgets(buf, sizeof(buf), sfp) != NULL) {
						fprintf(dfp, "%s", buf);
						memset(buf, 0, sizeof(buf));
					}
					fseek(dfp, 0, SEEK_END);
					size = ftell(dfp);
					printLog(src[i].name, size);
					fclose(sfp);
					fclose(dfp);
					timebuf.actime = src[i].saveAtime;
					timebuf.modtime = src[i].saveMtime;
					utime(dstbuf, &timebuf);
				}
			}
		}
	}

	return;
}

void printLog(char *filename, int size) 
{
	time_t current_t = time(NULL);
	char timebuf[50];

	strcpy(timebuf, ctime(&current_t));
	timebuf[strlen(timebuf) - 1] = '\0';

	if(checkfirst == 0) {
		if(rOption == 1 || tOption == 1 || mOption == 1) {
			fprintf(lfp, "[%s] ssu_rsync %s %s %s\n", timebuf, option, first, second) ;
			checkfirst = 1;
		}
		else {
			fprintf(lfp, "[%s] ssu_rsync %s %s\n", timebuf, first, second);
			checkfirst = 1;
		}
	}


	fprintf(lfp, "\t%s %dbytes\n", filename, size);

	return;
}

void printUsage()
{
	printf("<Usage>\n");
	printf("[option] <src> <dst>\n");
	printf("옵션이 없을수도 있습니다.\n");
	printf("src는 파일일 수도 디렉토리일 수도 있습니다.\n");
	printf("dst는 디렉토리여야 합니다.\n");
	printf("접근 권한을 확인해 주세요.\n");

	return;
}

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	end_t->tv_sec -= begin_t->tv_sec;

	if(end_t->tv_usec < begin_t->tv_usec) {
		end_t->tv_sec--;
		end_t->tv_usec += SECOND_TO_MICRO;
	}

	 end_t->tv_usec -= begin_t->tv_usec;
     printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);

	 return;
}
