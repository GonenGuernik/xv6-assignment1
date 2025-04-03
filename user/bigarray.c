#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ARRAY_SIZE (1 << 16)  // 65,536 elements
#define CHILDREN 4

int main() {
    int array[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i;
    }

    int pids[CHILDREN];
    if (forkn(CHILDREN, pids) < 0) {
        printf("forkn failed\n");
        exit(1, "");
    }

    int index;
    if ((index = forkn(CHILDREN, pids)) > 0) {
        int start = (index - 1) * (ARRAY_SIZE / CHILDREN);
        int end = index * (ARRAY_SIZE / CHILDREN);
        int sum = 0;

        for (int i = start; i < end; i++) {
            sum += array[i];
        }

        printf("Child %d sum: %d\n", index, sum);
        exit(sum, "");  // Exit with sum as status
    }

    // Parent process
    int n, statuses[NPROC];
    if (waitall(&n, statuses) < 0) {
        printf("waitall failed\n");
        exit(1, "");
    }

    if (n != CHILDREN) {
        printf("Error: Expected %d children, but got %d\n", CHILDREN, n);
        exit(1, "");
    }

    int total_sum = 0;
    for (int i = 0; i < n; i++) {
        total_sum += statuses[i];
    }

    printf("Final sum: %d\n", total_sum);
    exit(0, "Computation complete");
}
