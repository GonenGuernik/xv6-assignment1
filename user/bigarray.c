#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define N 4                // Number of child processes
#define ARR_SIZE (1 << 16) // 2^16 = 65536
  


int main() {
  int *arr = malloc(sizeof(int) * ARR_SIZE);
  if (!arr) {
    fprintf(2, "malloc failed\n");
    exit(1, "malloc failed\n");
  }

  // Initialize the array
  for (int i = 0; i < ARR_SIZE; i++) {
    arr[i] = i;
  }

  int result;
  int pids[N];
  int statuses[NPROC];
  int total_sum = 0;
  int finished = 0;

  result = forkn(N, pids);

  if (result < 0) {
    fprintf(2, "forkn failed\n");
    free(arr);
    exit(1, "forkn failed\n");
  }

  if (result == 0) {
    // Parent
    if (waitall(&finished, statuses) < 0) {
      free(arr);
      exit(1, "waitall failed\n");
    }

    printf("Parent: %d children exited\n", finished);
    for (int i = 0; i < finished; i++) {
      total_sum += statuses[i];
    }

    int mod = ARR_SIZE % N;
    for(int i =ARR_SIZE-1;i > ARR_SIZE-1-mod; i-- ) //if ARR_SIZE is not divided by N
      total_sum+= i;

    printf("Total sum: %d\n", total_sum);
    free(arr);
    exit(0, "parent done\n");
  } else {
    // Child: result is from 1 to 
    printf("In child process %d\n", result);
    int index = result - 1;
    int start = index * (ARR_SIZE / N);
    int end = (index + 1) * (ARR_SIZE / N);
    int part_sum = 0;

    for (int i = start; i < end; i++) {
      part_sum += arr[i];
    }

    printf("Partial sum of child number %d is: %d\n", result, part_sum);
    exit(part_sum, "");
  }
}
