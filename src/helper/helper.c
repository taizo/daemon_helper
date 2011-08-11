/*
$Id: helper.c,v 1.11 2006/12/18 04:47:02 taizo Exp $
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "helper.h"

#if defined(SYSLOG)
#include <syslog.h>
#endif
char config_file[MAX_BUFF];

typedef struct _config_ {
	char pattern[MAX_BUFF];       /* Process name.         */
	char command[MAX_BUFF];       /* Execute command.      */
	int  interval;                /* Monitoring interval.  */
} CONFIG;
static CONFIG *clist;

static int	log_fd = 0x00;

void log_it(message)
char	*message;
{
#if defined(LOG_FILE)
	char			*msg;
	time_t			now = time((time_t) 0);
	register struct tm	*t = localtime(&now);


#endif /*LOG_FILE*/

#if defined(LOG_FILE)
	msg = malloc(strlen(message)
		     + MAX_BUFF);

	if(msg == NULL)
		fprintf(stderr,"failed to allocate memory for log message");
	else {
		if(log_fd == 0x00) {
			log_fd = open(LOG_FILE, O_WRONLY|O_APPEND|O_CREAT, 0600);
			if(log_fd == 0x00) {
				fprintf(stderr,"can't open log file %s", LOG_FILE);
			} else {
				(void)fcntl(log_fd, F_SETFD, 1);
			}
		}

		sprintf(msg, "[%04d/%02d/%02d %02d:%02d:%02d] %s\n",
			t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min,
			t->tm_sec, message);

		if(log_fd == 0x00 || write(log_fd, msg, strlen(msg)) == -1) {
			fprintf(stderr,"can't write to log file %s", LOG_FILE);
		}

		free(msg);
	}
#endif /*LOG_FILE*/

#if defined(SYSLOG)
	syslog(LOG_ERR, "%s", message);
#endif /*SYSLOG*/
}

void log_close() {
	if(log_fd != 0x00) {
		close(log_fd);
		log_fd = 0x00;
	}
}

void signal_handler(sig)
int sig;
{

	switch(sig) {
	case SIGHUP:
		log_it("hangup signal catched");
		break;

	case SIGTERM:
		log_it("terminate signal catched");
		exit(0);
		break;
	}
}

void daemonize()
{
	int i,lfp;
	char str[10];

	if(getppid()==1) return; /* already a daemon */
	i=fork();
	if(i<0) exit(1); /* fork error */
	if(i>0) exit(0); /* parent exits */

	/* child (daemon) continues */
	setsid(); /* obtain a new process group */
	for(i=getdtablesize();i>=0;--i) close(i); /* close all descriptors */
	i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */
	umask(027); /* set newly created file permissions */
	chdir(EXECUTION_DIR); /* change running directory */
	lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
	if(lfp<0) exit(1); /* can not open */
	if(lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
	/* first instance continues */
	sprintf(str,"%d\n",getpid());
	write(lfp,str,strlen(str)); /* record pid to lockfile */
	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP,signal_handler); /* catch hangup signal */
	signal(SIGTERM,signal_handler); /* catch kill signal */
}

int startServ(command)
char *command;
{
	int pid, status;
	char buff[MAX_BUFF];

	memset(buff,0x00,MAX_BUFF);

	/* try to launch init script */
	if((pid=fork()) < 0){
		return -1;
	} else if(pid == 0){
		if (execlp("sh","sh","-c",command,">/dev/null 2>&1",NULL) < 0) {
			snprintf(buff,MAX_BUFF,"failed to execute \"%s\"",command);
			log_it(buff);
			exit(1);
		}
	} 
	snprintf(buff,MAX_BUFF,"Execute command: \"%s\"",command);
	log_it(buff);
	sleep(10);
/***
	while (waitpid(pid, &status, 0) != pid);
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		fprintf(stderr, "%s: could not launch action script\n", command);
	}
***/
	return 0;
}

unsigned int getProcessID(process)
char *process;
{
	int target_link;
	DIR *dir_p;
	DIR *dir_proc;
	struct dirent *dir_entry_p;
	struct dirent *dir_proc_p;
	char dir_name[35];
	char target_name[235];
	char exe_link[235];
	char process_id[20];

	dir_p = opendir("/proc/");

	while(NULL != (dir_entry_p = readdir(dir_p))) {

		/* Checking for numbered directories */
		if(strspn(dir_entry_p->d_name, "0123456789" ) == strlen(dir_entry_p->d_name)) {
			strcpy(process_id, dir_entry_p->d_name);
			strcpy(dir_name, "/proc/");
			strcat(dir_name, dir_entry_p->d_name);
			strcat(dir_name, "/");

			if((dir_proc = opendir(dir_name))) {
				while((dir_proc_p = readdir(dir_proc))) {
					/* Check for that exe link file */
					if(strcmp(dir_proc_p->d_name, "exe") == 0) {
						strcat(exe_link, dir_name);
						strcat(exe_link, dir_proc_p->d_name);

						/* Getting the target of the exe ie to which binary it points to */
						target_link = readlink(exe_link, target_name, sizeof(target_name)); 
						target_name[target_link] = '\0';

						/* Searching for process name in the target name */
						if(strstr(target_name, process) != NULL) {
							closedir(dir_p);
							closedir(dir_proc);
							return atoi(process_id);
						}
						strcpy(exe_link ,"" );
					}
				}
				closedir(dir_proc);
			} else {
				//printf( "Dir %s opening failed \n", dir_name );
			}
		}
	}
	closedir(dir_p);
	return 0;
}

int readConfig(filename)
char *filename;
{
	int len;
	CONFIG *p;
	FILE *fp;
	char aline[MAX_BUFF];
	char buff[MAX_BUFF];

	memset(aline,0x00,MAX_BUFF);
	memset(buff,0x00,MAX_BUFF);

	/* Get a CONFIG struct . */
	if((p = (CONFIG *) malloc(sizeof(CONFIG))) == NULL) {
		log_it("out of memory");
		return (-1);
	}

	if((fp = fopen(filename,"r")) == NULL) {
		snprintf(buff,MAX_BUFF,"cannot open file \"%s\"",filename);
		log_it(buff);
		return (-1);
	} else {
		while(fgets(aline, MAX_BUFF, fp) != NULL) {
            // strip linefeed
			len = strlen(aline);
			if(aline[len-1] == '\n' ) {
				aline[len-1] = '\0';
			}

			if(strncmp("pattern=",aline,8) == 0) {
				strncpy(p->pattern, &aline[8], MAX_BUFF);
			}
			if(strncmp("command=",aline,8) == 0) {
				strncpy(p->command, &aline[8], MAX_BUFF);
			}
			if(strncmp("interval=",aline,9) == 0) {
				p->interval = atoi(&aline[9]);
			}
		}
		fclose(fp);
	}

	if(p->command=="" || p->pattern=="") {
		log_it("cannot get parameters");
		return (-1);
	}
	clist = p;

	return 0;
}


int main(argc, argv)
int argc;
char **argv;
{

	int c,errflag;
	int pid,interval;
	char buff[MAX_BUFF];
	extern char *optarg;
	extern int optind;
	extern CONFIG *clist;

	memset(config_file,0x00,MAX_BUFF);
	memset(buff,0x00,MAX_BUFF);

	errflag = 0;
	while(!errflag && (c=getopt(argc,argv,"c:")) != -1 ) {
		switch(c) {
		case 'c':
			strncpy(config_file, optarg, MAX_BUFF);
			break;
		default:
			errflag = 1;
			break;
		}
	}

	if(*config_file=='\0') {
		strcpy(config_file, CONFIG_FILE);
	}

	if(readConfig(config_file) != 0) {
		return -1;
	}

	// make this program daemonized.
	daemonize();
	snprintf(buff,MAX_BUFF,"Starting helper daemon (version: %s)",VERSION);
	log_it(buff);

	while(1) {
		if((pid=getProcessID(clist->pattern)) == 0) {
			startServ(clist->command);
		}
		if(clist->interval) {
			interval = clist->interval;
		} else {
			interval = DEF_INTERVAL;
		}
		sleep(interval);
	}

	return 0;
}
/* EOF */
