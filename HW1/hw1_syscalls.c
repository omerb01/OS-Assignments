#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <wait.h>
#include "hw1_syscalls.h"
#include <stdlib.h>

#define assertTest(expression)\
    do {\
        if (!(expression)) {\
            printf("Assertion failed at %s:%d: "\
                   "in function \"%s\" "\
                   "with assertion \"%s\".\n",\
                   __FILE__, __LINE__, __func__, (#expression));\
        }\
    } while (0)

#define assertFail(index, syscall, syscall_threshold, proc_restriction)\
    do {\
        assertTest(fail[(index)].syscall_num == (syscall));\
        assertTest(fail[(index)].syscall_restriction_threshold == (syscall_threshold));\
        assertTest(fail[(index)].proc_restriction_level == (proc_restriction));\
        assertTest(fail[(index)].time > 0);\
    } while (0)

static void resetProcess();

static void restrict_fail_pidNegative();

static void restrict_fail_procPrivNegative();

static void restrict_fail_procPrivGreaterThan2();

static void restrict_fail_scrlNull();

static void restrict_fail_sizeNegative();

static void restrict_fail_returnESRCHBeforeEINVAL();

static void restrict_success_simple();

static void restrict_success_overrideScrDifferentPriv();

static void restrict_success_overrideMoreScr();

static void restrict_success_overrideLessScr();

static void restrict_success_restrictForOther();

static void restrict_success_sonHasNoRestrictions();

static void exampleTest();

static void set_proc_restriction_fail_pidNegative();

static void set_proc_restriction_fail_procPrivNegative();

static void set_proc_restriction_fail_procPrivGreaterThan2();

static void set_proc_restriction_fail_returnESRCHBeforeEINVAL();

static void set_proc_restriction_success();

static void get_process_log_fail_pidNegative();

static void get_process_log_fail_sizeMoreThanLogHas_hasNoneRequest1();

static void get_process_log_fail_sizeMoreThanLogHas_has1Request2();

static void get_process_log_fail_sizeMoreThan100();

static void get_process_log_fail_sizeNegative();

static void get_process_log_fail_returnESRCHBeforeEINVAL();

static void get_process_log_success_simple();

static void get_process_log_success_moreThan100();

static void get_process_log_success_failSorted();

static void test_zombieRetainInfo();

int main() {
    // Must succeed restrict_success_sonHasNoRestrictions() before other tests.
    restrict_success_sonHasNoRestrictions();

    resetProcess();
    restrict_fail_pidNegative();
    resetProcess();
    restrict_fail_procPrivNegative();
    resetProcess();
    restrict_fail_procPrivGreaterThan2();
    resetProcess();
    restrict_fail_scrlNull();
    resetProcess();
    restrict_fail_sizeNegative();
    resetProcess();
    restrict_fail_returnESRCHBeforeEINVAL();
    resetProcess();
    restrict_success_simple();
    resetProcess();
    restrict_success_overrideScrDifferentPriv();
    resetProcess();
    restrict_success_overrideMoreScr();
    resetProcess();
    restrict_success_overrideLessScr();
    resetProcess();
    restrict_success_restrictForOther();
    resetProcess();
    exampleTest();
    resetProcess();
    set_proc_restriction_fail_pidNegative();
    resetProcess();
    set_proc_restriction_fail_procPrivNegative();
    resetProcess();
    set_proc_restriction_fail_procPrivGreaterThan2();
    resetProcess();
    set_proc_restriction_fail_returnESRCHBeforeEINVAL();
    resetProcess();
    set_proc_restriction_success();
    resetProcess();
    get_process_log_fail_pidNegative();
    resetProcess();
    get_process_log_fail_sizeMoreThanLogHas_hasNoneRequest1();
    resetProcess();
    get_process_log_fail_sizeMoreThanLogHas_has1Request2();
    resetProcess();
    get_process_log_fail_sizeMoreThan100();
    resetProcess();
    get_process_log_fail_sizeNegative();
    resetProcess();
    get_process_log_fail_returnESRCHBeforeEINVAL();
    resetProcess();
    get_process_log_success_simple();
    resetProcess();
    get_process_log_success_moreThan100();
    resetProcess();
    get_process_log_success_failSorted();
    resetProcess();
    test_zombieRetainInfo();
    printf("End.\n");
    return 0;
}

static void resetProcess() {
    pid_t pid = fork();
    if (pid != 0) {
        exit(0);
    }
}

static void restrict_fail_pidNegative() {
    scr scrl[1];
    assertTest(sc_restrict(-1, 0, scrl, 0) == -1);
    assertTest(errno == ESRCH);
}

static void restrict_fail_procPrivNegative() {
    scr scrl[1];
    assertTest(sc_restrict(getpid(), -1, scrl, 0) == -1);
    assertTest(errno == EINVAL);
}

static void restrict_fail_procPrivGreaterThan2() {
    scr scrl[1];
    assertTest(sc_restrict(getpid(), 3, scrl, 0) == -1);
    assertTest(errno == EINVAL);
}

static void restrict_fail_scrlNull() {
    assertTest(sc_restrict(getpid(), 0, NULL, 0) == 0);
    assertTest(getpid() != -1);
}

static void restrict_fail_sizeNegative() {
    scr scrl[1];
    assertTest(sc_restrict(getpid(), 0, scrl, -1) == -1);
    assertTest(errno == EINVAL);
}

static void restrict_fail_returnESRCHBeforeEINVAL() {
    assertTest(sc_restrict(-1, -1, NULL, -1) == -1);
    assertTest(errno == ESRCH);
}

static void restrict_success_simple() {
    scr scrl[] = {{13, 0}, // 13 = time.
                  {20, 1}, // 20 = getpid.
                  {64, 2}}; // 64 = getppid.
    assertTest(sc_restrict(getpid(), 1, scrl, 3) == 0);
    assertTest(time(NULL) != -1);
    assertTest(getpid() != -1);
    assertTest(getppid() == -1);
    assertTest(errno == ENOSYS);
}

static void restrict_success_overrideScrDifferentPriv() {
    scr scrl[] = {{13, 0}, // 13 = time.
                  {20, 1}, // 20 = getpid.
                  {64, 2}}; // 64 = getppid.

    assertTest(sc_restrict(getpid(), 1, scrl, 3) == 0);
    assertTest(time(NULL) != -1);
    assertTest(getpid() != -1);
    assertTest(getppid() == -1);
    assertTest(errno == ENOSYS);

    assertTest(sc_restrict(getpid(), 2, scrl, 3) == 0);
    assertTest(time(NULL) != -1);
    assertTest(getpid() != -1);
    assertTest(getppid() != -1);
}

static void restrict_success_overrideMoreScr() {
    scr scrl1[] = {{13, 0}, // 13 = time.
                   {20, 1}}; // 20 = getpid.

    assertTest(sc_restrict(getpid(), 1, scrl1, 2) == 0);
    assertTest(time(NULL) != -1);
    assertTest(getpid() != -1);
    assertTest(getppid() != -1);

    scr scrl2[] = {{13, 0}, // 13 = time.
                   {20, 1}, // 20 = getpid.
                   {64, 2}}; // 64 = getppid.

    assertTest(sc_restrict(getpid(), 1, scrl2, 3) == 0);
    assertTest(time(NULL) != -1);
    assertTest(getpid() != -1);
    assertTest(getppid() == -1);
    assertTest(errno == ENOSYS);
}

static void restrict_success_overrideLessScr() {
    scr scrl1[] = {{13, 0}, // 13 = time.
                   {20, 1}, // 20 = getpid.
                   {64, 2}}; // 64 = getppid.

    assertTest(sc_restrict(getpid(), 1, scrl1, 3) == 0);
    assertTest(time(NULL) != -1);
    assertTest(getpid() != -1);
    assertTest(getppid() == -1);
    assertTest(errno == ENOSYS);

    scr scrl2[] = {{13, 0}, // 13 = time.
                   {20, 1}}; // 20 = getpid.

    assertTest(sc_restrict(getpid(), 1, scrl2, 2) == 0);
    assertTest(time(NULL) != -1);
    assertTest(getpid() != -1);
    assertTest(getppid() != -1);
}

static void restrict_success_restrictForOther() {
    scr scrl[] = {{13, 0}, // 13 = time.
                  {20, 1}, // 20 = getpid.
                  {64, 2}}; // 64 = getppid.

    pid_t sonsPid = fork();
    if (sonsPid == 0) {
        assertTest(sc_restrict(getppid(), 1, scrl, 3) == 0);
        exit(0);
    } else {
        waitpid(sonsPid, NULL, 0);
        assertTest(time(NULL) != -1);
        assertTest(getpid() != -1);
        assertTest(getppid() == -1);
        assertTest(errno == ENOSYS);
    }
}

static void restrict_success_sonHasNoRestrictions() {
    scr scrl[] = {{13, 2}, // 13 = time.
                  {20, 1}, // 20 = getpid.
                  {64, 0}}; // 64 = getppid.

    assertTest(sc_restrict(getpid(), 1, scrl, 3) == 0);

    pid_t sonPid = fork();
    if (sonPid == 0) {
        assertTest(getpid() != -1);
        assertTest(getppid() != -1);
        assertTest(time(NULL) != -1);
        exit(0);
    } else {
        assertTest(getppid() != -1);
        assertTest(getpid() != -1);
        assertTest(time(NULL) == -1);
        assertTest(errno == ENOSYS);
    }
}

static void exampleTest() {
    pid_t pid = getpid();
    scr scrl = {20, 2}; // 20 = getpid.
    assertTest(sc_restrict(pid, 0, &scrl, 1) == 0);
    assertTest(getpid() == -1);
    assertTest(errno == ENOSYS);
    assertTest(set_proc_restriction(pid, 2) == 0);
    assertTest(getpid() != -1);
}

static void set_proc_restriction_fail_pidNegative() {
    assertTest(set_proc_restriction(-1, 0) == -1);
    assertTest(errno == ESRCH);
}

static void set_proc_restriction_fail_procPrivNegative() {
    assertTest(set_proc_restriction(getpid(), -1) == -1);
    assertTest(errno == EINVAL);
}

static void set_proc_restriction_fail_procPrivGreaterThan2() {
    assertTest(set_proc_restriction(getpid(), 3) == -1);
    assertTest(errno == EINVAL);
}

static void set_proc_restriction_fail_returnESRCHBeforeEINVAL() {
    assertTest(set_proc_restriction(-1, -1) == -1);
    assertTest(errno == ESRCH);
}

static void set_proc_restriction_success() {
    scr scrl[1];
    assertTest(set_proc_restriction(getpid(), 0) == 0);
    assertTest(sc_restrict(getpid(), 1, scrl, 0) == 0);
    assertTest(set_proc_restriction(getpid(), 2) == 0);
}

static void get_process_log_fail_pidNegative() {
    fai fail[1];
    assertTest(get_process_log(-1, 0, fail) == -1);
    assertTest(errno == ESRCH);
}

static void get_process_log_fail_sizeMoreThanLogHas_hasNoneRequest1() {
    fai fail[1];
    assertTest(get_process_log(getpid(), 1, fail) == -1);
    assertTest(errno == EINVAL);
}

static void get_process_log_fail_sizeMoreThanLogHas_has1Request2() {
    fai fail[2];
    scr scrl[] = {{13, 1}}; // 13 = time.

    assertTest(sc_restrict(getpid(), 0, scrl, 1) == 0);
    assertTest(time(NULL) == -1);

    assertTest(get_process_log(getpid(), 2, fail) == -1);
    assertTest(errno == EINVAL);
}

static void get_process_log_fail_sizeMoreThan100() {
    fai fail[101];
    assertTest(get_process_log(getpid(), 101, fail) == -1);
    assertTest(errno == EINVAL);
}

static void get_process_log_fail_sizeNegative() {
    fai fail[1];
    assertTest(get_process_log(getpid(), -1, fail) == -1);
    assertTest(errno == EINVAL);
}

static void get_process_log_fail_returnESRCHBeforeEINVAL() {
    fai fail[1];
    assertTest(get_process_log(-1, -1, fail) == -1);
    assertTest(errno == ESRCH);
}

static void get_process_log_success_simple() {
    fai fail[5];
    pid_t pid = getpid();
    scr scrl[] = {{13, 0}, // 13 = time.
                  {20, 1}, // 20 = getpid.
                  {64, 2}}; // 64 = getppid.

    assertTest(sc_restrict(pid, 1, scrl, 3) == 0);
    assertTest(get_process_log(pid, 0, fail) == 0);

    assertTest(time(NULL) != -1);
    assertTest(getpid() != -1);
    assertTest(getppid() == -1);
    assertTest(errno == ENOSYS);
    assertTest(getppid() == -1);
    assertTest(errno == ENOSYS);
    assertTest(get_process_log(pid, 2, fail) == 0);

    assertFail(0, 64, 2, 1);
    assertFail(1, 64, 2, 1);

    assertTest(set_proc_restriction(pid, 0) == 0);
    assertTest(time(NULL) != -1);
    assertTest(getpid() == -1);
    assertTest(errno == ENOSYS);
    assertTest(getppid() == -1);
    assertTest(errno == ENOSYS);
    assertTest(getpid() == -1);
    assertTest(errno == ENOSYS);

    assertTest(get_process_log(pid, 2, fail) == 0);
    assertFail(0, 64, 2, 0);
    assertFail(1, 20, 1, 0);

    assertTest(get_process_log(pid, 5, fail) == 0);
    assertFail(0, 64, 2, 1);
    assertFail(1, 64, 2, 1);
    assertFail(2, 20, 1, 0);
    assertFail(3, 64, 2, 0);
    assertFail(4, 20, 1, 0);
}

static void get_process_log_success_moreThan100() {
    int i;
    pid_t pid = getpid();
    fai fail[100];
    scr scrl[] = {{13, 2}, // 13 = time.
                  {20, 1}, // 20 = getpid.
                  {64, 0}}; // 64 = getppid.

    assertTest(sc_restrict(pid, 0, scrl, 3) == 0);
    for (i = 0; i < 60; i++) {
        assertTest(time(NULL) == -1);
    }
    for (i = 0; i < 60; i++) {
        assertTest(getpid() == -1);
    }

    // 40 of time and 60 of getpid.
    assertTest(get_process_log(pid, 100, fail) == 0);
    for (i = 0; i < 100; i++) {
        if (i < 40) {
            assertFail(i, 13, 2, 0);
        } else {
            assertFail(i, 20, 1, 0);
        }
    }
}

static void get_process_log_success_failSorted() {
    pid_t pid = getpid();
    fai fail[5];
    scr scrl[] = {{13, 2}, // 13 = time.
                  {20, 2}, // 20 = getpid.
                  {64, 2}}; // 64 = getppid.

    assertTest(sc_restrict(pid, 0, scrl, 3) == 0);

    assertTest(getpid() == -1);
    assertTest(time(NULL) == -1);
    assertTest(getppid() == -1);
    assertTest(getpid() == -1);

    assertTest(get_process_log(pid, 3, fail) == 0);
    assertFail(0, 13, 2, 0);
    assertFail(1, 64, 2, 0);
    assertFail(2, 20, 2, 0);

    assertTest(fail[0].time <= fail[1].time);
    assertTest(fail[0].time <= fail[2].time);
    assertTest(fail[1].time <= fail[2].time);
}

static void test_zombieRetainInfo() {
    pid_t sonPid;
    fai fail[2];
    sonPid = fork();
    if (sonPid == 0) {

        scr scrl[] = {{13, 2}}; // 13 = time.
        assertTest(sc_restrict(getpid(), 1, scrl, 1) == 0);
        assertTest(time(NULL) == -1);
        assertTest(time(NULL) == -1);
        exit(0);

    } else {
        sleep(1);
        assertTest(get_process_log(sonPid, 2, fail) == 0);
        assertFail(0, 13, 2, 1);
        assertFail(1, 13, 2, 1);
    }
}
