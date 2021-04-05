#ifndef __AHC_TREE_H__
#define __AHC_TREE_H___

#include "AHC_Types.h"


#define AHC_SYMBOL_NODE(symbol, model) model.symbols_s.symbolNodes_a[symbol]


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

#endif/*__AHC_TREE_H___*/