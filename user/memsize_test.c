#include "kernel/types.h"
#include "user/user.h"

int main() {
    printf("memsize before malloc: %d\n", memsize());

    void *a = malloc(20 * 1024); // Allocate 20 KB

    printf("memsize after malloc: %d\n", memsize());

    free(a);

    printf("memsize after free: %d\n", memsize());

    exit(0, "great success\n");
}