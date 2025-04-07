#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"
// #include "kernel/proc.h"
// #include "kernel/stat.h"

#define N 4                // Number of child processes
#define ARR_SIZE (1 << 16) // 2^16 = 65536
  

int main() {
  int *arr = malloc(sizeof(int) * ARR_SIZE);
  if (!arr) {
    fprintf(2, "malloc failed\n");
    exit(1, "malloc failed\n");
  }
  int result;
  int pids[N];
  int total_sum = 0;
  int finished = 0;
  int statuses[NPROC];

  // Initialize array with consecutive numbers starting from 0
  for (int i = 0; i < ARR_SIZE; i++) {
    arr[i] = i;
  }
  
  // Call forkn system call
  result = forkn(N, pids);

  if (result < 0) {
    // fprintf(2, "forkn failed\n");
    free(arr);
    exit(1, "forkn failed\n");
  }

  if (result > 0) {
    // Child process
    int start = (result - 1) * (ARR_SIZE / N);
    int end = result * (ARR_SIZE / N);
    int part_sum = 0;
    
    for (int i = start; i < end; i++) {
      part_sum += arr[i];
    }
    exit(part_sum, "");
  }

  // Parent process
  if (waitall(&finished, statuses) < 0) {
    // fprintf(2, "waitall failed\n");
    free(arr);
    exit(1, "waitall failed\n");
  }

  // printf("Parent: %d children exited\n", finished);
  
  for (int i = 0; i < N; i++) {
      total_sum += statuses[i];
    }

  printf("Total sum: %d\n", total_sum);
  free(arr);
  exit(0, "parent done\n");
}


// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user.h"

// #define SIZE 65536  // 2^16
// #define NUM_CHILDREN 4

// // Function to calculate the sum of a portion of the array
// void sum_array(int start, int end, int *result) {
//     *result = 0;
//     for (int i = start; i < end; i++) {
//         *result += i;
//     }
// }

// // Function to convert an integer to a string (manual implementation)
// void itoa(int n, char *str) {
//   int i = 0;
//   int sign = n;
//   if (n < 0) {
//       n = -n;
//   }

//   // Generate the digits in reverse order
//   do {
//       str[i++] = n % 10 + '0'; // Get next digit
//   } while ((n /= 10) > 0);

//   // Append negative sign if the number was negative
//   if (sign < 0) {
//       str[i++] = '-';
//   }

//   str[i] = '\0'; // Null terminate the string

//   // Reverse the string
//   int start = 0;
//   int end = i - 1;
//   while (start < end) {
//       char temp = str[start];
//       str[start] = str[end];
//       str[end] = temp;
//       start++;
//       end--;
//   }
// }

// int main() {
//     int pids[NUM_CHILDREN];
//     int statuses[NUM_CHILDREN];
//     char lower_half_msgs[NUM_CHILDREN][10];  // To store the lower half of each sum as a string
//     int sum = 0;

//     // Use a variable to store the number of children
//     int num_children = NUM_CHILDREN;

//     // Fork 4 child processes to compute partial sums
//     int pid_count = forkn(NUM_CHILDREN, pids);  // Create NUM_CHILDREN child processes

//     if (pid_count > 0) {
//       // Each child calculates its portion of the array
//       for (int i = 0; i < NUM_CHILDREN; i++) {
//           if (pids[i] == 0) {
//               // Calculate partial sum for this child
//               int start = i * (SIZE / NUM_CHILDREN);
//               int end = (i + 1) * (SIZE / NUM_CHILDREN);
//               int partial_sum = 0;
//               sum_array(start, end, &partial_sum);

//               // Split the partial sum into two parts
//               int upper_half = partial_sum >> 16; // Top 16 bits
//               int lower_half = partial_sum & 0xFFFF; // Bottom 16 bits

//               // Convert the lower half into a string (message)
//               itoa(lower_half, lower_half_msgs[i]); // Store lower_half as a string

//               // Exit with the upper_half in the exit code and lower_half as the message
//               exit(upper_half, lower_half_msgs[i]); // Pass string message correctly
//           }
//       }
//   }

//   // Parent waits for all child processes to finish
//   waitall(&num_children, statuses);

//   // Calculate the total sum from the partial sums
//   for (int i = 0; i < NUM_CHILDREN; i++) {
//       // Extract the upper_half (from exit code) and lower_half (from message)
//       int upper_half = statuses[i] << 16; // Get the upper 16 bits (from exit code) and shift
//       int lower_half = atoi(lower_half_msgs[i]);  // Convert the message back to an integer

//       sum += (upper_half + lower_half);  // Reconstruct the full sum
//   }

//   // Print the final sum
//   printf(1, "Total sum: %d\n", sum);

//   exit(0, 0);  // Parent exits with status 0
// }