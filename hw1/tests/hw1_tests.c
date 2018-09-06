#include <criterion/criterion.h>
#include <criterion/logging.h>
#include "hw1.h"

Test(hw1_tests_suite, validargs_help_test) {
    int argc = 2;
    char *argv[] = {"bin/audible", "-h", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = 1;
    unsigned long opt = global_options;
    unsigned long flag = 0x8000000000000000L;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
		 ret, exp_ret);
    cr_assert_eq(opt & flag, flag, "Correct bit (0x800000000000) not set for -h. Got: %lx", opt);
}

Test(hw1_tests_suite, validargs_speedup_test) {
    int argc = 4;
    char *argv[] = {"bin/audible", "-u", "-f", "2", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = 1;
    unsigned long opt = global_options;
    unsigned long flag = 0x4000000000000000L;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
		 ret, exp_ret);
    cr_assert_eq(opt & flag, flag, "Speedup mode bit wasn't set. Got: %lx", opt);

}

Test(hw1_tests_suite, help_system_test) {
    char *cmd = "bin/audible -h";

    // system is a syscall defined in stdlib.h
    // it takes a shell command as a string and runs it
    // we use WEXITSTATUS to get the return code from the run
    // use 'man 3 system' to find out more
    int return_code = WEXITSTATUS(system(cmd));

    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with %d instead of EXIT_SUCCESS",
		 return_code);
}
