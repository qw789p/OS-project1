#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>

asmlinkage int sys_my_time(int isStart, 
			   unsigned long *start_s, unsigned long *start_ns,
			   unsigned long *end_s, unsigned long *end_ns, 
			   int *pid) {
  struct timespec t;
  getnstimeofday(&t);

  if (isStart) {
    *start_s = t.tv_sec;
    *start_ns = t.tv_nsec;
  } else {
    *end_s = t.tv_sec;
    *end_ns = t.tv_nsec;
    printk(
        "[project1] %d %lu.%09lu %lu.%09lu\n",
        *pid, *start_s, *start_ns, *end_s, *end_ns);
  }

  return 0;
}
