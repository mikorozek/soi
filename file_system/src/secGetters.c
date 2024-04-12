#include "section_getters.h"


void getVirDiskHeader(FILE *virtual_disk, virDiskHeader *header) {
	fseek(virtual_disk, 0, SEEK_SET);
	fread(header, sizeof(virDiskHeader), 1, virtual_disk);
}