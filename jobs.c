/*Assignment1*/
# define _POSIX_SOURCE
# include <sys/types.h>
# include <stdio.h>
# include <time.h>
# include <signal.h>
# include <string.h>
# include <sys/wait.h>
# include <unistd.h>
# include <sys/times.h>
# include <sys/resource.h>
# include <errno.h>
# define MAX_JOBS 32
/*# define CLOCKS_PER_SEC 1000000l*/

struct job {
    int index;
    int pid;
    char cmd[200];
    struct job *next;
}; 

int main(void) {

    /*first job*/
    struct job job1 = {
        .index = 0,
    };

    pid_t pid = getpid();
    int text_return;

    /*set limit of CPU time*/
    struct rlimit limit;
    limit.rlim_cur = 600;
    limit.rlim_max = 600;

    text_return = setrlimit(RLIMIT_CPU, &limit);

    /*check set CPU time success or not*/
    if (text_return) {
        printf("Fail to set CPU limit. Function exited.\n");
        return -1;
    }

    /*get the time at the start*/
    struct tms times_begin;
    clock_t time_begin = times(&times_begin);

    while (1) {
        char command[20], pgm[256], arg1[20], arg2[20], arg3[20], arg4[20];
        /*loop and wait for user command*/
        printf("a1jobs[%d]:",pid);
        scanf("%s ", command); /*Just the command*/

        /*fix the case*/
        if (strcmp(command, "list")==0) {
	    printf("list all process\n");
            scanf("%*[^\n]"); /*skip rest of the line*/

            /*go through link list and print all*/
            struct job job_i = job1;
	    printf("I have a pen%d\n",job_i.next==NULL);
            while (job_i.next != NULL) {
		printf("i have an apple\n");
                printf("%d: (pid=  %d, cmd= %s\n", job_i.index, job_i.pid, job_i.cmd);
                job_i = *job_i.next;
            }
            continue;
        }
        else if (strcmp(command, "run")==0) {
            struct job job_i = job1;
            while (job_i.next != NULL) {
                job_i = *job_i.next;
            }

            if (job_i.index == 32) {
                printf("Sorry jobs have arrive the max.\n");
                continue;
            }

            /*get pgm and args*/
            scanf("%[^\n]",pgm);
	    char cmd[256];
	    strcpy(cmd,pgm);

            /*divied argment with program name*/
            char *token;
	    int count = 0;
	    char *s = " ";
	    char *sp = NULL;
            token = strtok_r(cmd, s, &sp);
            if (token != NULL) {
                strcpy(pgm,token);
                token = strtok_r(NULL, s, &sp);
            }
            if (token != NULL) {
                strcpy(arg1,token);
		count += 1;
                token = strtok_r(NULL, s, &sp);
            }
            if (token != NULL) {
                strcpy(arg2,token);
		count += 1;
                token = strtok_r(NULL, s, &sp);
            }
            if (token != NULL) {
                strcpy(arg3,token);
		count += 1;
                token = strtok_r(NULL, s, &sp);
            }
            if (token != NULL) {
                strcpy(arg4,token);
		count += 1;
                token = strtok_r(NULL, s, &sp);
            }
            if (token != NULL) {
                printf("Too many argument!\n");
            }

            /*fork*/
            pid_t fpid = fork();
            if (fpid < 0) {
                printf("Error in fork\n");
                continue;
            } else if (fpid == 0) {
                /*child process*/
		int i;
		if (count == 4) {
		    i = execlp(pgm,arg1,arg2,arg3,arg4,NULL);
		} else if (count == 3) {
		    i = execlp(pgm,arg1,arg2,arg3,NULL);
		} else if (count == 2) {
		    i = execlp(pgm,arg1,arg2,NULL);
		} else if (count == 1) {
		    i = execlp(pgm,arg1,(char*) NULL);
		} else{
		    i = execlp(pgm,pgm,(char*) NULL);
		}
                if (i < 0){
		    printf("%s, %s, %s, %s, %d, %s\n",arg1,arg2,arg3,arg4,count,pgm);
		    printf("Fail to run execv: %d\n.",errno);
                    kill(fpid, SIGKILL);
		    waitpid(fpid,NULL,0);
                    continue;
                }
            } else {
                /*update the link list*/
		printf("father\n");
                job_i.pid = fpid;
		strcpy(job_i.cmd,cmd);
                struct job job_next = {};
                job_next.index = job_i.index+1;
                *job_i.next = job_next;
            }
            continue;
        }
        else if (strcmp(command, "suspend")==0){
            /*get the pid*/
            int job_num;
            scanf("%d", &job_num);
            scanf("%*[^\n]"); /*skip rest of the line*/

            struct job job_i = job1;
            while (job_i.next != NULL) {
                if (job_i.index == job_num){
                    break;
                }
                job_i = *job_i.next;
            }

            if (job_i.index != job_num) {
                printf("Fail to find the process.\n");
                continue;
            }

            /*send single*/
            kill(job_i.pid, SIGSTOP);
            continue;
        }
        else if (strcmp(command, "resume")==0){
            /*get the pid*/
            int job_num;
            scanf("%d", &job_num);
            scanf("%*[^\n]"); /*skip rest of the line*/
            
            struct job job_i;
            job_i = job1;
            while (job_i.next != NULL) {
                if (job_i.index == job_num){
                    break;
                }
                job_i = *job_i.next;
            }

            if (job_i.index != job_num) {
                printf("Fail to find the process.\n");
                continue;
            }

            /*send single*/
            kill(job_i.pid, SIGCONT);
            continue;
        }
        else if (strcmp(command, "terminate")==0) {
            /*get the pid*/
            int job_num;
            scanf("%d", &job_num);
            scanf("%*[^\n]"); /*skip rest of the line*/
            
            struct job job_i;
            job_i = job1;
            while (job_i.next != NULL) {
                if (job_i.index == job_num){
                    break;
                }
                job_i = *job_i.next;
            }

            if (job_i.index != job_num) {
                printf("Fail to find the process.\n");
                continue;
            }
            /*send single*/
            kill(job_i.pid, SIGKILL);
	    waitpid(job_i.pid,NULL,0);
            continue;
	  }
        else if (strcmp(command, "exit")==0){
            scanf("%*[^\n]"); /*skip rest of the line*/
            break;
	  }
        else if (strcmp(command, "quit")==0){
            scanf("%*[^\n]"); /*skip rest of the line*/
            printf("Exit without kill child processes!\n");
            return 0;
	  }
        else{
            printf("Fail to recgnized the command");
            continue;
        }
    }
    
    struct job job_i;
    job_i = job1;
    while (job_i.next != NULL) {
        kill(job_i.pid, SIGKILL);
	waitpid(job_i.pid, NULL, 0);
        printf("job %d terminated\n", job_i.pid);
        job_i = *job_i.next;
    }

    /*get the time at the end*/
    struct tms times_end;
    clock_t time_end = times(&times_end);

    printf("real: %ld sec.\nuser: %ld sec.\nsys: %ld sec.\n", (time_begin-time_end)/CLOCKS_PER_SEC,(times_begin.tms_utime-times_begin.tms_utime)/CLOCKS_PER_SEC,(times_begin.tms_stime-times_begin.tms_stime)/CLOCKS_PER_SEC);
    printf("child user: %ld sec.\nchild sys: %ld sec.\n",(times_begin.tms_cutime-times_begin.tms_cutime)/CLOCKS_PER_SEC,(times_begin.tms_cstime-times_begin.tms_cstime)/CLOCKS_PER_SEC);
    
    return 0;
}
