#ifndef DELETE_H
#define DELETE_H

#include "structs.h"
#include <stdio.h>

int isFilenameBusy(char *filename, fileRecord *file_record_array);
int deleteVirDiskFile(char *virtual_disk_filepath, char *filename);

#endif
