#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "utils.h"
#include "list.h"

int compare(const void *a, const void *b) {
    return (*(int*)a-*(int*)b);
}

size_t findindex(const int a[], size_t size, int value) {
    size_t index = 0;
    while (index < size && a[index] != value)
        ++index;
    return (index == size ? -1 : index);
}

void sort(int arr[], int index[], int N){
    int tmp[N];
    memcpy(tmp, arr, N*sizeof(int));
    qsort(arr, N, sizeof(int), compare);
    for (size_t i = 0; i < N; i++) {
        index[i] = findindex(tmp, N, arr[i]);
        tmp[index[i]] = -1;
    }
}

void inverse_permutation(const int a[], int b[], int N){
	for (int i = 0; i < N; i++) {
        b[a[i]] = i;
    }
}

struct ready_queue *get_shortest_proc (struct ready_queue *ready) {
	long long shortest = LLONG_MAX; 
	struct list_head *shortest_pos; //record the shortest time process
    struct list_head *current_pos;
	struct ready_queue *proc;
	list_for_each(current_pos, &(ready->list)){ //iterate the ready queue
		proc = list_entry(current_pos, struct ready_queue, list);
		if ( proc->exec_time < shortest ) {
			shortest = proc->exec_time;
			shortest_pos = current_pos;
		}
	}
	proc = list_entry(shortest_pos, struct ready_queue, list); //get the shortest process
	list_del(&(proc->list));
	list_add(&(proc->list), &(ready->list)); //add to the front
	
	return proc;
}


void _proc_terminate_check (struct ready_queue *ready, \
                            struct sched_param *param, \
                            unsigned long current_time, \
                            int *finish){

	struct ready_queue *proc;
	proc = list_entry(ready->list.next, struct ready_queue, list); // get the running process
	if (proc->start_time + proc->exec_time == current_time) { //check terminate
		list_del(&(proc->list)); //remove
		if (!list_empty(&ready->list)) {
			proc = get_shortest_proc(ready); //find next process
			proc->start_time = current_time; //let it start
			if (proc->pid) {
		 		if (sched_setscheduler(proc->pid, SCHED_FIFO, param)) {
					printf("sched_setscheduler error: %s\n", strerror(errno));
					exit(1);
				}
			} else {
				*finish = 1; //terminate
			}
		}
	}
}

struct ready_queue *preempt (struct ready_queue *ready, \
                             struct ready_queue *proc, \
                             unsigned long current_time, \
                             int *p) {

	struct ready_queue *proc_next;
	proc_next = list_entry(ready->list.next, struct ready_queue, list); // get the next entry
	if (proc != proc_next) {
		if(proc_next->start_time + proc_next->exec_time > proc->exec_time + current_time){ // the process can preept
			proc_next->exec_time = proc_next->start_time + proc_next->exec_time - current_time; //how long to exec
			proc_next->start_time = -1;
			*p = 1;
			list_del(&(proc->list));
			list_add(&(proc->list), &(ready->list));
			proc->start_time = current_time;
		}
	} else{
		*p = 1;
	}
	return proc_next;
}

void _proc_remain_check (struct ready_queue *ready, \
                         struct sched_param *param, \
                         unsigned long *current_time) {
    
	struct ready_queue *proc;
	proc = list_entry(ready->list.next, struct ready_queue, list);
	while (*current_time < proc->start_time + proc->exec_time) { // if not finish, run
		wait_one_unit;
		++(*current_time);
	}
	list_del(&(proc->list)); //the process done and to be removed
	if (!list_empty(&ready->list)) { // check reeady queue
		proc = get_shortest_proc(ready); // find next to exec
		proc->start_time = *current_time; //let it start
		if (sched_setscheduler(proc->pid, SCHED_FIFO, param)) { //FIFO
			printf("sched_setscheduler error: %s\n", strerror(errno));
			exit(1);
		}
	}
}

