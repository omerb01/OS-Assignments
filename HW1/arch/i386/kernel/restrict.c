#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

asmlinkage int check_restrictions(int syscall_num) {

    int i;
    int m_syscall_num, m_restriction_threshold, proc_restriction_level;

    for (i = 0; i < current->hw1_list_size; ++i) {
        m_syscall_num = current->hw1_restrictions_list[i].syscall_num;
        m_restriction_threshold = current->hw1_restrictions_list[i].restriction_threshold;
        proc_restriction_level = current->hw1_proc_restriction_level;

        if (m_syscall_num == syscall_num) {

            if (m_restriction_threshold > proc_restriction_level) {

                // we wrote to the final place in the last iteration
                if (current->hw1_curr_log_index >= 99) {
                    current->hw1_curr_log_index = 0;
                } else {
                    current->hw1_curr_log_index++;
                }

                int j = current->hw1_curr_log_index; // just for readable code
                current->hw1_logs_array[j].syscall_num                   = syscall_num;
                current->hw1_logs_array[j].syscall_restriction_threshold = m_restriction_threshold;
                current->hw1_logs_array[j].proc_restriction_level        = proc_restriction_level;
                current->hw1_logs_array[j].time                          = jiffies;

                if (current->hw1_logs_array_size < 100) {
                    current->hw1_logs_array_size++;
                }

                return -ENOSYS;
            }
        }
    }

    return 0; // Success
}