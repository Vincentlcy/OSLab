/*Assignment1           */
/*All Copyright reserve */
/*2018 OCT 4th          */
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

struct job {
    int index;
    int pid;
    char cmd[200];
}; 

int main(void) {

    /*first job*/
    struct job jobs[32];
    for (int i=0;i<32;i++) {
        jobs[i].index = i;
        jobs[i].pid = 0;
    }

    /*get self pdi*/
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
        scanf("%s", command); /*Just the command*/

        /*fix the case*/
        if (strcmp(command, "list")==0) {
            scanf("%*[^\n]"); /*skip rest of the line*/

            /*go through array and print all*/
            for (int i=0;i<32;i++) {
                if (jobs[i].pid == 0) {
                    break;
                }
                printf("%d: (pid=    %d,  cmd=  %s\n", jobs[i].index, jobs[i].pid, jobs[i].cmd);
            }
            continue;
        }
        else if (strcmp(command, "run")==0) {
            int i;
            /*get the index of empty*/
            for (i=0;i<32;i++) {
                if (jobs[i].pid == 0) {
                    break;
                }
            }
            /*check full or not*/
            if (jobs[31].pid) {
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
            token = strtok_r(pgm, s, &sp);
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
		        if (count == 4) {
		            execlp(pgm,pgm,arg1,arg2,arg3,arg4,(char*) NULL);
		        } else if (count == 3) {
		            execlp(pgm,pgm,arg1,arg2,arg3,(char*) NULL);
		        } else if (count == 2) {
		            execlp(pgm,pgm,arg1,arg2,(char*) NULL);
		        } else if (count == 1) {
		            execlp(pgm,pgm,arg1,(char*) NULL);
		        } else{
		            execlp(pgm,pgm,(char*) NULL);
		        }
                /*quesion here*/
                
                if (errno) {
                    /*child exec fail, suicide*/
		            printf("Fail to run execv: %d; child suicide.",errno);
                    _exit(-1);
                    errno = 0;
                }
            } else if (fpid > 0){
                /*update the link list*/
                jobs[i].pid = fpid;
		        strcpy(jobs[i].cmd,cmd);
	        }
            continue;
        }
        else if (strcmp(command, "suspend")==0){
            /*get the pid*/
            int job_num;
            scanf("%d", &job_num);
            scanf("%*[^\n]"); /*skip rest of the line*/

            if (job_num < 0 || job_num > 31 || jobs[job_num].pid == 0) {
                printf("Fail to find the process\n");
                continue;
            }

            kill(jobs[job_num].pid, SIGSTOP);
            
            continue;
        }
        else if (strcmp(command, "resume")==0){
            /*get the pid*/
            int job_num;
            scanf("%d", &job_num);
            scanf("%*[^\n]"); /*skip rest of the line*/
            
            if (job_num < 0 || job_num > 31 || jobs[job_num].pid == 0) {
                printf("Fail to find the process\n");
                continue;
            }

            /*send single*/
            kill(jobs[job_num].pid, SIGCONT);
            continue;
        }
        else if (strcmp(command, "terminate")==0) {
            /*get the pid*/
            int job_num;
            scanf("%d", &job_num);
            scanf("%*[^\n]"); /*skip rest of the line*/
            
            if (job_num < 0 || job_num > 31 || jobs[job_num].pid == 0) {
                printf("Fail to find the process\n");
                continue;
            }
            /*send single*/
            kill(jobs[job_num].pid, SIGKILL);
	        waitpid(jobs[job_num].pid,NULL,0);
            continue;
	    }
        else if (strcmp(command, "exit")==0){
            scanf("%*[^\n]"); /*skip rest of the line*/
            for (int i=0;i<32;i++) {
                if (jobs[i].pid == 0) {
                    break;
                }
                kill(jobs[i].pid, SIGKILL);
	            waitpid(jobs[i].pid,NULL,0);
            }
            break;
	    }
        else if (strcmp(command, "quit")==0){
            scanf("%*[^\n]"); /*skip rest of the line*/
            printf("Exit without kill child processes!\n");
            break;
	    } else {
            printf("Fail to recgnized the command\n");
            continue;
        }
    }

    /*get the time at the end*/
    struct tms times_end;
    clock_t time_end = times(&times_end);

    printf("real: %lf sec.\nuser: %lf sec.\nsys: %lf sec.\n", (double)(time_end-time_begin)/CLOCKS_PER_SEC,(double)(times_end.tms_utime-times_begin.tms_utime)/CLOCKS_PER_SEC,(double)(times_end.tms_stime-times_begin.tms_stime)/CLOCKS_PER_SEC);
    printf("child user: %lf sec.\nchild sys: %lf sec.\n",((double)times_end.tms_cutime-times_begin.tms_cutime)/CLOCKS_PER_SEC,(double)(times_end.tms_cstime-times_begin.tms_cstime)/CLOCKS_PER_SEC);
    
    return 0;
}
