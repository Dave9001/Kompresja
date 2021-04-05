#ifndef __IO_HIDDEN_TYPES_H__
#define __IO_HIDDEN_TYPES_H__

#include <stdlib.h>
#include <errno.h>
#include "IO_Types.h"

#define FALSE (0U)
#define TRUE (!FALSE)
#define EMPTY_MSG (0U)
#define NUM_OF_BITS_IN_BYTE (8U)
#define MSB_INDEX(var) ((sizeof(var) * NUM_OF_BITS_IN_BYTE) - 1U)
#define LSB_INDEX (0U)
#define IO_CLEAR_BUFFOR(buffor_ps) do{\
            buffor_ps->message = EMPTY_MSG;\
            buffor_ps->lastBitIndexInMsg = MSB_INDEX(buffor_ps->lastBitIndexInMsg);\
            buffor_ps->state_e = IO_WRITING;\
        }while(FALSE)

#define IO_WRITE_BIT(bitVal, buffor_ps) do{\
            if(IO_WRITING == buffor_ps->state_e)\
            {\
                buffor_ps->message |= (bitVal << buffor_ps->lastBitIndexInMsg);\
                if(0U == buffor_ps->lastBitIndexInMsg)\
                {\
                    buffor_ps->state_e = IO_BUFF_IS_FULL;\
                }\
                else\
                {\
                    buffor_ps->lastBitIndexInMsg--;\
                }\
            }\
        }while(FALSE)

#define IO_STDIN (1U)
#define IO_STDOUT (0U)

#define ERROR_PRINT(msg)  do{\
            printf("Error: ");perror("");puts("");\
            printf("%s in FUNCTION: %s\n", msg, __func__);\
            printf("FILE: %s in LINE: %d\n", __FILE__, __LINE__);\
            exit(EXIT_FAILURE);\
        }while(FALSE)
#define IO_BUFFOR_STATE(buffor_ps) buffor_pcs->state_e


typedef IO_msg IO_bitMask;



#endif /*__IO_HIDDEN_TYPES_H__*/