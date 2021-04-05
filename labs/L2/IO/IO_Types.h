#ifndef __IO_TYPES_H__
#define __IO_TYPES_H__

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define IO_FEOF_STREAM_OPEN (0U)

typedef uint8_t IO_msg;
typedef IO_msg IO_index;

typedef const char *const IO_cPtrCStr;
typedef FILE *const IO_FILEcp;

typedef enum tag_IO_states_e IO_states_e;
typedef const IO_states_e IO_states_ce;
/*typedef enum tag_IO_states_e
{
    IO_WRITING,
    IO_BUFF_IS_FULL,
}IO_states_e;*/

typedef struct tag_IO_buffor_s IO_buffor_s;
typedef const IO_buffor_s IO_buffor_cs;
/*typedef struct tag_IO_buffor_s
{
    IO_msg message;
    IO_index lastBitIndexInMsg;
    IO_states_e state_e;
    const IO_FILEcp inputStreamPtr;
    const IO_FILEcp outputStreamPtr;
}IO_buffor_s;*/


#endif /*__IO_TYPES_H__*/