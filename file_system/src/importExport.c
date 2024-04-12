#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import_export.h"
#include "const.h"
#include "section_getters.h"
#include "delete.h"
#include "structs.h"


static int chooseEmptyFileRecord(fileRecord *file_record_array) {
	for (int i = 0; i < MAX_FILE_AMOUNT; i++) {
		if (file_record_array[i].i_node_index == -1) {
			return i;
		}
	}
	return -1;
}

static int chooseEmptyINode(iNode *i_node_array) {
	for (int i = 0; i < MAX_FILE_AMOUNT; i++) {
		if (i_node_array[i].file_size == 0) {
			return i;
		}
	}
	return -1;
}

static int chooseEmptyDataBlock(char *bitmap) {
	for (int i = 0; i < DATA_BLOCK_AMOUNT; i++) {
		if (bitmap[i] == 0) {
			return i;
		}
	}
	return -1;
}


int exportIntoVirDisk(char *virtual_disk_filepath, char *filepath, char *filename) {
	FILE *virtual_disk, *source_file;
	virtual_disk = fopen(virtual_disk_filepath, "r+b");
	if (virtual_disk == NULL) {
		perror("Error with opening virtual disk");
		return -1;
	}

	source_file = fopen(filepath, "r+");
	if (source_file == NULL) {
		perror("Error with opening source file");
		fclose(virtual_disk);
		return -1;
	}

	fseek(source_file, 0, SEEK_END);
	long source_file_size = ftell(source_file);

	if (source_file_size > MAX_FILE_SIZE) {
		perror("Source file size is too large");
		fclose(virtual_disk);
		fclose(source_file);
		return -1;
	}

	virDiskHeader header;
	getVirDiskHeader(virtual_disk, &header);

	unsigned char bitmap[DATA_BLOCK_AMOUNT];
	fseek(virtual_disk, header.bitmap_offset, SEEK_SET);
	fread(bitmap, sizeof(bitmap), 1, virtual_disk);

	fileRecord file_record_array[MAX_FILE_AMOUNT];
	fseek(virtual_disk, header.file_record_array_offset, SEEK_SET);
	fread(file_record_array, sizeof(file_record_array), 1, virtual_disk);

	int file_record_index = isFilenameBusy(filename, file_record_array);
	int i_node_index;
	iNode i_node;
	int source_file_block_amount = CEIL_DIV(source_file_size, DATA_BLOCK_SIZE);

	if (file_record_index >= 0) {
		i_node_index = file_record_array[file_record_index].i_node_index;
		fseek(virtual_disk, header.i_node_array_offset + i_node_index * sizeof(iNode), SEEK_SET);
		fread(&i_node, sizeof(iNode), 1, virtual_disk);
		int available_block_amount = CEIL_DIV(i_node.file_size, DATA_BLOCK_SIZE) + header.empty_data_block_amount;
		if (source_file_block_amount > available_block_amount) {
			perror("Source file size is too large");
			fclose(virtual_disk);
			fclose(source_file);
			return -1;
		}
		char *zero_data_block = calloc(DATA_BLOCK_SIZE, sizeof(char));
		for (int i = 0; i < 3; i++) {
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
	} else {
		if (header.current_file_amount == MAX_FILE_AMOUNT) {
			perror("Max file amount reached on virtual disk");
			fclose(virtual_disk);
			fclose(source_file);
			return -1;
		}
		if (source_file_block_amount > header.empty_data_block_amount) {
			perror("Source file size is too large");
			fclose(virtual_disk);
			fclose(source_file);
			return -1;
		}
		header.current_file_amount += 1;
		iNode i_node_array[MAX_FILE_AMOUNT];
		fseek(virtual_disk, header.i_node_array_offset, SEEK_SET);
		fread(i_node_array, sizeof(i_node_array), 1, virtual_disk);
		file_record_index = chooseEmptyFileRecord(file_record_array);
		i_node_index = chooseEmptyINode(i_node_array);
		strcpy(file_record_array[file_record_index].filename, filename);
		file_record_array[file_record_index].i_node_index = i_node_index;
		i_node = i_node_array[i_node_index];
	}
	i_node.file_size = source_file_size;
	char *temp_data_block = calloc(DATA_BLOCK_SIZE, sizeof(char));
	fseek(source_file, 0, SEEK_SET);
	int i = 0;
	while (fread(temp_data_block, sizeof(char), DATA_BLOCK_SIZE, source_file) > 0) {
		int data_block_index = chooseEmptyDataBlock(bitmap);
		bitmap[data_block_index] = 1;
		header.empty_data_block_amount -= 1;
		i_node.data_blocks[i++] = data_block_index;
		fseek(virtual_disk, header.data_block_array_offset + data_block_index * DATA_BLOCK_SIZE, SEEK_SET);
		fwrite(temp_data_block, sizeof(char), DATA_BLOCK_SIZE, virtual_disk);		
	}
	free(temp_data_block);
	fseek(virtual_disk, header.i_node_array_offset + i_node_index * sizeof(iNode), SEEK_SET);
	fwrite(&i_node, sizeof(iNode), 1, virtual_disk);

	fseek(virtual_disk, header.file_record_array_offset + file_record_index * sizeof(fileRecord), SEEK_SET);
	fwrite(&file_record_array[file_record_index], sizeof(fileRecord), 1, virtual_disk);

	fseek(virtual_disk, header.bitmap_offset, SEEK_SET);
	fwrite(bitmap, sizeof(bitmap), 1, virtual_disk);

	fseek(virtual_disk, 0, SEEK_SET);
	fwrite(&header, sizeof(virDiskHeader), 1, virtual_disk);

	fclose(source_file);
	fclose(virtual_disk);

	return 	0;
}

int importFromVirDisk(char *virtual_disk_filepath, char *filename, char *filepath) {
	FILE *virtual_disk, *destination_file;
	virtual_disk = fopen(virtual_disk_filepath, "r+b");
	if (virtual_disk == NULL) {
		perror("Error with opening virtual disk");
		return -1;
	}

	destination_file = fopen(filepath, "w+");
	if (destination_file == NULL) {
		perror("Error with opening destination file");
		fclose(virtual_disk);
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
		fclose(destination_file);
		return -1;
	}

	int i_node_index = file_record_array[file_record_index].i_node_index;
	iNode i_node;
	fseek(virtual_disk, header.i_node_array_offset + i_node_index * sizeof(iNode), SEEK_SET);
	fread(&i_node, sizeof(iNode), 1, virtual_disk);

	char *temp_data_block = calloc(DATA_BLOCK_SIZE, sizeof(char));

	fseek(destination_file, 0, SEEK_SET);

	for (int i = 0; i < CEIL_DIV(MAX_FILE_SIZE, DATA_BLOCK_SIZE); i++) {
		int data_block_index = i_node.data_blocks[i];
		if (data_block_index >= 0) {
			fseek(virtual_disk, header.data_block_array_offset + data_block_index * DATA_BLOCK_SIZE, SEEK_SET);
			fread(temp_data_block, sizeof(char), DATA_BLOCK_SIZE, virtual_disk);
			fwrite(temp_data_block, sizeof(char), DATA_BLOCK_SIZE, destination_file);
		} else {
			break;
		}
	}
	free(temp_data_block);

	fclose(virtual_disk);
	fclose(destination_file);
	return 0;
}