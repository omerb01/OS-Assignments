
#include <errno.h>
#include <termios.h>

#ifndef HW2_SYSCALLS_H_
#define HW2_SYSCALLS_H_


struct sched_param {
	int sched_priority; //ignored for SHORT processes
	int requested_time; //between 1 and 3000
	int sched_short_prio; //between 0 and 139
};

#define SCHED_OTHER		0
#define SCHED_FIFO		1
#define SCHED_RR		2
#define SCHED_SHORT     5



int is_short ( pid_t pid) {

	 unsigned int res;
	 __asm__(
		 "int $0x80;"
		 : "=a" (res)
		 : "0" (243) ,"b" (pid)
		 : "memory"
	 );
	 if (res >= (unsigned long)(-125))
	 {
		 errno = -res;
		 res = -1;
	 }
	 return (int) res;
}
int short_remaining_time(pid_t pid){
		 unsigned int res;
	 __asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (244) ,"b" (pid)
		: "memory"
	 );
	 if (res >= (unsigned long)(-125))
	 {
		errno = -res;
		res = -1;
	 }
 return (int) res;
}
int short_place_in_queue(pid_t pid){
	 unsigned int res;
	 __asm__(
		"int $0x80;"
		: "=a" (res)
		: "0" (245) ,"b" (pid)
		: "memory"
	 );
	 if (res >= (unsigned long)(-125))
	 {
		errno = -res;
		res = -1;
	 }
 return (int) res;
 
}

#endif 
