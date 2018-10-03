/*Assignment1*/

# include <stdio.h>
# include <signal.h>
# include <string.h>
# include <unistd.h>
# include <sys/times.h>
# include <sys/resource.h>

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
        char command[20], pgm[128], arg1[20], arg2[20], arg3[20], arg4[20];
        /*loop and wait for user command*/
        printf("a1jobs[%d]:",pid);
        scanf("%s ", &command); /*Just the command*/

        /*fix the case*/
        if (strcmp(command, "list")) {
            scanf("%*[^\n]"); /*skip rest of the line*/

            /*go through link list and print all*/
            struct job job_i = job1;
            while (job_i.next != NULL) {
                printf("%d: (pid=  %d, cmd= %s\n", job_i.index, job_i.pid, job_i.cmd);
                job_i = *job_i.next;
            }
            continue;
        }
        else if (strcmp(command, "run")) {
            struct job job_i = job1;
            while (job_i.next != NULL) {
                job_i = *job_i.next;
            }

            if (job_i.index == 32) {
                printf("Sorry jobs have arrive the max.\n");
                continue;
            }

            /*get pgm and args*/
            scanf("%s", &pgm);

            /*divied argment with program name*/
            char *token;
            token = strtok(pgm, " ");
            if (token != NULL) {
                strcopy(pgm,token);
                token = str(NULL, " ");
            }
            if (token != NULL) {
                strcopy(arg1,token);
                token = str(NULL, " ");
            }
            if (token != NULL) {
                strcopy(arg2,token);
                token = str(NULL, " ");
            }
            if (token != NULL) {
                strcopy(arg3,token);
                token = str(NULL, " ");
            }
            if (token != NULL) {
                strcopy(arg4,token);
                token = str(NULL, " ");
            }
            if (token != NULL) {
                printf("Too many argument!\n");
            }

            char argv[] = {arg1,arg2,arg3,arg4,NULL};
                            
            /*fork*/
            pid_t fpid = fork();
            if (fpid < 0) {
                printf("Error in fork\n");
                continue;
            } else if (fpid == 0) {
                /*child process*/
                int i = execv(pgm,argv);
                if (i < 0) {
                    printf("Fail to run execv.");
                    kill(fpid, SIGKILL);
                    continue;
                }
            } else {
                /*update the link list*/
                job_i.pid = fpid;
                strcat(job_i.cmd,pgm);
		strcat(job_i.cmd,argv);
                struct job job_next = {};
                job_next.index = job_i.index+1;
                *job_i.next = job_next;
            }
            continue;
        }
        else if (strcmp(command, "suspend")){
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
        else if (strcmp(command, "resume")){
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
        else if (strcmp(command, "terminate") {
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
                job_i = job_i.next;
            }

            if (job_i.index != job_num) {
                printf("Fail to find the process.\n");
                continue;
            }
            /*send single*/
            kill(job_i.pid, SIGKILL);
            continue;
	  }
        else if (strcmp(command, "exit"){
            scanf("%*[^\n]"); /*skip rest of the line*/
            break;
	  }
        else if (strcmp(command, "quit"){
            scanf("%*[^\n]"); /*skip rest of the line*/
            printf("Exit without kill child processes!\n");
            return;
	  }
        else{
            printf("Fail to recgnized the command");
            continue;
        }
    }
    
    struct job job_i;
    job_i = job1;
    while (job_i.next != NULL) {
        kill(job_i.pid, SIGNKILL);
        printf("job %d terminated\n", job_i.pid);
        &job_i = job_i.next;
    }

    /*get the time at the end*/
    struct tms times_end;
    clock_t time_end = times($times_en);

    printf("real: %d sec.\nuser: %d sec.\nsys: %d sec.\n", (time_begin-time_end)/CLOCKS_PER_SEC,(times_begin.tms_stime-times_begin.tms_stime)/CLOCKS_PER_SEC);
    printf("child user: %d sec.\nchild sys: %d sec.\n",(times_begin.tms_cutime-times_begin.tms_cutime)/CLOCKS_PER_SEC,(times_begin.tms_cstime-times_begin.tms_cstime)/CLOCKS_PER_SEC);
    
    return 0;
}
