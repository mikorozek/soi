#ifndef READ_H
#define READ_H

int listVirDiskFiles(char *virtual_disk_filepath);
int readVirDiskFile(char *virtual_disk_filepath, char *filename);
int displayVirDiskUsage(char *virtual_disk_filepath);

#endif