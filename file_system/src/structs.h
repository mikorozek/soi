#ifndef STRUCTS_H
#define STRUCTS_H

#include "const.h"
#include <math.h>

#define CEIL_DIV(a, b) (((a) + (b)-1) / (b))

typedef struct {
  char filename[MAX_FILENAME_LENGTH];
  int i_node_index;
} fileRecord;

typedef struct {
  unsigned int file_size;
  int data_blocks[CEIL_DIV(MAX_FILE_SIZE, DATA_BLOCK_SIZE)];
} iNode;

typedef struct {
	unsigned int current_file_amount;
	unsigned int empty_data_block_amount;
	unsigned int bitmap_offset;
	unsigned int file_record_array_offset;
	unsigned int i_node_array_offset;
	unsigned int data_block_array_offset;
} virDiskHeader;

#endif