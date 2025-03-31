#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
    // exit(0, "Goodbye World xv6\n");

    
    // int pid = fork();
    // if (pid == 0) {
    //     exit(0, "child finished");
    // } else {
    //     int status;
    //     char msg[32];
    //     int child_pid = wait(&status, msg);
    //     printf("child %d exited with status %d, msg: %s\n", child_pid, status, msg);
    // }
    // exit(0, "parent done");

    
    // char *msgs[] = {
    //     "first child done",
    //     "second child here",
    //     "third exiting"
    //   };
    
    //   int num_children = 3;
    //   for (int i = 0; i < num_children; i++) {
    //     int pid = fork();
    //     if (pid == 0) {
    //       // Child
    //       exit(0, msgs[i]);
    //     }
    //   }
    
    //   // Parent
    //   char msg[32];
    //   int status;
    //   for (int i = 0; i < num_children; i++) {
    //     int pid = wait(&status, msg);
    //     if (pid >= 0)
    //       printf("Child %d exited with status %d, msg: %s\n", pid, status, msg);
    //     else
    //       printf("wait failed\n");
    //   }
    
    //   exit(0, "parent done");


    int pid = fork();
    if (pid == 0) {
        exit(0, "child done\n");
    } else {
        int status;
        char msg[32];
        wait(&status, msg);
        printf("child exited with msg: %s", msg);
    }
    exit(0, "parent done\n");
}