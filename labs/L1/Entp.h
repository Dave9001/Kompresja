/**
 *  @Author: Self
 *  
 *  @Brief: File contains declarations of functions, that are used
 *          in Entp.c file
 *          
 * 
 *  @Usage: 
 * 
 *  @Limitations: 
 *  
 *  @Violation: 
 * 

 * 
 * @Date: 10.10.2018
 **/

#ifndef _ENTP_H_
#define _ENTP_H_

/*******************************************************************************
 *                              INCLUDS
 ******************************************************************************/

#include <stdio.h>
#include "Entp_Types.h"

#ifndef _ALPHABET_
#define _ALPHABET_
    #define ALPHABET_SIZE ((Entp_index_t)256U) /* provides size of alphabet */
#endif

#ifndef _ARRSIZELIMINBYTE_
#define _ARRSIZELIMINBYTE_
    #define ARRAY_BYTE_LENGTH_LIMIT (2U << 15U) /* provides upper boundary for size of 1D array in bytes */
#endif

#ifndef _ARRSIZELIM_
#define _ARRSIZELIM_
    #define ARRAY_LENGTH_LIMIT (ARRAY_BYTE_LENGTH_LIMIT / sizeof(Entp_symbol_t))  /* provides upper boundary for size of 1D array */
#endif

#ifndef _ARRPROBSIZELIM_
#define _ARRPROBSIZELIM_
    #define ARRAY_PROBAB_LENGTH_LIMIT (ARRAY_BYTE_LENGTH_LIMIT / sizeof(Entp_metrics_t))  /* provides upper boundary for size of 1D array */
#endif

#define FIRST_SYMBOL (0U)

#define STREAM_SOURCE "stdin"
#define MODE "none"
#define STDIN_CPR (0U)

#define INC_1D_ARRAY_CELL_VAL(array, cell) array[cell] = array[cell] + 1
#define INC_2D_ARRAY_CELL_VAL(array, cell_1, cell_2)\
        array[cell_1][cell_2] = array[cell_1][cell_2] + 1

#define INIT_METRICS(metricsName, func)  Entp_metricsCollection_s metricsName\
        = {.symbolsProbability_a = func(ALPHABET_SIZE),\
            .entropy = (0x00U)\
        }





#define ERROR_PRINT(msg)  printf("Error:\n");\
                          printf("%s in FUNCTION:%s\n", msg, __func__);\
                          printf("FILE:%s in LINE:%d\n", __FILE__, __LINE__);\
                          exit(EXIT_FAILURE)




extern void Entp_Manager(void);




#endif /* _ENTP_H */