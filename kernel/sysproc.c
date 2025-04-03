#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"


#define MAX_CHILDREN 16


uint64
sys_exit(void)
{
  int n;
  char exit_msg[32]; 
  
  argint(0, &n);

  // Retrieve string argument safely from user space
  if (argstr(1, exit_msg, sizeof(exit_msg)) < 0)
    return -1;

  exit(n, exit_msg);
  return 0; 
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  uint64 msg_addr;
  argaddr(0, &p); // First arg: int*
  argaddr(1, &msg_addr); // Second arg: char*
  return wait(p, msg_addr);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_memsize(void)
{
  // uint64 addr;

  // addr = myproc()->sz;
  return myproc()->sz;
}

uint64 
sys_forkn(void) 
{
    int n;
    uint64 pids;  // Userspace pointer for child PIDs
    argint(0, &n);
    argaddr(1, &pids);
    // if (n != 0 || pids != 0) {
    //     return -1; // Invalid arguments
    // }

    if (n < 1 || n > MAX_CHILDREN) {
        return -1; // Restrict range of child processes
    }

    int created = 0;
    int child_pids[MAX_CHILDREN];

    // Fork n child processes
    for (int i = 0; i < n; i++) {
        int pid = fork();
        if (pid < 0) {
            // Cleanup: Kill already created processes
            for (int j = 0; j < created; j++) {
                kill(child_pids[j]);
            }
            return -1;  // Indicate failure
        } else if (pid == 0) {
            return i + 1;  // Child returns its index (1-based)
        }
        child_pids[created++] = pid;
    }

    // Copy child PIDs to userspace
    if (copyout(myproc()->pagetable, pids, (char *)child_pids, sizeof(int) * n) < 0) {
        return -1;
    }

    return 0; // Success, parent returns 0
}

uint64 
sys_waitall(void) 
{
  int finished = 0;
  int statuses_arr[NPROC];
  struct proc *p;
  struct proc *curproc = myproc();
  int n;
  uint64 statuses;

  // Get user pointers
  argint(0, &n);
  argaddr(1, &statuses);
  // if (argint(0, &n) != 0 || argaddr(1, &pids) != 0)
  //     return -1;

  acquire(&wait_lock);

  int has_children = 0;

  for (;;) {  // Loop until all children are ZOMBIE
      finished = 0;

      for (p = proc; p < &proc[NPROC]; p++) {
          acquire(&p->lock);

          if (p->parent == curproc) {
              has_children = 1;

              if (p->state == ZOMBIE) {
                  statuses_arr[finished++] = p->xstate;  // Store exit status
                  freeproc(p);  // Clean up process
              }
          }

          release(&p->lock);
      }

      if (!has_children) {
          // No child processes exist → return 0, set n = 0, do not modify statuses
          release(&wait_lock);
          if (copyout(curproc->pagetable, n, (char *)&finished, sizeof(int)) < 0)
              return -1;
          return 0;
      }

      if (finished > 0) {
          // Some children finished, return results
          release(&wait_lock);

          if (copyout(curproc->pagetable, n, (char *)&finished, sizeof(int)) < 0 ||
              copyout(curproc->pagetable, statuses, (char *)statuses_arr, sizeof(int) * finished) < 0) {
              return -1;  // Error: Do not modify n or statuses
          }

          return 0;
      }

      // If no finished children, wait
      sleep(curproc, &wait_lock);
  }
}

