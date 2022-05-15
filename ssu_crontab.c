#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0
#define SECOND_TO_MICRO 1000000

int printFile();
int check(char *buf);
int checkNumber(char *buf, int n);
void delete(char *num);
void add();
void printUsage();
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

FILE *fp;
FILE *lfp;

char pbuf[1000][200];
char *fname = "ssu_crontab_file";
char *lname = "ssu_crontab_log";

int main()
{
	char buf[1024];
	char copybuf[1024];
	char com[1024];
	char *num;
	char *option;
	char *ptr;
	char *str;
	char copyp[200];
	int i = 0;
	time_t ct = time(NULL);
	char timebuf[50];
	struct timeval start, end;

	gettimeofday(&start, NULL);

	memset(buf, 0, sizeof(buf));

	if((fp = fopen(fname, "r+")) == NULL) {
		if((fp = fopen(fname, "w+")) == NULL ) {
			fprintf(stderr, "file open error for ssu_crontab_file\n");
			exit(1);
		}
	}

	if((lfp = fopen(lname, "a+")) == NULL) {
		fprintf(stderr, "file open error for %s\n", lname);
		exit(1);
	}

	while(fgets(pbuf[i], 200,fp) != NULL) {
		i++;
	}


	while(1) {
		i = printFile();
		printf("20172618> ");
		fgets(buf, 1024, stdin);
		strcpy(copybuf, buf);
		strcpy(com, buf);
		option = strtok(buf, " ");

		if(!strcmp(option, "add")) {
		 	if(check(copybuf) == FALSE) {
				printUsage();
				continue;
			}
			
			strcpy(timebuf, ctime(&ct));
			timebuf[strlen(timebuf) - 1] = '\0';
			fprintf(lfp, "[%s] %s", timebuf, com);
			sprintf(pbuf[i], "%d. %s", i, buf+4);
			add();
		}
		else if(!strcmp(option, "remove")) {
			num= strtok(NULL, " ");

			strcpy(timebuf, ctime(&ct));
			timebuf[strlen(timebuf) - 1] = '\0';

			strcpy(copyp, pbuf[atoi(num)]);
			ptr = strtok_r(copyp, " ", &str);
			fprintf(lfp, "[%s] remove %s", timebuf, str);
			delete(num);
		}
		else if(!strncmp(buf, "exit",4)) {
			fclose(fp);
			gettimeofday(&end, NULL);
			ssu_runtime(&start, &end);
			exit(0);
		}
		else
			fprintf(stderr, "올바른 명령어를 입력하세요.\n");
	}

	return 0;
}

int check(char *buf)
{
 	int i = 0;
	int j = 0;
	int num1, num2;
	int check;
	char *ptr;
	char str[1024];
	char *context;
	char period[5][100];

	strcpy(str, buf);
	ptr = strtok(str, " ");
    

	while((ptr = strtok(NULL, " ")) != NULL) {
		if(i == 5) 
			break;
		strcpy(period[i], ptr);
		i++;
	}


	for(i=0;i<5;i++) {
		if((strlen(period[i]) == 1) && period[i][0] == '*') {
			continue;
		}

		if(period[i][0] == '/' || period[i][0] == ',' || period[i][0] == '-') {
			return FALSE;
		}

		if((checkNumber(period[i], i)) == FALSE)
			return FALSE;
	}


	return TRUE;
		
}

int checkNumber(char *buf, int n)
{
	int num = 0;
	int i=0;

		while(buf[i] != '\0') {
			if((buf[i] == '*') && i == 0) {
				if(buf[i+1] == ',' || buf[i+1] == '-' || buf[i+1] == '*')
					return FALSE;
				else if(buf[i+1] == '/') {
					if(buf[i+2] == '/' || buf[i+2] == ',' || buf[i+2] == '*' || buf[i+2] == '-')
						return FALSE;
					i++;
				}
				else {
					i++;
					continue;
				}
			}
			else if(buf[i] >= '0' && buf[i] <= '9') {
				if(buf[i+1] >= '0' && buf[i+1] <= '9') { //2자리 수일때
					if(buf[i+2] >= '0' && buf[i+2] <= '9')  { // 세자리 수 이상 일때
						return FALSE;
					}
					num = num * 10 + buf[i] - 48;
					num = num * 10 + buf[i+1] - 48;
					if(n == 0 && num > 59)
						return FALSE;
					else if(n==1 && num > 23)
						return FALSE;
					else if(n==2 && (num < 1 || num > 31))
						return FALSE;
					else if(n==3 && (num < 1 || num > 12)) 
						return FALSE;
					else if(n==4)
						return FALSE;
					
					num = 0;
					i++;
				}
				else {  //한자리 수일때
					num = num * 10 + buf[i] - 48;
					if(n == 2 && num == 0)
						return FALSE;
					else if(n == 3 && num == 0)
						return FALSE;
					else if(n == 4 && num >= 7)
						return FALSE;

					num = 0;
				}
			}
			else if(buf[i] == '/' || buf[i] == '-' || buf[i] == ',') {
				if(buf[i+1] == '/' || buf[i+1] == '-' || buf[i+1] == ',' || buf[i+1] == '*')
					return FALSE;

				i++;
				continue;
			}
			else
				return FALSE;

			i++;
		}

		return TRUE;
}

void add()
{
	int i = 0;

	freopen(fname, "w", fp);

		
	while(pbuf[i][0] != '\0') {
		fprintf(fp, "%s", pbuf[i]);
		i++;
	}

	return;
}

int printFile() 
{
	char buf[1024];
	int i = 0;
	
	fseek(fp, 0, SEEK_SET);

	if(pbuf[0][0] == '\0')
		return i;

	while(pbuf[i][0] != '\0') {
		printf("%s",pbuf[i]);
		i++;
	}
	
	printf("\n");

	return i;
}

void delete(char *num)
{
 	int rnum = 0;
	int i=0;
	int j=0;
	int character;
	char deletebuf[200];
	char buf[200];
	char *ptr;

	memset(deletebuf, 0, sizeof(deletebuf));

	if(num[0] < '0' || num[0] > '9') {
		printUsage();
		return;
	}


	while(1) {
		if(i == strlen(num)-1)
			break;
		rnum = 10 * rnum + num[i] - 48;
		i++;
	}

	if(pbuf[rnum][0] == '\0') {
		printf("%d번째는 존재하지 않습니다.\n", rnum);
		return;
	}

	for(j=rnum;j<1000;j++) {
		if(pbuf[j][0] == '\0') {
			strcpy(pbuf[j-1], deletebuf);
			break;
		}
		ptr = strtok(pbuf[j+1], " ");
		ptr = strtok(NULL, "\0");
		sprintf(pbuf[j], "%d. %s", j, ptr);
	}

	freopen(fname, "w+", fp);
	i=0;
	while(pbuf[i][0] != '\0') {
		fprintf(fp, "%s", pbuf[i]);
		i++;
	}

	return;
}

void printUsage()
{
	printf("<Usage>\n");
	printf("add <실행주기> <명령어>\n");
	printf("‘*’: 해당 필드의 모든 값을 의미함\n");
   	printf("‘-’: ‘-’으로 연결된 값 사이의 모든 값을 의미함(범위 지정)\n");
   	printf("‘,’: ‘,’로 연결된 값들 모두를 의미함(목록)\n");
   	printf("‘/’: 앞에 나온 주기의 범위를 뒤에 나온 숫자만큼 건너뛰는 것을 의미함\n");
   	printf("remove <COMMAND_NUMBER>\n");
   	printf("exit : exit\n");
	
	return;
}

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t) 
{
     end_t->tv_sec -= begin_t->tv_sec; //실행후 - 실행전

     if(end_t->tv_usec < begin_t->tv_usec){
         end_t->tv_sec--;
         end_t->tv_usec += SECOND_TO_MICRO;
     }

     end_t->tv_usec -= begin_t->tv_usec;
     printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);

	 return;
}

