/* This file contains the necessary constants used in the program. */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Main constants */
#define MEMORY_START 100
#define MAX_MEMORY 256
#define MIN_ADDRESS 0
#define MAX_ADDRESS 255
#define MAX_ERROR 100
#define MAX_MESSAGE 200
#define MAX_SYMBOL_NAME 35
#define MAX_LINE 80
#define MAX_EXTERNALS 100

/* Bit masks and shifts */
#define ARE_SHIFT 2
#define ARE_ABSOLUTE 0x0
#define ARE_EXTERNAL 0x1
#define ARE_RELOCATABLE 0x2

/* Word encoding masks */
#define OPCODE_MASK 0xF
#define ADDRESSING_MODE_MASK 0x3
#define REGISTER_MASK 0x7
#define VALUE_MASK 0x3FF

/* Bit positions */
#define OPCODE_SHIFT 6
#define SRC_MODE_SHIFT 4
#define DEST_MODE_SHIFT 2
#define SRC_REG_SHIFT 6
#define DEST_REG_SHIFT 2

/* Register limits */
#define MIN_REGISTER 0
#define MAX_REGISTER 7

/* Addressing modes */
#define IMMEDIATE_MODE 0
#define DIRECT_MODE 1
#define MATRIX_MODE 2
#define REGISTER_MODE 3

#endif
