#ifndef UTILS_H_
#define UTILS_H_

#include <errno.h>
#include <sched.h>
#include "list.h"

//#define wait_unit(unit) ({ for (size_t i = 0; i < unit; ++i) wait_one_unit; })
#define wait_one_unit ({ volatile unsigned long i; for (i = 0; i < 1000000UL; i++); })

struct ready_queue{
	struct list_head list;
	long start_time;
    long exec_time;
	pid_t pid;
};

int compare(const void *a, const void *b);
size_t findindex(const int a[], size_t size, int value);
void sort(int value[], int index[], int N);
void inverse_permutation (const int a[], int b[], int N);
struct ready_queue *get_shortest_proc (struct ready_queue *ready);
void _proc_terminate_check (struct ready_queue *ready, \
                            struct sched_param *param, \
                            unsigned long current_time, \
                            int *finish);
struct ready_queue *preempt(struct ready_queue *ready, \
                            struct ready_queue *proc, \
                            unsigned long current_time, \
                            int *p);
void _proc_remain_check (struct ready_queue *ready, \
                         struct sched_param *param, \
                         unsigned long *current);

#endif
