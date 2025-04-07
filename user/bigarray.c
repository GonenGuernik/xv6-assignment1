/*
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

*/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

#define SIZE 65536  // 2^16
#define NUM_CHILDREN 4

// Function to calculate the sum of a portion of the array
void sum_array(int start, int end, int *result) {
    *result = 0;
    for (int i = start; i < end; i++) {
        *result += i;
    }
}

// Function to convert an integer to a string (manual implementation)
void itoa(int n, char *str) {
  int i = 0;
  int sign = n;
  if (n < 0) {
      n = -n;
  }

  // Generate the digits in reverse order
  do {
      str[i++] = n % 10 + '0'; // Get next digit
  } while ((n /= 10) > 0);

  // Append negative sign if the number was negative
  if (sign < 0) {
      str[i++] = '-';
  }

  str[i] = '\0'; // Null terminate the string

  // Reverse the string
  int start = 0;
  int end = i - 1;
  while (start < end) {
      char temp = str[start];
      str[start] = str[end];
      str[end] = temp;
      start++;
      end--;
  }
}

int main() {
    int pids[NUM_CHILDREN];
    int statuses[NUM_CHILDREN];
    char lower_half_msgs[NUM_CHILDREN][10];  // To store the lower half of each sum as a string
    int sum = 0;

    // Use a variable to store the number of children
    int num_children = NUM_CHILDREN;

    // Fork 4 child processes to compute partial sums
    int pid_count = forkn(NUM_CHILDREN, pids);  // Create NUM_CHILDREN child processes

    if (pid_count == 0) {
      // Each child calculates its portion of the array
      for (int i = 0; i < NUM_CHILDREN; i++) {
          if (pids[i] == 0) {
              // Calculate partial sum for this child
              int start = i * (SIZE / NUM_CHILDREN);
              int end = (i + 1) * (SIZE / NUM_CHILDREN);
              int partial_sum = 0;
              sum_array(start, end, &partial_sum);

              // Split the partial sum into two parts
              int upper_half = partial_sum >> 16; // Top 16 bits
              int lower_half = partial_sum & 0xFFFF; // Bottom 16 bits

              // Convert the lower half into a string (message)
              itoa(lower_half, lower_half_msgs[i]); // Store lower_half as a string

              // Exit with the upper_half in the exit code and lower_half as the message
              exit(upper_half, lower_half_msgs[i]); // Pass string message correctly
          }
      }
  }

  // Parent waits for all child processes to finish
  waitall(&num_children, statuses);

  // Calculate the total sum from the partial sums
  for (int i = 0; i < NUM_CHILDREN; i++) {
      // Extract the upper_half (from exit code) and lower_half (from message)
      int upper_half = statuses[i] << 16; // Get the upper 16 bits (from exit code) and shift
      int lower_half = atoi(lower_half_msgs[i]);  // Convert the message back to an integer

      sum += (upper_half + lower_half);  // Reconstruct the full sum
  }

  // Print the final sum
  printf(1, "Total sum: %d\n", sum);

  exit(0, 0);  // Parent exits with status 0
}