#include <stdio.h>
#include <stdlib.h>
#include "section_getters.h"
#include "delete.h"
#include "structs.h"
#include "const.h"

int listVirDiskFiles(char * virtual_disk_filepath) {
	FILE *virtual_disk;
	virtual_disk = fopen(virtual_disk_filepath, "r+");
	if (virtual_disk == NULL) {
		perror("Error with opening virtual disk");
		return -1;
	}
	
	virDiskHeader header;
	getVirDiskHeader(virtual_disk, &header);


	fileRecord file_record_array[MAX_FILE_AMOUNT];
	fseek(virtual_disk, header.file_record_array_offset, SEEK_SET);
	fread(file_record_array, sizeof(file_record_array), 1, virtual_disk);

	for (int i = 0; i < MAX_FILE_AMOUNT; i++) {
		if (file_record_array[i].i_node_index >= 0) {
			printf("%s\t\t", file_record_array[i].filename);
		}
	}
	printf("\n");

	fclose(virtual_disk);

	return 0;
}

int readVirDiskFile(char *virtual_disk_filepath, char* filename) {
	FILE *virtual_disk;
	virtual_disk = fopen(virtual_disk_filepath, "r+");
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

	iNode i_node;
	fseek(virtual_disk, header.i_node_array_offset + i_node_index * sizeof(iNode), SEEK_SET);
	fread(&i_node, sizeof(iNode), 1, virtual_disk);

	char *temp_data_block = calloc(DATA_BLOCK_SIZE, sizeof(char));

	int i = 0, data_block_index;
	while (((data_block_index = i_node.data_blocks[i]) >= 0) && i < CEIL_DIV(MAX_FILE_SIZE, DATA_BLOCK_SIZE)) {
		fseek(virtual_disk, header.data_block_array_offset + data_block_index * DATA_BLOCK_SIZE, SEEK_SET);
		fread(temp_data_block, sizeof(char), DATA_BLOCK_SIZE, virtual_disk);
		printf("%s", temp_data_block);
		i++;
	}
	free(temp_data_block);

	fclose(virtual_disk);
	return 0;
}

int displayVirDiskUsage(char *virtual_disk_filepath) {
	FILE* virtual_disk;
	virtual_disk = fopen(virtual_disk_filepath, "r+");
	if (virtual_disk == NULL) {
		perror("Error with opening virtual disk");
		return -1;
	}
	
	virDiskHeader header;
	getVirDiskHeader(virtual_disk, &header);

	printf("---HEADER---\n");
	printf("Offset: %d\n", 0);
	printf("Current file amount: %d\n", header.current_file_amount);
	printf("Empty data block amount: %d\n", header.empty_data_block_amount);

	unsigned char bitmap[DATA_BLOCK_AMOUNT];
	fseek(virtual_disk, header.bitmap_offset, SEEK_SET);
	fread(bitmap, sizeof(bitmap), 1, virtual_disk);
	printf("\n---BITMAP---\n");
	printf("Offset: %d %#010x\n", header.bitmap_offset, header.bitmap_offset);

	fileRecord *file_record_array = calloc(MAX_FILE_AMOUNT, sizeof(fileRecord));
	fseek(virtual_disk, header.file_record_array_offset, SEEK_SET);
	fread(file_record_array, sizeof(fileRecord), MAX_FILE_AMOUNT, virtual_disk);
	printf("\n---FILE RECORDS---\n");
	printf("Offset: %d %#010x \n", header.file_record_array_offset, header.file_record_array_offset);
	for (int i = 0; i < MAX_FILE_AMOUNT; i++) {
		if (file_record_array[i].i_node_index >= 0)
			printf("\n[%d] filename: \"%s\" iNode: %d\n", i, file_record_array[i].filename, file_record_array[i].i_node_index);
	}
	free(file_record_array);

	iNode *i_node_array = calloc(MAX_FILE_AMOUNT, sizeof(iNode));
	fseek(virtual_disk, header.i_node_array_offset, SEEK_SET);
	fread(i_node_array, sizeof(iNode), MAX_FILE_AMOUNT, virtual_disk);
	printf("\n---iNodes---\n");
	printf("Offset: %d %#010x\n", header.i_node_array_offset, header.i_node_array_offset);
	for (int i = 0; i < MAX_FILE_AMOUNT; i++) {
		if (i_node_array[i].file_size > 0)
			printf("\n[%d] size: %d data block amount: %d\n", i, i_node_array[i].file_size, CEIL_DIV(i_node_array[i].file_size, DATA_BLOCK_SIZE));
	}
	free(i_node_array);

	printf("\n---DATA BLOCKS---\n");
	printf("Offset: %d %#010x\n", header.data_block_array_offset, header.data_block_array_offset);
	for (int i = 0; i < DATA_BLOCK_AMOUNT; i++) {
		if (bitmap[i] == 1) {
			printf("\n[%d] status: busy\n", i);
		} else {
			printf("\n[%d] status: free\n", i);
		}
	}
	printf("---END---\n");
	fclose(virtual_disk);
	return 0;
}