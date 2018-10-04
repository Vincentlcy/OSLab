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

int main(int argc,char *argv[]) {
    int pid;
    int intercal;

    if (argc != 2 || 3) {
        printf("Argument not enough\n");
        return 1;
    }

    /*set limit of CPU time*/
    struct rlimit limit;
    limit.rlim_cur = 600;
    limit.rlim_max = 600;