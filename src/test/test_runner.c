#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int run_test(char *name);

int main(int argc, char **argv) {
    int failing_tests = 0;
    int total_tests   = argc - 1;
    printf("Running %d tests...\n", argc - 1);
    for (int i = 1; i < argc; i++) {
        printf("Running test: %s\n", argv[i]);
        if (run_test(argv[i]) != 0) {
            printf("Test %s failed!\n", argv[i]);
            failing_tests++;
        }
    }
    printf("Tests: %d, Successful: %d, Failed: %d\n", total_tests, total_tests - failing_tests, failing_tests);
    return failing_tests;
}

int run_test(char *name) {
    pid_t pid = fork();
    if (pid == 0) {
        // fork and run test
        return execv(name, NULL);
    } else {
        // let parent wait for child
        int ret = 0;
        waitpid(pid, &ret, 0);
        if (WIFEXITED(ret)) {
            return WEXITSTATUS(ret);
        }
        return -1;
    }
}
