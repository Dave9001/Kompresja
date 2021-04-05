#include "AHC_Tree.h"

#define AHC_PARENT_OF(node) (node->parent)

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
    IO_msg code[AHC_NUM_OF_SYMBOLS];
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


extern bool AHC_SymbolNotInTree(
    AHC_symbol_c symbol,
    AHC_model_cs *const model_ps
);
extern void AHC_TreeAddSymbol(
    AHC_symbol_c symbol,
    AHC_model_s *const model_cpcs
);
extern void AHC_TreeOrderUpdate(
    AHC_node_s *const startNode_cps,
    AHC_model_s *const model_cps
);


static inline bool AHC_TheLastSymbol(
    AHC_symbol_c symbol,
    AHC_model_cs *const model_cpcs
);
static inline void AHC_ReplaceWithNyt(
    AHC_symbol_c symbol,
    AHC_model_s *const model_cps
);
static inline void AHC_AddSymbolToSymbols(
    AHC_symbol_c symbol,
    AHC_node_cs *const symbolNode_cpcs,
    AHC_model_s *const model_cps
);
static AHC_node_s* AHC_TreeAddNode(
    AHC_node_cs *const parent, 
    AHC_NODES_TYPES_E nodeType,
    AHC_index id,
    AHC_symbol symbol,
    AHC_numOfOccur value
);
static AHC_node_s* AHC_BlockBiggestValueNode(
    AHC_node_s* root_ps,
    AHC_node_s* updatedNode_ps
);
static void AHC_BlockPostorderSearch(
    AHC_node_s* currentNode_ps,
    AHC_node_s** biggestNode_ps,
    AHC_numOfOccur blockValue,
    AHC_index currentDeepthInTree
);
static inline void AHC_SwitchNodesPosition(
    AHC_node_s** nodeA_ps,
    AHC_node_s** nodeB_ps
);

extern bool AHC_SymbolNotInTree(
    AHC_symbol_c symbol,
    AHC_model_cs *const model_cpcs
)
{
    AHC_node_s *symbolNode_ps = NULL;
    symbolNode_ps = model_cpcs->symbols_s.symbolNodes_a[symbol];

    bool notInTree = TRUE;
    if(NULL != symbolNode_ps)
    {
        notInTree = FALSE;
    }

    return notInTree;
}

extern void AHC_TreeOrderUpdate(
    AHC_node_s *const startNode_cps,
    AHC_model_s *const model_cps
)
{
    AHC_node_s* currentUpdatedNode_ps = startNode_cps;

    AHC_node_s* biggestNode_ps = NULL;
    AHC_node_s* treeRoot_ps = model_cps->tree_s.root;

    while(currentUpdatedNode_ps != treeRoot_ps &&
          currentUpdatedNode_ps != NULL
    )
    {
        currentUpdatedNode_ps = AHC_PARENT_OF(currentUpdatedNode_ps);
        biggestNode_ps = AHC_BlockBiggestValueNode(
                            model_cps->tree_s.root,
                            currentUpdatedNode_ps
                         );

        if(biggestNode_ps != currentUpdatedNode_ps &&
           biggestNode_ps != NULL
        )
        {
            AHC_SwitchNodesPosition(&currentUpdatedNode_ps, &biggestNode_ps);
        }

        currentUpdatedNode_ps->value = currentUpdatedNode_ps->value + 1U;
    }
}

extern void AHC_TreeAddSymbol(
    AHC_symbol_c symbol,
    AHC_model_s *const model_cpcs
)
{
    if(AHC_TheLastSymbol(symbol, model_cpcs))
    {
        AHC_ReplaceWithNyt(symbol, model_cpcs);
    }
    else
    {
        AHC_node_s* old_NYT = model_cpcs->tree_s.nyt;

        AHC_node_s*   new_NYT  = AHC_TreeAddNode(
                                    old_NYT,
                                    AHC_NODE_NYT,
                                    old_NYT->id - 2U,
                                    old_NYT->symbol,
                                    0U
                                 );
        AHC_node_s* new_Symbol = AHC_TreeAddNode(
                                    old_NYT,
                                    AHC_NODE_SYMBOL,
                                    old_NYT->id - 1U,
                                    symbol,
                                    1U
                                 );

        AHC_AddSymbolToSymbols(symbol, new_Symbol, model_cpcs);

        new_NYT->symbol = old_NYT->symbol;

        old_NYT->leftChild  = new_NYT;
        old_NYT->rightChild = new_Symbol;
        old_NYT->symbol     = 0U;
        old_NYT->type       = AHC_NODE_INNER;
        old_NYT->value      = 1U;

        new_NYT->parent = old_NYT;
        model_cpcs->tree_s.nyt = new_NYT;
    }

}



static inline bool AHC_TheLastSymbol(
    AHC_symbol_c symbol,
    AHC_model_cs *const model_cpcs
)
{
    bool notLast = FALSE;
    if(model_cpcs->symbols_s.numOfAddedSymbols == AHC_NUM_OF_SYMBOLS - 1U)
    {
        notLast = TRUE;
    }
    else if(model_cpcs->symbols_s.numOfAddedSymbols > AHC_NUM_OF_SYMBOLS - 1U)
    {
        ERROR_PRINT("Double nyt exchange");
    }
    return notLast;
}

static inline void AHC_ReplaceWithNyt(
    AHC_symbol_c symbol,
    AHC_model_s *const model_cps
)
{
    model_cps->tree_s.nyt->leftChild  = NULL;
    model_cps->tree_s.nyt->rightChild = NULL;
    model_cps->tree_s.nyt->symbol     = symbol;
    model_cps->tree_s.nyt->type       = AHC_NODE_SYMBOL;
    model_cps->tree_s.nyt->value      = 1U;
    
    model_cps->symbols_s.symbolNodes_a[symbol] = model_cps->tree_s.nyt;
    model_cps->symbols_s.numOfAddedSymbols++;

    model_cps->tree_s.nyt = NULL;
}

static inline void AHC_AddSymbolToSymbols(
    AHC_symbol_c symbol,
    AHC_node_cs *const symbolNode_cpcs,
    AHC_model_s *const model_cps
)
{
    model_cps->symbols_s.numOfAddedSymbols++;
    model_cps->symbols_s.symbolNodes_a[symbol] = (AHC_node_s *const)symbolNode_cpcs;
}

static AHC_node_s* AHC_TreeAddNode(
    AHC_node_cs *const parent, 
    AHC_NODES_TYPES_E nodeType,
    AHC_index id,
    AHC_symbol symbol,
    AHC_numOfOccur value
)
{
    AHC_node_s* node = malloc(sizeof(*node));
    if(node == NULL)
    {
        ERROR_PRINT("malloc FAILED to allocate mem:\n");
    }

    node->parent = (AHC_node_s *)parent;
    node->rightChild = NULL;
    node->leftChild = NULL;

    node->value = value;
    node->symbol = symbol;
    node->id = id;
    node->type = nodeType;

    return node;
}


static AHC_node_s* AHC_BlockBiggestValueNode(
    AHC_node_s* root_ps,
    AHC_node_s* updatedNode_ps
)
{
    AHC_node_s*    biggestNode_ps = updatedNode_ps;
    AHC_numOfOccur blockValue = updatedNode_ps->value;

    AHC_index nodeDeepthInTree = 0U;
    AHC_node_s* bubble = updatedNode_ps;
    
    while(bubble->parent != NULL)
    {
        nodeDeepthInTree++;
        bubble = bubble->parent;
    }

    AHC_BlockPostorderSearch(
        root_ps,
        &biggestNode_ps,
        blockValue,
        nodeDeepthInTree
    );

    return biggestNode_ps;
}

static void AHC_BlockPostorderSearch(
    AHC_node_s* currentNode_ps,
    AHC_node_s** biggestNode_ps,
    AHC_numOfOccur blockValue,
    AHC_index currentDeepthInTree
)
{
    if(0 == currentDeepthInTree)   /*Don't need to search below (in tree)*/
    {
        return;
    }

    if(NULL == currentNode_ps)
    {
        return;
    }

    AHC_BlockPostorderSearch(
        currentNode_ps->leftChild,
        biggestNode_ps,
        blockValue,
        currentDeepthInTree - 1U
    );

    AHC_BlockPostorderSearch(
        currentNode_ps->rightChild,
        biggestNode_ps,
        blockValue,
        currentDeepthInTree - 1U
    );


    if(currentNode_ps->value == blockValue) /*same block*/
    {
        if(currentNode_ps->id > (*biggestNode_ps)->id)
        {
            (*biggestNode_ps) = currentNode_ps;
        }
    }
}

static inline void AHC_SwitchNodesPosition(
    AHC_node_s** nodeA_ps,
    AHC_node_s** nodeB_ps
)
{
    AHC_node_s* oldNodeAParanet_s = (*nodeA_ps)->parent;
    AHC_node_s* leftChildOfMyParent  = NULL;

    leftChildOfMyParent = (*nodeA_ps)->parent->leftChild;
    if((*nodeA_ps) == leftChildOfMyParent)
    {
        (*nodeA_ps)->parent->leftChild = (*nodeB_ps);
    }
    else
    {
        (*nodeA_ps)->parent->rightChild = (*nodeB_ps);
    }
    (*nodeA_ps)->parent = (*nodeB_ps)->parent;
    
    leftChildOfMyParent = (*nodeB_ps)->parent->leftChild;
    if((*nodeB_ps) == leftChildOfMyParent)
    {
        (*nodeB_ps)->parent->leftChild = (*nodeA_ps);
    }
    else
    {
        (*nodeB_ps)->parent->rightChild = (*nodeA_ps);
    }
    (*nodeB_ps)->parent = oldNodeAParanet_s;

    AHC_index aId = (*nodeA_ps)->id;
    (*nodeA_ps)->id  = (*nodeB_ps)->id;
    (*nodeB_ps)->id  = aId;
}