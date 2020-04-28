#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <linux/kernel.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include "list.h"
#include "utils.h"
#include "scheduler.h"

void scheduler (char *policy, char process[][32], \
                int ready_time[], int ready_index[], int ready_seq[], \
                int exec_time[], int exec_index[], int exec_seq[], \
                int no_of_proc) {

    // restrict to one cpu use
    cpu_set_t parent, child;

    // main process on 0
    CPU_ZERO(&parent);
    CPU_SET(0, &parent);
    
    // children on 1
    CPU_ZERO(&child);
    CPU_SET(1, &child);

    // main process
    if (sched_setaffinity(0, sizeof(cpu_set_t), &parent)) {
        printf("sched_setaffinity error: %s\n", strerror(errno));
        exit(1);
    }

    // set the scheduler to RR or FIFO
    struct sched_param param;
    if (policy[0] == 'R') {
        param.sched_priority = sched_get_priority_max(SCHED_RR);
    } else {
        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    }
    // for SCHED_IDLE
    struct sched_param param_idle;
    param_idle.sched_priority = 0;

    // set for all children process
    if(policy[0] == 'F'){
        if(sched_setscheduler(0, 1, &param)){
            printf("1 sched_setscheduler error: %s\n", strerror(errno));
            exit(1);
        }
    }
    if(policy[0] == 'R'){
        if(sched_setscheduler(0, 2, &param)){
            printf("1 sched_setscheduler error: %s\n", strerror(errno));
            exit(1);
        }
    }

    // ready
    pid_t pid, current_pid;
    unsigned long current_time = 0;
    struct timespec start_n, end_n;
    struct ready_queue ready, *proc, *proc_next;

    INIT_LIST_HEAD(&ready.list);

    // start
    for (unsigned long i = 0; i < no_of_proc; ++i) {
        int p = 0, finish = 0;

        while (current_time != ready_time[i]) { //wait until process come
            if((policy[0] == 'S' || policy[0] == 'P') && !list_empty(&ready.list)) //check if process terminate because it's (P)SJF, we have to decide next
                _proc_terminate_check(&ready, &param, current_time, &finish);
            wait_one_unit;
            current_time++;
        }

        proc = (struct ready_queue*)malloc(sizeof(struct ready_queue)); //add new process to queue
        if (list_empty(&ready.list)) {  // if ready queue is empty, let it start
            proc->start_time = ready_time[i];
        } else {
            proc->start_time = -1;
        }
        int index = exec_seq[ready_index[i]]; // find the execution time array index
        proc->exec_time = exec_time[index];
        list_add_tail(&(proc->list), &(ready.list)); // add to the queue

        if (policy[0] == 'S' || policy[0] == 'P') {
            if (!list_empty(&ready.list)) {
                _proc_terminate_check(&ready, &param, current_time, &finish);
                if (policy[0] == 'P') {
                    proc_next = preempt(&ready, proc, current_time, &p); //check if the process can be preempt (proc_next: be preempted)
                }
            }
        }

        //start, print the time
        syscall(334, 1, &start_n.tv_sec, &start_n.tv_nsec, &end_n.tv_sec, &end_n.tv_nsec, &current_pid);
        pid = fork();
        if (pid == 0) {
            current_pid = getpid();
            printf("%s %d\n", process[ready_index[i]], getpid());
            for(unsigned long _ = 0; _ < exec_time[index]; ++_) wait_one_unit; //execution
            syscall(334, 0, &start_n.tv_sec, &start_n.tv_nsec, &end_n.tv_sec, &end_n.tv_nsec, &current_pid); //end
            exit(0);
        } else if(pid == -1) {
            printf("Fork error!\n");
            exit(1);
        } else {
            proc->pid = pid;
            if (sched_setaffinity(pid, sizeof(cpu_set_t), &child)){ // restrict all child processes on cpu 1
                printf("sched_setaffinity error: %s\n", strerror(errno));
                exit(1);
            }
            if (finish) {
                if(sched_setscheduler(pid, SCHED_FIFO, &param)){
                    printf("policy: %d, sched_setscheduler error: %s\n", SCHED_FIFO, strerror(errno));
                    exit(1);
                }
            } else {
                if (policy[0] == 'S' || policy[0] == 'P') {
                    if (list_empty(&ready.list)){
                        if(sched_setscheduler(pid, SCHED_FIFO, &param)){ // set priority for the process
                            printf("policy: %d, sched_setscheduler error: %s\n", SCHED_FIFO, strerror(errno));
                            exit(1);
                        }
                    } else {
                        if (policy[0] == 'P' && p) { // if is PSJF and can be preempt
                            if (sched_setscheduler(pid, SCHED_FIFO, &param)) { // add to fifo
                                printf("policy: %d, sched_setscheduler error: %s\n", SCHED_FIFO, strerror(errno));
                                exit(1);
                            }
                            if (sched_setscheduler(proc_next->pid, SCHED_IDLE, &param_idle)) { //set the process (be preempted) to idle
                                printf("policy: %d, sched_setscheduler error: %s\n", SCHED_IDLE, strerror(errno));
                                exit(1);
                            }
                        } else {
                            if (list_entry(ready.list.next, struct ready_queue, list)->pid == pid) { //error check
                                if (sched_setscheduler(pid, SCHED_IDLE, &param_idle)) {
                                    printf("policy: %d, sched_setscheduler error: %s\n", SCHED_IDLE, strerror(errno));
                                    exit(1);
                                }
                            }
                        }
                    }
                }
            }
            if((policy[0] == 'S' || policy[0] == 'P') && !list_empty(&ready.list))
                _proc_terminate_check(&ready, &param, current_time, &finish);
        }
    }

    while(!list_empty(&ready.list) && (policy[0] == 'S' || policy[0] == 'P')){ //continue the remaining which are still in idle.
        _proc_remain_check(&ready, &param, &current_time);
    }

    while(wait(NULL) > 0); //wait all process finish

    exit(0);   
    
}

