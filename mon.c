/*All Copyright Reserve*/
/*2018 OCT 4th         */

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
    /*set interval*/
    int interval = 3;

    if (argc != 2 && argc != 3) {
        printf("Argument not enough\n");
        return 1;
    } else if (argc == 3){
        /*update interval*/
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

    /*counter for times of loop; pid for self pid; pros for pointer to array of process; */
    int counter = 1;
    int pid = getpid();
    struct process *pros;
    pros = calloc(20, sizeof(struct process));

    while (1) {
        printf("almon [counter= %d, pid= %d, target_pid= %d, interval= %d sec]:\n", counter++, pid, ppid, interval);

        FILE *fp, *f;
	
        /*open pipeline*/
        if ((fp = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r")) == NULL) {
            printf("Error in open the pipeline");
        }
        

        /*Find first layer child; t for target exist(1) or not(0)*/
        int t = find_child(ppid, fp, pros, 1);
	    fclose(fp);
	    if (t==0 && counter==2){
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
            /*Find child for child*/
            find_child(pros[i].pid, fp, pros,0);
            printf("%d:[%d,%s], ",i,pros[i].pid,pros[i].cmd);
	        fclose(fp);
        }
        printf("]\n");

        /*terminated*/
        if (counter>2 && t==0) {
            break;
        }

        sleep(interval);
    }

    /*Tagert terminated; terminating child processes*/
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
    /*pros: array of process; print_key: print the fp(1); target: target exist(1); check: child still rum(1)*/
    int pid_tem, ppid_tem;
    int i = 0, target = 0, check = 0;
    char cmd[100], tep[1024];

    /*get a line, print if print_key*/
    fgets(tep,256,fp);
    if (print_key) {printf("%s\n",tep);}

    while (fgets(tep,256,fp)!=NULL) {
        /*print each line if print key*/
	    if (print_key) {
            printf("%s\n",tep);
        }

        /*divide the line*/
        char *token;
        char *s = " ";
        char *sp = NULL;

        token = strtok_r(tep, s, &sp); // User name

        token = strtok_r(NULL, s, &sp); // pid
        pid_tem = strtol(token,NULL,10);

        token = strtok_r(NULL, s, &sp); // ppid
        ppid_tem = strtol(token,NULL,10);

    	token = strtok_r(NULL, s, &sp); // time
        token = strtok_r(NULL, s, &sp); // state

        token = strtok_r(NULL, s, &sp); // process
        token = strtok_r(token, "\n", &sp); // remove \n
        strcpy(cmd,token);

        if (pid_tem == ppid) {
	        target = 1; // if target exist
	    }

        /*if target not exist, array will not update*/
        if (ppid_tem == ppid) {
           /*check array full*/
            if ((i+2)%20 == 0) {
                pros = realloc(pros, (i+22)*sizeof(struct process));
                i++;
            }
            /*record child process*/
            pros[i].pid = pid_tem;
	        strcpy(pros[i].cmd,cmd);
		    i++;
        }
	
	/*Since target doesnot exist, the ppid for child tend to 1, the upper
	 * check will not useable, so check each child that still run or not*/
	if (target == 0 && print_key ==1) {
	    for (int j=0;pros[j].pid!=0;j++) {
	    	if (pid_tem == pros[j].pid) {
		    pros[i].pid = pros[j].pid;
		    strcpy(pros[i].cmd,pros[j].cmd);
		    i++;
		}
	    }
	}
	    if (pid_tem == pros[0].pid) {
		check =1; // check child exist or not
	    }
    }

    if (i > 0) {
	pros[i].pid =0; //All process still run is in postion < i; discard rest
    }

    if (check == 0&& i>0) {
	pros[0].pid = 0; //Target exist but all child terminated
    }

    if (check == 0 && target == 0) {
	pros[0].pid = 0; //Target exit with all child terminated
    }

    return target;
}
