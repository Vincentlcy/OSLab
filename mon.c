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
    int index
}

int main(int argc,char *argv[]) {
    /*set pid and interval time*/
    int pid = argv[1];
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

    while (1) {

        FILE *fp;
        
        /*open pipeline*/
        if ((fp = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r") == NULL) {
            printf("Error in open the pipeline");
        }

        /*read*/
        while (1) {
            int pid
            fscanf(" %d", pid)
        }
    }


