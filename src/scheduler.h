#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#define START 1
#define END 0

void scheduler (char *policy, char process[][32], \
                int ready_time[], int ready_index[], int ready_seq[], \
                int exec_time[], int exec_index[], int exec_seq[], \
                int no_of_proc);
#endif
