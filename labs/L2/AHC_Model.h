#ifndef __AHC_MODEL_H__
#define __AHC_MODEL_H__


#include "AHC_Types.h"

#define AHC_INCREMENT_OCCURENCE_NUM(symbol, model_s) model_s.symbols_s.symbolNodes_a[symbol]->value++;


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
extern void AHC_CleanModel(
    AHC_model_s * model_ps,
    AHC_node_s ** symbolNode
);
extern void AHC_SendHardCode(AHC_model_s *const model_cps);

#endif /*__AHC_MODEL_H__*/