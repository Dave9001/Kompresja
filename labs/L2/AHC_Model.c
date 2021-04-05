#include "AHC_Model.h"
#include "IO/IO.h"
#include <stdio.h>

typedef enum tag_AHC_nodes_types_e
{
    AHC_NODE_INNER,
    AHC_NODE_SYMBOL,
    AHC_NODE_NYT
}AHC_NODES_TYPES_E;

typedef struct tag_AHC_node_s
{
    struct tag_AHC_node_s* parent;
    struct tag_AHC_node_s* rightChild;
    struct tag_AHC_node_s* leftChild;

    AHC_numOfOccur value;
    AHC_index id;
    AHC_NODES_TYPES_E type;
    AHC_symbol symbol;
}AHC_node_s;

typedef struct tag_AHC_tree_s
{
    AHC_node_s* root;
    AHC_node_s* nyt;
}AHC_tree_s;

typedef struct tag_AHC_symbolsInfo_s
{
    AHC_node_s*      symbolNodes_a[AHC_NUM_OF_SYMBOLS]; /*NYT will be at 256 place*/
    AHC_symbolsCount numOfAddedSymbols;
}AHC_symbolsInfo_s;

typedef struct tag_AHC_code_s
{
    AHC_symbol code[AHC_NUM_OF_SYMBOLS];
    AHC_counter codeLen;
}AHC_code_s;

typedef enum tag_IO_states_e
{
    IO_WRITING,
    IO_BUFF_IS_FULL,
}IO_states_e;

typedef struct tag_IO_buffor_s
{
    IO_msg message;
    IO_index lastBitIndexInMsg;
    IO_states_e state_e;
    const IO_FILEcp inputStreamPtr;
    const IO_FILEcp outputStreamPtr;
}IO_buffor_s;

typedef struct tag_AHC_model_s
{
    AHC_tree_s tree_s;
    AHC_symbolsInfo_s symbols_s;
    AHC_code_s code_s;
    IO_buffor_s buffor_s;
}AHC_model_s;



#define AHC_RESET_CODE_LEN(model_ps) model_ps->code_s.codeLen = 0U

extern AHC_model_s AHC_ModelInit(IO_buffor_s *const buffor_cps);
extern void AHC_ComputeNodeCode(
    AHC_node_cs *const node_cpcs,
    AHC_model_s *const model_cps
);
extern void AHC_ComputeSymbolCode(
    AHC_symbol_c symbol,
    AHC_model_s *const model_cps
);
extern void AHC_SendCode(AHC_model_s *const model_cps);
extern void AHC_SendHardCode(AHC_model_s *const model_cps);

static void AHC_ClenNode(
    AHC_node_s* currentNode_ps
);


extern AHC_model_s AHC_ModelInit(IO_buffor_s *const buffor_cps)
{
    AHC_index index;
    AHC_model_s model_s = {.buffor_s = *buffor_cps};

    index = 0;
    while(index < AHC_NUM_OF_SYMBOLS)
    {
        model_s.code_s.code[index] = 0U;
        model_s.symbols_s.symbolNodes_a[index] = NULL;
        
        index++;
    }
    model_s.code_s.codeLen = 0U;
    model_s.symbols_s.numOfAddedSymbols = 0U;

    model_s.tree_s.nyt = malloc(sizeof(*model_s.tree_s.nyt));
    if(NULL == model_s.tree_s.nyt)
    {
        ERROR_PRINT("Allocation mem. failed");
    }
    AHC_NYT_INIT(model_s.tree_s.nyt);

    model_s.tree_s.root = model_s.tree_s.nyt;

    return model_s;
}

extern void AHC_ComputeNodeCode(
    AHC_node_cs *const node_cpcs,
    AHC_model_s *const model_cps
)
{
    AHC_RESET_CODE_LEN(model_cps);
   
    if(node_cpcs != NULL)
    {
        AHC_index index = 0U;
        AHC_node_s *buble = (AHC_node_s *)node_cpcs;
        AHC_node_s *bubleParent = buble->parent;
        while(bubleParent != NULL)
        {
            if(buble == bubleParent->leftChild)
            {
                model_cps->code_s.code[index] = 0U;
            }
            else
            {
                model_cps->code_s.code[index] = 1U;
            }
            model_cps->code_s.codeLen++;

            index++;
            buble = bubleParent;
            bubleParent = buble->parent;
        }
    }
}

extern void AHC_ComputeSymbolCode(
    AHC_symbol_c symbol,
    AHC_model_s *const model_cps
)
{
    AHC_symbol bitPtr = 1U << AHC_MSB(bitPtr);
    AHC_index index = AHC_NumOfBitsInByte - 1U;
    while(index >= 0U)
    {
        if((symbol & bitPtr) != 0U)
        {
            model_cps->code_s.code[index] = 1U;
        }
        else
        {
            model_cps->code_s.code[index] = 0U;
        }
        model_cps->code_s.codeLen++;

        bitPtr >>= 1U;
        if(index == 0U)
        {
            break;
        }
        index--;
    }
    model_cps->code_s.codeLen = AHC_NumOfBitsInByte;
}

extern void AHC_SendCode(AHC_model_s *const model_cps)
{
    IO_WriteN(
        model_cps->code_s.code,
        model_cps->code_s.codeLen,
        &(model_cps->buffor_s)
    );
}

extern void AHC_SendHardCode(AHC_model_s *const model_cps)
{
    IO_index currentBitIndex = model_cps->buffor_s.lastBitIndexInMsg;
    while(currentBitIndex > 0U)
    {
        if(1U == model_cps->code_s.code[currentBitIndex])
        {
            model_cps->buffor_s.message |= 1U << currentBitIndex;
        }
        currentBitIndex--;
    }
    
    IO_WriteHard(
        model_cps->buffor_s.message,
        &(model_cps->buffor_s)
    );
}


extern void AHC_CleanModel(
    AHC_model_s * model_ps,
    AHC_node_s** symbolNode
)
{
    AHC_ClenNode(model_ps->tree_s.root);

    AHC_index indexSymbols = 0U;
    while(indexSymbols <= AHC_symbolsArraysLastIndex)
    {
        model_ps->symbols_s.symbolNodes_a[indexSymbols] = NULL;
        indexSymbols++;
    }

    IO_CloseIO(&(model_ps->buffor_s));
    *symbolNode = NULL;
}


static void AHC_ClenNode(
    AHC_node_s* currentNode_ps
)
{
    if(NULL == currentNode_ps)
    {
        return;
    }

    AHC_ClenNode(
        currentNode_ps->leftChild
    );

    AHC_ClenNode(
        currentNode_ps->rightChild
    );

    currentNode_ps->parent = NULL;
    currentNode_ps->leftChild= NULL;
    currentNode_ps->rightChild = NULL;
    currentNode_ps->type = 0U;
    currentNode_ps->value = 0U;
    currentNode_ps->id = 0U;
    currentNode_ps->symbol = 0U;

    free(currentNode_ps);
}
