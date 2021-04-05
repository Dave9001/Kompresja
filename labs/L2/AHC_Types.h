#ifndef __AHC_TYPES_H__
#define __AHC_TYPES_H___

#include "IO/IO_Types.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>


#define AHC_NumOfBitsInByte (8U)
#define AHC_NUM_OF_SYMBOLS  ((1U << sizeof(AHC_symbol) * AHC_NumOfBitsInByte))
#define AHC_NumOfBitsInMaxCode (8U)//(AHC_NUM_OF_SYMBOLS - 0U)//1U)
#define AHC_ByteLengthOfMaxCode \
            (((AHC_NumOfBitsInMaxCode + 1U) / sizeof(AHC_code)) / AHC_NumOfBitsInByte) /* +1U bc. (AHC_NumOfBitsInMaxCode + 1U ) %8 =0 */
#define AHC_symbolsArraysLastIndex (AHC_NUM_OF_SYMBOLS - 1U)
#define AHC_codeArrayLastIndex (AHC_NumOfBitsInMaxCode - 1U)
#define AHC_bufforLen (sizeof(AHC_symbol) * AHC_NumOfBitsInByte)
#define AHC_MAX_NUM_OF_NODES ((AHC_NUM_OF_SYMBOLS * 2U) - 1U)
#define AHC_NextToLast (AHC_NUM_OF_SYMBOLS - 1U)

#define AHC_MSB(var) ((sizeof(var)*AHC_NumOfBitsInByte)-1U)
#define AHC_LSB(var) (1U)

#define AHC_ROOT_ID ((AHC_NUM_OF_SYMBOLS * 2U) - 1U)

#define MASK_BIT_0 (0x01U)
#define MASK_BIT_1 (0x02U)
#define MASK_BIT_2 (0x04U)
#define MASK_BIT_3 (0x08U)
#define MASK_BIT_4 (0x10U)
#define MASK_BIT_5 (0x20U)
#define MASK_BIT_6 (0x40U)
#define MASK_BIT_7 (0x80U)

#define AHC_NYT_INIT(nyt) do{\
    nyt->id = AHC_ROOT_ID;\
    nyt->parent = NULL;\
    nyt->leftChild = NULL;\
    nyt->rightChild = NULL;\
    nyt->symbol = 0;\
    nyt->type = AHC_NODE_NYT;\
    nyt->value = 0U;\
}while(FALSE)



#define ERROR_PRINT(msg)  do{\
            printf("Error: ");perror("");puts("");\
            printf("%s in FUNCTION: %s\n", msg, __func__);\
            printf("FILE: %s in LINE: %d\n", __FILE__, __LINE__);\
            exit(EXIT_FAILURE);\
        }while(FALSE)

#define AHC_SYMBOL_INDEX(symbol, symbols_ps) (symbols_ps->symbolIndex_a[symbol])


#ifndef __FALSE__
#define __FALSE__
    #define FALSE (0U)  /* by exception that 0 is eq. to false */
#endif

#ifndef __TRUE__
#define __TRUE__
    #define TRUE (!FALSE)  /* logical negation of false is true which is DECRISING wanted value*/
#endif

typedef IO_msg AHC_symbol;             /* symbol from alphabet */
typedef const AHC_symbol AHC_symbol_c;             /* symbol from alphabet */
typedef uint16_t AHC_symbolsCount;
typedef uint32_t AHC_numOfOccur;

//typedef uint64_t AHC_code;
typedef uint8_t AHC_code;
typedef uint16_t AHC_counter;
typedef uint16_t AHC_index;

typedef AHC_code AHC_buffor;

typedef enum tag_AHC_nodes_types_e AHC_NODES_TYPES_E;
/*typedef enum
{
    AHC_NODE_INNER,
    AHC_NODE_SYMBOL,
    AHC_NODE_NYT
}AHC_NODES_TYPES_E;
*/

typedef struct tag_AHC_node_s AHC_node_s;
typedef const AHC_node_s AHC_node_cs;
/*typedef struct tag_AHC_node_s
{
    struct tag_AHC_node_s* parent;
    struct tag_AHC_node_s* rightChild;
    struct tag_AHC_node_s* leftChild;

    AHC_numOfOccur value;
    AHC_symbol symbol;
    AHC_index id;
    AHC_NODES_TYPES_E type;
}AHC_node_s;*/


typedef struct tag_AHC_tree_s AHC_tree_s;
/*typedef struct tag_AHC_tree_s
{
    AHC_node_s* root;
    AHC_node_s* nyt;
}AHC_tree_s;*/

typedef struct tag_AHC_symbolsInfo_s AHC_symbolsInfo_s;

/*typedef struct tag_AHc_symbolsInfo_s
{
    AHC_node_s*      symbolNodes_a[AHC_NUM_OF_SYMBOLS]; NYT will be at 256 place
   AHC_symbolsCount numOfAddedSymbols;
}AHC_symbolsInfo_s;*/


typedef struct tag_AHC_code_s AHC_code_s;
/*typedef struct tag_AHC_code_s
{
    IO_msg code[AHC_NUM_OF_SYMBOLS];
    AHC_counter codeLen;
}AHC_code_s;*/


typedef  struct tag_AHC_model_s AHC_model_s;
typedef const AHC_model_s AHC_model_cs;
/*typedef struct tag_AHC_model_s
{
    AHC_tree_s tree_s;
    AHC_symbolsInfo_s symbols_s;
    AHC_code_s code_s;
    IO_buffor_s buffor_s;
}AHC_model_s;*/


#endif /*__AHC_TYPES_H__*/