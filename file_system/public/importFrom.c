#include "../src/import_export.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <virtual_disk_filepath> <filename> <filepath>\n", argv[0]);
        return 1;
    }

    char *vir_disk_filepath = argv[1];
	char *filename = argv[2];
	char *filepath = argv[3];

    if (importFromVirDisk(vir_disk_filepath, filename, filepath) != 0) {
        fprintf(stderr, "File import failed: \"%s\"\n", filename);
        return 1;
    }

    return 0;
}