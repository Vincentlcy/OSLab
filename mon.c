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

int find_child(int ppid, FILE *fp, struct process *pros, int print_key);

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
    int target = 0;
    pros = calloc(20, sizeof(struct process));

    while (1) {
        printf("almon [counter= %d, pid= %d, target_pid= %d, interval= %d sec]:\n", counter, pid, ppid, interval);

        FILE *fp;
	
        /*open pipeline*/
        if ((fp = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r")) == NULL) {
            printf("Error in open the pipeline");
        }

        /*Find first child*/
        int t = find_child(ppid, fp, pros, 1);
	fclose(fp);
	target+=t; 
	if (t==0 && target==0){
	    printf("No such process\n");
	    return 1;
	}
	
        printf("[");
        for (int i=0;;i++) {
            if (pros[i].pid == 0) {break;}
            /*open pipeline*/
            if ((fp = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r")) == NULL) {
            printf("Error in open the pipeline");
	    fclose(fp);
        }
            find_child(pros[i].pid, fp, pros,0);
            printf("%d:[%d,%s], ",i,pros[i].pid,pros[i].cmd);
	    fclose(fp);
        }
        printf("]\n");

        /*terminated*/
        if (target>0 && t==0) {
            break;
        }
        sleep(interval);
    }

    printf("almon: target appears to have terminated; cleaning up\n");
    for (int i=0;;i++) {
            if (pros[i].pid == 0) {break;}
            kill(pros[i].pid, SIGKILL);
            printf("terminating [%d,%s]\n",pros[i].pid,pros[i].cmd);
    }
    free(pros);
    printf("exiting almon\n");
    return 0;
}


int find_child(int ppid, FILE *fp, struct process *pros, int print_key) {
    int pid_tem;
    int ppid_tem;
    int i = 0;
    /*s for last line target for target process 1 for terminate*/
    char cmd[100];
    int target = 0;
    char tep[1024];
    int check = 0;

    fgets(tep,256,fp);
    if (print_key) {printf("%s\n",tep);}
    while (fgets(tep,256,fp)!=NULL) {
        /*scanf the pid*/
	if (print_key) {printf("%s\n",tep);}
        char *token;
        char *s = " ";
        char *sp = NULL;
        token = strtok_r(tep, s, &sp);
  	token = strtok_r(NULL, s, &sp);
	pid_tem = strtol(token,NULL,10);
  	token = strtok_r(NULL, s, &sp);
	ppid_tem = strtol(token,NULL,10);
    	token = strtok_r(NULL, s, &sp);
 	token = strtok_r(NULL, s, &sp);
 	token = strtok_r(NULL, s, &sp);
	token = strtok_r(token, "\n", &sp);
	strcpy(cmd,token);

        if (pid_tem == ppid) {
	    target = 1; 
	}

	
        if (ppid_tem == ppid) {

           /*check array full*/
            	if ((i+2)%20 == 0) {
                    pros = realloc(pros, (i+22)*sizeof(struct process));
                    i++;
                }
                pros[i].pid = pid_tem;
	        strcpy(pros[i].cmd,cmd);
		i++;
        }
	if (i > 0 && pid_tem == pros[i-1].pid) {
	    check = 1;
	}
    }
    if (i > 0) {
	pros[i].pid =0;
    }
    
    if (check == 0&& i>0) {
	pros[0].pid = 0;
    }
    if (check == 0 && target == 0) {
	pros[0].pid = 0;
    }

    return target;
}
