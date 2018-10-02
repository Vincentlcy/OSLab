/*Assignment1*/

# include <stdio.h>
# include <unistd.h>
# include <sys/time.h>
# include <sys/resource.h>

# define MAX_JOBS 32
/*# define CLOCKS_PER_SEC 1000000l*/

int a1jobs(void) {
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
    clock_t time_begin = times($tmp);

    while (true) {
        char command[10], pgm[50], arg1[5], arg2[5], arg3[5], arg4[5];
        /*loop and wait for user command*/
        printf("a1jobs[%d]:",pid);
        scanf("%s ", &command); /*Just the command*/

        /*fix the case*/
        switch(command) {
            case 'list':
                scanf("%*[^\n]"); /*skip rest of the line*/

                continue;
            case 'run':
                /*get pgm and args*/
                scanf("%s", pgm);

                /*check legal or not*/
                int amount=0;
                char *p = ch;
                while (*p != '\0') {
                    if (*p == ' ') {
                        amount++;
                    }
                    ptr++;
                }
                if (amount > 4) {
                    printf("Fail to recgnized the argument\n");
                    continue;
                }

                /*fork*/
                pid_t = fpid;
                fpid = fork();
                if (fpid < 0) {
                    printf("Error in fork\n");
                    continue;
                }
                else if ()



                continue;
            case 'suspend':
                scanf("%*[^\n]"); /*skip rest of the line*/
                continue;
            case 'resume':
                scanf("%*[^\n]"); /*skip rest of the line*/
                continue;
            case 'terminate':
                scanf("%*[^\n]"); /*skip rest of the line*/
                continue;
            case 'exit':
                scanf("%*[^\n]"); /*skip rest of the line*/
                break;
            case 'quit':
                scanf("%*[^\n]"); /*skip rest of the line*/
                return;
            default:
                printf("Fail to recgnized the command");
                continue;
        }

        /*break out loop for exit*/
        break; 

        /*continue jump to here*/
    }





}