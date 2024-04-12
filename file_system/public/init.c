#include "../src/init.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *vir_disk_filename = argv[1];

    if (createVirDisk(vir_disk_filename) != 0) {
        fprintf(stderr, "Initialization failed: \"%s\"\n", vir_disk_filename);
        return 1;
    }

    printf("Virtual disk \"%s\" creation successful.\n", vir_disk_filename);
    return 0;
}