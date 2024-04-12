#include "../src/delete.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <virtual_disk_filepath> <filename>\n", argv[0]);
        return 1;
    }

    char *vir_disk_filepath = argv[1];
	char *filename = argv[2];

    if (deleteVirDiskFile(vir_disk_filepath, filename) != 0) {
        fprintf(stderr, "File deletion failed: \"%s\"\n", filename);
        return 1;
    }

    return 0;
}