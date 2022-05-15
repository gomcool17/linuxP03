#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>

int daemon_init(void);
void do_crond();
void printrun();
void divide(char (*save)[50], char *com, int i);

FILE *lfp;
char path[200];
char *crontfile = "ssu_crontab_file";
char *crontlog = "ssu_crontab_log";
char saveFile[1000][200];
int beforeCount;


int main()
{
	FILE *rfp;
	int i = 0;
	char buf[200];
	char *strbuf;

	getcwd(path, 200);
	
//		if((lfp  = fopen(crontlog, "a+")) == NULL) {
//			fprintf(stderr, "fopen error for %s\n", crontlog);
//			exit(1);
//		}
//	do_crond();
	daemon_init();

	return 0;
}

void do_crond() 
{
	FILE  *rfp;
	char compareFile[1000][200];
	char buf[200];
	char *strbuf;
	int i = 0;
	int afterCount;

	memset(compareFile, 0, sizeof(compareFile));

	getcwd(path, 200);
	
	memset(saveFile, 0, sizeof(saveFile));
	memset(buf, 0, sizeof(buf));

	if((rfp = fopen(crontfile, "r")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", crontfile);
		exit(1);
	}

	while(fgets(buf, 200, rfp) != NULL) {
		strtok_r(buf, " ", &strbuf);
		strcpy(saveFile[i], strbuf);
		memset(buf, 0, sizeof(buf));
		i++;
	}

	beforeCount = i;

	printrun();

	fclose(rfp);

	return;
}

void printrun()
{
	long min[60] = {0}; //분
	long hour[24] = {0}; //시
	long date[32] = {0}; //일
	long mon[13] = {0}; //월
	long day[7] = {0}; //요일
	char save[5][50]; //주기 저장
	char copysave[50];
	char str[50];
	char com[200]; //명령어 저장
	char *ptr;
	char timebuf[200];
	int hyphen = 0; //하이폰
	int star = 0; //별
	int comma = 0; //콤마
	int slash = 0; //슬래시
	int len;
	int num1, num2, num3;
	int i = 0,j = 0,k = 0, h = 0;
	time_t ct = time(NULL);
	struct tm t;

	memset(save, 0, sizeof(save));
	

	for(i = 0; i<beforeCount; i++) {
		divide(save, com,i);
		for(j=0; j<5; j++) {
			strcpy(copysave, save[j]);

			ptr = strtok(copysave, ",");
			
			while(ptr != NULL) {
	 			strcpy(str, ptr);
	 			len = strlen(str);
				star = 0;
				hyphen = 0;
				slash = 0;
				k=0;
				while(k < len) {
					if(str[k] == '-') 
						hyphen = k;
					if(str[k] == '/')
						slash = k;
					if(str[k] == '*')
						star = 1;

					k++;
				}

				if(star == 1 && slash != 0) {
					num3 = atoi(str + slash + 1);
					if(j == 0) {
						num1 = 0;
						num2 = 59;
						for(h = 0; h + num1<=num2; h++) {
							if((h+1) % num3 == 0) {
								min[h+num1] = 1;
							}
						}
					}
					else if(j == 1) {
						num1 = 0;
						num2 = 23;
						for(h=0; h+num1 <= num2; h++) {
							if((h+1)%num3 == 0)
								hour[h+num1] = 1;
						}
					}
					else if(j == 2) {
						num1 = 1;
						num2 = 31;
						for(h=0; h+num1 <= num2; h++) {
							if((h+1) % num3  == 0)
								date[h+num1] = 1;
						}
					}
					else if(j == 3) {
						num1 = 1;
						num2 = 12;
						for(h=0; h+num1<=num2; h++) {
							if((h+1) % num3 == 0)
								mon[h+num1] = 1;
						}
					}
					else if(j == 4) {
						num1 = 1;
						num2 = 6;
						for(h=0; h+num1<=num2; h++) {
							if((h+1)%num3 == 0)
								min[h+num1] = 1;
						}
					}
					else
						;
				}
				else if(star == 1) {
					if(j==0) {
						for(h=0;h<60;h++)
							min[h] = 1;
					}
					else if(j == 1) {
						for(h=0; h<24; h++)
							hour[h] = 1;
					}
					else if(j == 2) {
						for(h=1;h<=31;h++) 
							date[h] = 1;
					}
					else if(j ==3) {
						for(h=1; h<=12; h++)
							mon[h] = 1;
					}
					else if(j == 4) {
						for(h=0; h<7; h++) 
							day[h] = 1;
					}
					else
						;
				}
				else if(hyphen != 0 && slash != 0) {
					num1 = atoi(str);
					num2 = atoi(str + hyphen+1);
					num3 = atoi(str +slash + 1);
					for(h = 0; h + num1<=num2; h++) {
						if((h+1) % num3 == 0) {
							if(j == 0)
								min[h+num1] = 1;
							else if(j == 1)
								hour[h+num1] = 1;
							else if(j == 2)
								date[h+num1] = 1;
							else if(j==3)
								mon[h+num1] = 1;
							else if(j==4)
								day[h+num1] = 1;
							else
								;
						}
					}
				}
				else if(hyphen != 0) {
					num1 = atoi(str);
					num2 = atoi(str + hyphen + 1);
					for(h = num1; h <= num2; h++) {
						if(j == 0)
							min[h] = 1;
						else if(j == 1)
							hour[h] = 1;
						else if(j == 2)
							date[h] = 1;
						else if(j==3)
							mon[h] = 1;
						else if(j==4)
							day[h] = 1;
						else
							;
					}
				}	
				else {
					num1 = atoi(str);
					if(j == 0)
						min[num1] = 1;
					else if(j == 1)
						hour[num1] = 1;
					else if(j == 2)
						date[num1] = 1;
					else if(j==3)
						mon[num1] = 1;
					else if(j==4)
						day[num1] = 1;
					else
						;
				}

				ptr = strtok(NULL, ",");
			}
		}

	    localtime_r(&ct, &t);

		if(min[t.tm_min] == 1 && hour[t.tm_hour] == 1 && date[t.tm_mday] == 1 && mon[t.tm_mon+1] == 1 && day[t.tm_wday] == 1) {

			system(com);
			strcpy(timebuf, ctime(&ct));
			timebuf[strlen(timebuf) - 1] = '\0';
			fprintf(lfp, "[%s] run %s", timebuf, saveFile[i]);
			
		}

		memset(min, 0, sizeof(min));
		memset(hour, 0, sizeof(hour));
		memset(date, 0, sizeof(date));
		memset(mon, 0, sizeof(mon));
		memset(day, 0, sizeof(day));
	}
	
	return;
}

void divide(char (*save)[50], char *com, int i)
{
	int j = 1;
	char copy[200];
	char *str;
	char *ptr;

	strcpy(copy, saveFile[i]);

	ptr = strtok_r(copy, " ", &str);
	strcpy(save[0], ptr);
	
	ptr = strtok_r(NULL, " ", &str);
	strcpy(save[1], ptr);

	ptr = strtok_r(NULL, " ", &str);
	strcpy(save[2], ptr);

	ptr = strtok_r(NULL, " ", &str);
	strcpy(save[3], ptr);

	ptr = strtok_r(NULL, " ", &str);
	strcpy(save[4],ptr);

	strcpy(com, str);

	return;
}

int daemon_init(void)
{
	pid_t pid;
	int fd, maxfd;
	time_t c;
	struct tm t;


	if((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid != 0)
		exit(0);

	pid = getpid();
	setsid();

	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	maxfd = getdtablesize();

	for(fd = 0; fd < maxfd; fd++)
		close(fd);

	umask(0);
	chdir("/");

	fd = open("/dev/null", O_RDWR);
	dup(0);
	dup(0);

	chdir(path);


	while(1) {
		c = time(NULL);
		localtime_r(&c, &t);
		if(t.tm_sec==0)
			break;
	}
	while(1) {
		if((lfp  = fopen(crontlog, "a+")) == NULL) {
			fprintf(stderr, "fopen error for %s\n", crontlog);
			exit(1);
		}
		do_crond();

		sleep(60);

		fclose(lfp);
	}

	return 0;
}
