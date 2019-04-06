#ifndef OS_ASSIGNMENTS_HW1_SYSCALLS_H
#define OS_ASSIGNMENTS_HW1_SYSCALLS_H

#include <errno.h>

#define LOG_ARR_SIZE 100

int sc_restrict(pid_t pid, int proc_restriction_level, scr *restrictions_list, int list_size) {
    int res;

    __asm__(
    "int $0x80;"
    : "=a" (res)
    : "0" (243) ,"b" (pid) ,"c" (proc_restriction_level), "d" (restrictions_list), "e" (list_size)
    : "memory"
    );

    if ((res) < 0){
        errno = (-res);
        return -1;
    }
    return res;
}

int sys_set_proc_restriction(pid_t pid, int proc_restriction_level) {
    int res;

    __asm__(
    "int $0x80;"
    : "=a" (res)
    : "0" (244) ,"b" (pid), "c" (proc_restriction_level)
    : "memory"
    );

    if ((res) < 0){
        errno = (-res);
        return -1;
    }
    return res;
}

int sys_get_process_log(pid_t pid, int size, fai *user_mem) {
    int res;

    __asm__(
    "int $0x80;"
    : "=a" (res)
    : "0" (245) ,"b" (pid) ,"c" (size), "d" (user_mem)
    : "memory"
    );

    if ((res) < 0){
        errno = (-res);
        return -1;
    }
    return res;
}





#endif //OS_ASSIGNMENTS_HW1_SYSCALLS_H
