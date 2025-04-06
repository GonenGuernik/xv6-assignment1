#include "kernel/types.h"
#include "user/user.h"
// #include "kernel/proc.h"
// #include "kernel/stat.h"
// #include "kernel/param.h"

#define N 4                // Number of child processes
#define ARR_SIZE (1 << 16) // 2^16 = 65536

void itoa(int num, char *buf) {
    int i = 0;
    int is_negative = 0;
  
    if (num == 0) {
      buf[i++] = '0';
      buf[i] = '\0';
      return;
    }
  
    if (num < 0) {
      is_negative = 1;
      num = -num;
    }
  
    while (num > 0) {
      buf[i++] = (num % 10) + '0';
      num /= 10;
    }
  
    if (is_negative)
      buf[i++] = '-';
  
    buf[i] = '\0';
  
    // Reverse the string
    for (int j = 0, k = i - 1; j < k; j++, k--) {
      char temp = buf[j];
      buf[j] = buf[k];
      buf[k] = temp;
    }
  }
  

int main() {
  int *arr = malloc(sizeof(int) * ARR_SIZE);
  if (!arr) {
    fprintf(2, "malloc failed\n");
    exit(1, "malloc failed\n");
  }

  // Initialize array with consecutive numbers starting from 0
  for (int i = 0; i < ARR_SIZE; i++) {
    arr[i] = i;
  }

  // Allocate space for child PIDs
  int pids[N];

  // Call forkn system call
  int result = forkn(N, pids);
  if (result < 0) {
    fprintf(2, "forkn failed\n");
    free(arr);
    exit(1, "forkn failed\n");
  }

  if (result > 0) {
    // Child process
    int idx = result - 1; // result is 1-based index
    int start = idx * (ARR_SIZE / N);
    int end = start + (ARR_SIZE / N);
    uint sum = 0;

    for (int i = start; i < end; i++) {
      sum += arr[i];
    }

    printf("Child #%d calculated sum: %d\n", idx, sum);

    char sumstr[32];
    itoa(sum, sumstr);
    // snprintf(sumstr, sizeof(sumstr), "%d", sum);
    exit(0, sumstr);
    // free(arr);
    // exit(sum % 256, "child done\n");  // exit status is limited to 0–255
  }

  // Parent process
  printf("Parent: forked %d children\n", N);

  // Prepare to collect statuses
  int finished;
  int statuses[N];
  if (waitall(&finished, statuses) < 0) {
    fprintf(2, "waitall failed\n");
    free(arr);
    exit(1, "waitall failed\n");
  }

  printf("Parent: %d children exited\n", finished);

  char msg[32];
  int status;
  int total_sum = 0;
  for (int i = 0; i < N; i++) {
    int pid = wait(&status, msg);
    if (pid > 0) {
      total_sum += atoi(msg);
      printf("Child %d sum: %s\n", pid, msg);
    }
  }
  printf("Total sum: %d\n", total_sum);

  //   int total_sum = 0;
//   for (int i = 0; i < finished; i++) {
//     printf("Child exit status (mod 256): %d\n", statuses[i]);
//     total_sum += statuses[i];  // only part of total sum due to modulo
//   }


  free(arr);
  exit(0, "parent done\n");
}