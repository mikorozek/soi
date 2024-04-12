#include "../src/import_export.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <virtual_disk_filepath> <filepath> <filename>\n", argv[0]);
        return 1;
    }

    char *vir_disk_filepath = argv[1];
	char *filepath = argv[2];
	char *filename = argv[3];

    if (exportIntoVirDisk(vir_disk_filepath, filepath, filename) != 0) {
        fprintf(stderr, "File export failed: \"%s\"\n", filepath);
        return 1;
    }

    return 0;
}