#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

int sc_restrict(pid_t pid, int proc_restriction_level, scr *restrictions_list, int list_size) {

    if (pid < 0) {
        return -ESRCH;
    }

    task_t *proc = find_task_by_pid(pid);
    if (proc == NULL) {
        return -ESRCH;
    }

    if (proc_restriction_level < 0 || proc_restriction_level > 2) {
        return -EINVAL;
    }

    if (list_size < 0) {
        return -EINVAL;
    }

    proc->hw1_proc_restriction_level = proc_restriction_level;

    scr *sys_restrictions_list = kmalloc(sizeof(scr) * list_size, GFP_KERNEL);
    if (sys_restrictions_list == NULL) {
        return -ENOMEM;
    }

    int func_res;
    func_res = copy_from_user(sys_restrictions_list, restrictions_list, sizeof(scr) * list_size);
    if (func_res != 0) {
        kfree(sys_restrictions_list);
        proc->hw1_list_size = -1;
        return -ENOMEM;
    }
    proc->hw1_list_size = list_size;

    if (proc->hw1_restrictions_list != NULL) {
        kfree(proc->hw1_restrictions_list);
        proc->hw1_restrictions_list = NULL;
    }

    proc->hw1_restrictions_list = sys_restrictions_list;

    fai *log_arr = kmalloc(sizeof(fai) * LOG_ARR_SIZE, GFP_KERNEL);
    if (log_arr == NULL) {
        kfree(proc->hw1_restrictions_list);
        proc->hw1_restrictions_list = NULL;
        proc->hw1_list_size = -1;
        return -ENOMEM;
    }

    if (proc->hw1_logs_array != NULL) {
        kfree(proc->hw1_logs_array);
        proc->hw1_logs_array = NULL;
        proc->hw1_logs_array_size = -1;
        proc->hw1_curr_log_index = -1;
    }

    proc->hw1_logs_array = log_arr;
    proc->hw1_logs_array_size = 0;
    proc->hw1_curr_log_index = -1;

    return 0; // SUCCESS
}

int sys_set_proc_restriction(pid_t pid, int proc_restriction_level) {

    if (pid < 0) {
        return -ESRCH;
    }

    task_t *proc = find_task_by_pid(pid);
    if (proc == NULL) {
        return -ESRCH;
    }

    if (proc_restriction_level < 0 || proc_restriction_level > 2) {
        return -EINVAL;
    }

    proc->hw1_proc_restriction_level = proc_restriction_level;

    return 0; // SUCCESS
}

int sys_get_process_log(pid_t pid, int size, fai *user_mem) {

    if (pid < 0) {
        return -ESRCH;
    }

    task_t *proc = find_task_by_pid(pid);
    if (proc == NULL) {
        return -ESRCH;
    }

    if (size > proc->hw1_logs_array_size) {
        return -EINVAL;
    }

    if (size < 0) {
        return -EINVAL;
    }

    fai res_arr[size];
    int index = proc->hw1_curr_log_index;

    int i;
    for (i = 0; i < size; i++) {

        if (index <= 0) {
            index = 99;
        }

        res_arr[size - i - 1].syscall_num                   = proc->hw1_logs_array[index].syscall_num;
        res_arr[size - i - 1].syscall_restriction_threshold = proc->hw1_logs_array[index].syscall_restriction_threshold;
        res_arr[size - i - 1].proc_restriction_level        = proc->hw1_logs_array[index].proc_restriction_level;
        res_arr[size - i - 1].time                          = proc->hw1_logs_array[index].time;
        index--;
    }

    int func_res;
    func_res = copy_to_user(user_mem, res_arr, sizeof(fai) * size);
    if (func_res != 0) {
        return -ENOMEM;
    }

    return 0;
}