#include "../src/read.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <virtual_disk_filepath>\n", argv[0]);
        return 1;
    }

    char *vir_disk_filepath = argv[1];

    if (listVirDiskFiles(vir_disk_filepath) != 0) {
        fprintf(stderr, "File listing failed: \"%s\"\n", vir_disk_filepath);
        return 1;
    }

    return 0;
}