#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

asmlinkage int check_restrictions(int pid) {
    int i;
    for (i = 0; i < current->hw1_list_size; ++i) {
        if (current->hw1_restrictions_list[i].syscall_num == pid) {

            if (current->hw1_restrictions_list[i].restriction_threshold >
                current->hw1_proc_restriction_level) {
                // ToDo: enter to log
                return -ENOSYS;
            }
        }
    }

    return 0; // Success
}