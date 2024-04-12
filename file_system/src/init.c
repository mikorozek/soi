#include "init.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static void initVirDiskHeader(virDiskHeader* header) {
	header->current_file_amount = 0;
	header->empty_data_block_amount = DATA_BLOCK_AMOUNT;
	header->bitmap_offset = sizeof(virDiskHeader);
	header->file_record_array_offset = header->bitmap_offset + DATA_BLOCK_AMOUNT * sizeof(unsigned char);
	header->i_node_array_offset = header->file_record_array_offset + MAX_FILE_AMOUNT * sizeof(fileRecord);
	header->data_block_array_offset = header->i_node_array_offset + MAX_FILE_AMOUNT * sizeof(iNode);
}


static void initFileRecordArray(fileRecord *file_record_array) {
	for (int i = 0; i < MAX_FILE_AMOUNT; i++) {
		file_record_array[i].i_node_index = -1;
	}
}


static void initINodeArray(iNode *i_node_array) {
	for (int i = 0; i < MAX_FILE_AMOUNT; i++) {
		memset(i_node_array[i].data_blocks, -1, sizeof(int) * CEIL_DIV(MAX_FILE_SIZE, DATA_BLOCK_SIZE));
	}
}


int createVirDisk(char *vir_disk_filename) {
	FILE *virtual_disk;
	virtual_disk = fopen(vir_disk_filename, "w+");
	if (virtual_disk == NULL) {
		perror("Error with file opening");
		return -1;
	}

	virDiskHeader temp_header;
	initVirDiskHeader(&temp_header);
	if (fwrite(&temp_header, sizeof(virDiskHeader), 1, virtual_disk) != 1) {
		perror("Error with writing header to virtual disk");
		fclose(virtual_disk);
		return -1;
	}

	unsigned char bitmap[DATA_BLOCK_AMOUNT];
	memset(bitmap, 0, DATA_BLOCK_AMOUNT * sizeof(unsigned char));
	if (fwrite(bitmap, sizeof(bitmap), 1, virtual_disk) != 1) {
		perror("Error with writing bitmap to virtual disk");
		fclose(virtual_disk);
		return -1;
	}

	fileRecord *file_record_array = calloc(MAX_FILE_AMOUNT, sizeof(fileRecord));
	initFileRecordArray(file_record_array);
	if (fwrite(file_record_array, sizeof(fileRecord), MAX_FILE_AMOUNT, virtual_disk) != MAX_FILE_AMOUNT) {
		perror("Error with writing file record array to virtual disk");
		fclose(virtual_disk);
		return -1;
	}
	free(file_record_array);

	iNode *i_node_array = calloc(MAX_FILE_AMOUNT, sizeof(iNode));
	initINodeArray(i_node_array);
	if (fwrite(i_node_array, sizeof(iNode), MAX_FILE_AMOUNT, virtual_disk) != MAX_FILE_AMOUNT) {
		perror("Error with writing iNode array to virtual disk");
		fclose(virtual_disk);
		return -1;
	}
	free(i_node_array);

	char (*data_block_array)[DATA_BLOCK_SIZE] = calloc(DATA_BLOCK_AMOUNT, DATA_BLOCK_SIZE);
	if (!data_block_array || fwrite(data_block_array, DATA_BLOCK_SIZE, DATA_BLOCK_AMOUNT, virtual_disk) != DATA_BLOCK_AMOUNT) {
		perror("Error with writing data block array to virtual disk");
		fclose(virtual_disk);
		return -1;
	}
	free(data_block_array);

	fclose(virtual_disk);
	return 0;
}