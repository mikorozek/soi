#include <string.h>
#include <stdlib.h>
#include "delete.h"
#include "section_getters.h"


int isFilenameBusy(char *filename, fileRecord* file_record_array) {
	for (int i = 0; i < MAX_FILE_AMOUNT; i++) {
		if (strcmp(file_record_array[i].filename, filename) == 0) {
			return i;
		}
	}
	return -1;
}


int deleteVirDiskFile(char *virtual_disk_filepath, char *filename) {
	FILE *virtual_disk;
	virtual_disk = fopen(virtual_disk_filepath, "r+b");
	if (virtual_disk == NULL) {
		perror("Error with opening virtual disk");
		return -1;
	}

	virDiskHeader header;
	getVirDiskHeader(virtual_disk, &header);
	
	fileRecord file_record_array[MAX_FILE_AMOUNT];
	fseek(virtual_disk, header.file_record_array_offset, SEEK_SET);
	fread(file_record_array, sizeof(file_record_array), 1, virtual_disk);

	int file_record_index = isFilenameBusy(filename, file_record_array);
	if (file_record_index < 0) {
		perror("File with such name does not exist");
		fclose(virtual_disk);
		return -1;
	}

	int i_node_index = file_record_array[file_record_index].i_node_index;

	unsigned char bitmap[DATA_BLOCK_AMOUNT];
	fseek(virtual_disk, header.bitmap_offset, SEEK_SET);
	fread(bitmap, sizeof(bitmap), 1, virtual_disk);

	iNode i_node;
	fseek(virtual_disk, header.i_node_array_offset + i_node_index * sizeof(iNode), SEEK_SET);
	fread(&i_node, sizeof(iNode), 1, virtual_disk);

	char *zero_data_block = calloc(DATA_BLOCK_SIZE, sizeof(char));
	
	for (int i = 0; i < CEIL_DIV(MAX_FILE_SIZE, DATA_BLOCK_SIZE); i++) {
		int data_block_index = i_node.data_blocks[i];
		if (data_block_index >= 0) {
			fseek(virtual_disk, header.data_block_array_offset + data_block_index * DATA_BLOCK_SIZE, SEEK_SET);
			fwrite(zero_data_block, sizeof(char), DATA_BLOCK_SIZE, virtual_disk);
			bitmap[data_block_index] = 0;
			header.empty_data_block_amount += 1;
		}
	}

	free(zero_data_block);

	memset(&i_node, 0, sizeof(iNode));
	memset(i_node.data_blocks, -1, sizeof(int) * 3);
	fseek(virtual_disk, header.i_node_array_offset + i_node_index * sizeof(iNode), SEEK_SET);
	fwrite(&i_node, sizeof(iNode), 1, virtual_disk);

	memset(&file_record_array[file_record_index], 0, sizeof(fileRecord));
	file_record_array[file_record_index].i_node_index = -1;
	fseek(virtual_disk, header.file_record_array_offset + file_record_index * sizeof(fileRecord), SEEK_SET);
	fwrite(&file_record_array[file_record_index], sizeof(fileRecord), 1, virtual_disk);

	fseek(virtual_disk, header.bitmap_offset, SEEK_SET);
	fwrite(bitmap, sizeof(bitmap), 1, virtual_disk);

	header.current_file_amount -= 1;

	fseek(virtual_disk, 0, SEEK_SET);
	fwrite(&header, sizeof(virDiskHeader), 1, virtual_disk);

	return 0;
}