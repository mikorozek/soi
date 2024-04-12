#ifndef IMPORT_EXPORT_H
#define IMPORT_EXPORT_H

int exportIntoVirDisk(char *virtual_disk_filepath, char *filepath,
                      char *filename);
int importFromVirDisk(char *virtual_disk_filepath, char *filename,
                      char *filepath);

#endif