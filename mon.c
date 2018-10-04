/*All Copyright Reserve*/

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

struct process {
    int pid;
    char[128] cmd;
};

int find_child(int ppid, FILE *fp struct *pros);

int main(int argc,char *argv[]) {
    /*set pid and interval time*/
    int ppid = argv[1];
    int interval = 3;

    if (argc != 2 || 3) {
        printf("Argument not enough\n");
        return 1;
    } else if (argc == 3){
        interval = argv[2];
    }

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

    while (1) {
        printf("almon [counter= %d, pid= %d, target_pid= %d, interval= %d sec]:\n", counter, pid, ppid, interval);

        FILE *fp;
        struct process *pros = (struct job*)calloc(sizeof(struct process)*20);
        
        /*open pipeline*/
        if ((fp = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r") == NULL) {
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


int find_child(int ppid, FILE *fp struct *pros) {
    int pid_tem;
    int ppid_tem;
    int i = 0;
    /*s for last line target for target process 1 for terminate*/
    char s;
    int target = 0;

    while (1) {
        /*scanf the pid*/
        fscanf(fp, " %d", pid_tem);
        fscanf(fp, "%d", ppid_tem);

        if (pid_tem == ppid) {target=1;}

        if (ppid_tem == ppid) {

            /*check array full*/
            if (pros[i].pid) {
                realloc(pros, 2*i*sizeof(struct process));
                i++;
            }

            pros[i].pid = pid_tem;

            fscanf(fp, "%s", s);
            if (s == R) {
                return target;
            }

            fscanf(fp, "%s", pros[i].cmd);
        } 
    }
    return target;
}