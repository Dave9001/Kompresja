#include "AHC_Encoder.h"
#include "AHC_Model.h"
#include "AHC_Tree.h"
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


extern void AHC_Encoder(void);

static void AHC_EndOfTransmition(AHC_model_s *const model_cps);
static void AHC_ShortenCode(AHC_model_s *const model_cps);
static AHC_node_s *const AHC_NodeEndMsg(AHC_model_s *const model_cps);
static void AHC_SearchNodeEndMsg(
    AHC_node_s* currentNode_ps,
    AHC_node_s** msgNode_ps,
    AHC_index currentDeepthInTree
);

int main(void)
{
    AHC_Encoder();
}

extern void AHC_Encoder(void)
{
    IO_buffor_s buffor_s = IO_InitBuffor(NULL, "rb", "t1", "wb");
    AHC_model_s model_s = AHC_ModelInit(&buffor_s);
    AHC_node_s* symbolNode = NULL;

    IO_msg symbol = IO_READ(buffor_s.inputStreamPtr);
    while(IO_StillReadingFromStream(symbol, buffor_s.inputStreamPtr))
    {
        if(AHC_SymbolNotInTree(symbol, &model_s))
        {
            AHC_ComputeNodeCode(model_s.tree_s.nyt, &model_s);
            AHC_SendCode(&model_s);

            AHC_ComputeSymbolCode(symbol, &model_s);
            AHC_SendCode(&model_s);

            AHC_TreeAddSymbol(symbol, &model_s);

            symbolNode = AHC_SYMBOL_NODE(symbol, model_s);
            AHC_TreeOrderUpdate(symbolNode->parent, &model_s);
        }
        else
        {
            symbolNode = AHC_SYMBOL_NODE(symbol, model_s);
            AHC_ComputeNodeCode(symbolNode, &model_s);

            AHC_SendCode(&model_s);

            AHC_INCREMENT_OCCURENCE_NUM(symbol, model_s);
            AHC_TreeOrderUpdate(symbolNode, &model_s);
        }
        symbol = IO_READ(buffor_s.inputStreamPtr);
    }
    AHC_EndOfTransmition(&model_s);

    AHC_CleanModel(&model_s, &symbolNode);
}

static void AHC_EndOfTransmition(AHC_model_s *const model_cps)
{
    if(model_cps->buffor_s.lastBitIndexInMsg > 0U)
    {
        if(model_cps->tree_s.nyt != NULL)
        {
            AHC_ComputeNodeCode(model_cps->tree_s.nyt, model_cps);
            AHC_ShortenCode(model_cps);
            AHC_SendHardCode(model_cps);
        }
        else
        {
            AHC_node_s *const nodeEndOfMsg = AHC_NodeEndMsg(model_cps);
            AHC_ComputeNodeCode(nodeEndOfMsg, model_cps);
            AHC_SendHardCode(model_cps);
        }
    }
    else
    {
        AHC_SendHardCode(model_cps);
    }
}


static void AHC_ShortenCode(AHC_model_s *const model_cps)
{
    AHC_symbol tmpArr[256] = {};
    AHC_index index = model_cps->code_s.codeLen - 1U;
    AHC_index lowerLimit = model_cps->buffor_s.lastBitIndexInMsg;
    while(index >= 0U && lowerLimit > 0U)
    {
        tmpArr[lowerLimit] = model_cps->code_s.code[index];

        if(index != 0U && lowerLimit > 0U)
        {
            index--;
            lowerLimit--;
        }
        else
        {
            break;
        }
    }

    index = model_cps->buffor_s.lastBitIndexInMsg;
    while(index > 0U)
    {
        model_cps->code_s.code[index] = tmpArr[index];
        
        if(index != 0U)
        {
            index--;
        }
        else
        {
            break;
        } 
    }

    model_cps->code_s.codeLen = model_cps->buffor_s.lastBitIndexInMsg;
}

static AHC_node_s *const AHC_NodeEndMsg(AHC_model_s *const model_cps)
{
    AHC_index wantedDepth = model_cps->buffor_s.lastBitIndexInMsg;

    AHC_node_s * foundedNode = NULL;
    AHC_SearchNodeEndMsg(
        model_cps->tree_s.root,
        &foundedNode,
        wantedDepth
    );

    return (AHC_node_s *const)foundedNode;
}

static void AHC_SearchNodeEndMsg(
    AHC_node_s* currentNode_ps,
    AHC_node_s** msgNode_ps,
    AHC_index currentDeepthInTree
)
{
    if(NULL != *msgNode_ps)
    {
        return;
    }

    if(0 == currentDeepthInTree)   /*Don't need to search below (in tree)*/
    {
        return;
    }

    if(NULL == currentNode_ps)
    {
        return;
    }

    AHC_SearchNodeEndMsg(
        currentNode_ps->leftChild,
        msgNode_ps,
        currentDeepthInTree - 1U
    );

    AHC_SearchNodeEndMsg(
        currentNode_ps->rightChild,
        msgNode_ps,
        currentDeepthInTree - 1U
    );


    if(currentNode_ps->type == AHC_NODE_INNER) /*same block*/
    {
        *msgNode_ps = currentNode_ps;
    }
}