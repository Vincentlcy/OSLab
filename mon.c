/*All Copyright Reserve*/

# define _POSIX_SOURCE
# define _BSD_SOURCE
# include <sys/types.h>
# include <stdlib.h>
# include <stdio.h>
# include <time.h>
# include <signal.h>
# include <string.h>
# include <sys/wait.h>
# include <unistd.h>
# include <sys/times.h>
# include <sys/resource.h>

struct process {
    int pid;
    char cmd[128];
};

int find_child(int ppid, FILE *fp, struct process *pros);

int main(int argc,char *argv[]) {
    int interval = 3;

    if (argc != 2 && argc != 3) {
        printf("Argument not enough\n");
        return 1;
    } else if (argc == 3){
        interval = strtol(argv[2],NULL,10);
    }

    /*set pid and interval time*/
    int ppid = strtol(argv[1],NULL,10);
    
    /*set limit of CPU time*/
    struct rlimit limit;
    limit.rlim_cur = 600;
    limit.rlim_max = 600;
    int text_return = setrlimit(RLIMIT_CPU, &limit);

    /*if set cpu limit fail*/
    if (text_return) {
        printf("Fail to set CPU limit\n");
        return -1;
    }

    int counter = 1;
    int pid = getpid();
    struct process *pros;

    while (1) {
        printf("almon [counter= %d, pid= %d, target_pid= %d, interval= %d sec]:\n", counter, pid, ppid, interval);

        FILE *fp;
	pros = calloc(20, sizeof(struct process));
         
        /*open pipeline*/
        if ((fp = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r")) == NULL) {
            printf("Error in open the pipeline");
        }

        /*Find first child*/
        int target = find_child(ppid, fp, pros);
	
        printf("[");
        for (int i=0;;i++) {
            if (pros[i].pid = 0) {break;}
            find_child(pros[i].pid, fp, pros);
            printf("%d:[%d,%s]",i,pros[i].pid,pros[i].cmd);
        }
        printf("]\n");

        /*terminated*/
        if (target) {
            break;
        }
	pclose(fp);
        sleep(interval);
    }

    printf("almon: target appears to have terminated; cleaning up\n");
    for (int i=0;;i++) {
            if (pros[i].pid = 0) {break;}
            kill(pros[i].pid, SIGKILL);
            printf("terminating [%d,%s]\n",pros[i].pid,pros[i].cmd);
    }
    printf("exiting almon\n");
    return 0;
}


int find_child(int ppid, FILE *fp, struct process *pros) {
    int pid_tem;
    int ppid_tem;
    int i = 0;
    /*s for last line target for target process 1 for terminate*/
    char cmd[100];
    int target = 0;
    char tep[1024];

    while (fgets(tep,256,fp)!=NULL) {
        /*scanf the pid*/
        fscanf(tep, " %d %d", &pid_tem, &ppid_tem);
	fscanf(tep, "%s",cmd);
	printf("%d %s\n", pid_tem,cmd);
        fscanf(fp, "%*[^\n]");
	
        if (pid_tem == ppid) {
	    target=1;
	}

        if (ppid_tem == ppid) {

            /*check array full*/
            if (pros[i].pid) {
                if (realloc(pros, 2*i*sizeof(struct process))==NULL){
		    exit(-1);
		}
                i++;
            }
            pros[i].pid = pid_tem;
	    strcpy(pros[i].cmd,cmd);
        } 
    }
    return target;
}
