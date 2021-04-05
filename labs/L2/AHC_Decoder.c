#include "AHC_Model.h"
#include "AHC_Tree.h"
#include "IO/IO.h"
#include <stdio.h>




#define AHC_NYT_ADD_TO_SYMBOLS(nyt_ps, symbols_s) symbols_s.symbolNodes_a[nyt_ps->symbol] = nyt_ps
#define AHC_PTR_SYMBOL_NODE_UPDATE(ptr, symbol, symbols_ps) do{ptr = symbols_ps.symbolNodes_a[symbol];}while(FALSE)
#define AHC_PRINT(symbol) putc(symbol, ptrOutput);//fwrite(&symbol,sizeof(symbol),1,ptrOutput);//putc(symbol, stdout)
#define AHC_LAST_BIT (0U)
#define AHC_MSB_INDEX(type) (sizeof(type) * AHC_NumOfBitsInByte - 1U)
#define AHC_READ_BIT(decoder_s) do{\
        decoder_s.symbolToAdd |= \
                    (decoder_s.readedCode & decoder_s.codeNeedle) >> \
                    decoder_s.readedSymbolBitsNum\
        ;\
}while(FALSE)\


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


typedef enum
{
    AHC_STATE_LEAF,
    AHC_STATE_TRAVEL,
    AHC_STATE_ADD_NEW_SYMBOL,
    AHC_STATE_READ_NEW_SYMBOL
}AHC_STATES_E;

typedef struct tag_AHC_decoderBuffor
{
    AHC_symbol symbolToAdd;
    AHC_symbol readedSymbolBitsNum;

    AHC_symbol readedCode;
    AHC_symbol codeNeedle;
    AHC_symbol readedCodeBitsNum;

    AHC_STATES_E state;

    AHC_node_s* nodeTraveler;
}AHC_decoder_s;



extern void AHC_Decoder(void);

static void AHC_DecoderInit(AHC_decoder_s* decoder_ps);
static inline void AHC_DecoderReset(AHC_decoder_s* decoder_ps, AHC_tree_s* tree_ps);
static inline void AHC_ResetCodeData(AHC_decoder_s* decoder_s, AHC_symbol code);
static inline void AHC_SetState(AHC_decoder_s* decoder_sp);
static inline void AHC_MoveDecoder(AHC_decoder_s* decoder_p, AHC_tree_s* tree_ps);

static inline void AHC_DecoderModelStart(
    AHC_decoder_s *const decoder_cps,
    AHC_model_s *const model_cps
);



int main(void)
{
    AHC_Decoder();
    return 0;
}


static inline void AHC_DecoderModelStart(
    AHC_decoder_s *const decoder_cps,
    AHC_model_s *const model_cps
)
{
    AHC_DecoderInit(decoder_cps);
    decoder_cps->nodeTraveler = model_cps->tree_s.root;
    
    AHC_symbol symbol = IO_READ(model_cps->buffor_s.inputStreamPtr);
    
    AHC_TreeAddSymbol(symbol, model_cps);
    AHC_node_s* symbolNode = model_cps->symbols_s.symbolNodes_a[symbol];
    AHC_TreeOrderUpdate(symbolNode->parent, model_cps);

    IO_WriteHard(symbol, (IO_buffor_s*const)&(model_cps->buffor_s));
}


extern void AHC_Decoder(void)
{
    IO_buffor_s buffor_s = IO_InitBuffor("t1", "rb", "d1", "wb");
    AHC_model_s model_s = AHC_ModelInit(&buffor_s);
    AHC_node_s* symbolNode = NULL;

    AHC_decoder_s decoder_s;
    AHC_DecoderModelStart( &decoder_s, &model_s);

    AHC_symbol code = IO_READ(buffor_s.inputStreamPtr);
    while(IO_StillReadingFromStream(code, buffor_s.inputStreamPtr))
    {
        AHC_ResetCodeData(&decoder_s, code);
        printf("%x\n",code);
        while(decoder_s.readedCodeBitsNum < 8U)
        {
            AHC_SetState(&decoder_s);
            printf("state: %d\n", decoder_s.state);
            if(AHC_STATE_LEAF == decoder_s.state)
            {
                AHC_ComputeSymbolCode(
                    decoder_s.nodeTraveler->symbol,
                    &model_s
                );
                AHC_SendCode(&model_s);

                AHC_INCREMENT_OCCURENCE_NUM(code, model_s);
                AHC_TreeOrderUpdate(decoder_s.nodeTraveler, &model_s);
            }
            else if(AHC_STATE_ADD_NEW_SYMBOL == decoder_s.state)
            {
                AHC_ComputeSymbolCode(code, &model_s);
                AHC_SendCode(&model_s);

                AHC_TreeAddSymbol(code, &model_s);

                symbolNode = AHC_SYMBOL_NODE(code, model_s);
                AHC_TreeOrderUpdate(symbolNode->parent, &model_s);
            }
            else if(AHC_STATE_READ_NEW_SYMBOL == decoder_s.state)
            {
                AHC_READ_BIT(decoder_s);
            }
 
            AHC_MoveDecoder(&decoder_s, &(model_s.tree_s));
        }
        
        code = IO_READ(buffor_s.inputStreamPtr);
    }

    AHC_CleanModel(&model_s, &symbolNode);
}



static void AHC_DecoderInit(AHC_decoder_s* decoder_ps)
{
    decoder_ps->symbolToAdd = 0U;
    decoder_ps->readedCode = 0U;
    decoder_ps->codeNeedle = MASK_BIT_7;
    decoder_ps->readedCodeBitsNum = 0U;
    decoder_ps->readedSymbolBitsNum = 0U;

    decoder_ps->state = AHC_STATE_TRAVEL;

    decoder_ps->nodeTraveler = NULL;
}

static inline void AHC_DecoderReset(AHC_decoder_s* decoder_ps, AHC_tree_s* tree_ps)
{
    decoder_ps->symbolToAdd = 0U;
    decoder_ps->readedCode = 0U;
    decoder_ps->codeNeedle = MASK_BIT_7;
    decoder_ps->readedCodeBitsNum = 0U;
    decoder_ps->readedSymbolBitsNum = 0U;

    decoder_ps->state = AHC_STATE_TRAVEL;

    decoder_ps->nodeTraveler = tree_ps->root;
}


static inline void AHC_ResetCodeData(AHC_decoder_s* decoder_ps, AHC_symbol code)
{
    decoder_ps->readedCode = code;
    decoder_ps->readedCodeBitsNum = 0U;
}


static inline void AHC_SetState(AHC_decoder_s* decoder_ps)
{
    AHC_STATES_E state = decoder_ps->state;
    switch(state)
    {
        case AHC_STATE_ADD_NEW_SYMBOL:  state = AHC_STATE_TRAVEL;
                                        break;

        case AHC_STATE_LEAF:            state = AHC_STATE_TRAVEL;
                                        break;

        case AHC_STATE_READ_NEW_SYMBOL: if(AHC_NumOfBitsInByte == decoder_ps->readedSymbolBitsNum)
                                        {
                                            state = AHC_STATE_ADD_NEW_SYMBOL;
                                        }
                                        break;
                                        
        case AHC_STATE_TRAVEL:          if(AHC_NODE_NYT == decoder_ps->nodeTraveler->type)
                                        {                                        
                                            state = AHC_STATE_READ_NEW_SYMBOL;
                                            decoder_ps->readedSymbolBitsNum = 0U;
                                            decoder_ps->symbolToAdd = 0U;
                                        }
                                        else if(AHC_NODE_SYMBOL == decoder_ps->nodeTraveler->type)
                                        {
                                            state = AHC_STATE_LEAF;
                                        }
                                        break;
    }

    decoder_ps->state = state;
}


static inline void AHC_MoveDecoder(AHC_decoder_s* decoder_ps, AHC_tree_s* tree_ps)
{
    AHC_STATES_E state = decoder_ps->state;
    switch(state)
    {
        case AHC_STATE_ADD_NEW_SYMBOL:  decoder_ps->nodeTraveler = tree_ps->root;
                                        decoder_ps->readedSymbolBitsNum = 0U;
                                        decoder_ps->symbolToAdd = 0U;
                                        break;

        case AHC_STATE_READ_NEW_SYMBOL: 
                                        decoder_ps->readedSymbolBitsNum++;                                        
                                        decoder_ps->readedCode <<= 1U;
                                        decoder_ps->readedCodeBitsNum++;
                                        break;
        
        case AHC_STATE_LEAF:            decoder_ps->nodeTraveler = tree_ps->root;
                                        break;
                                        
        case AHC_STATE_TRAVEL:          if((decoder_ps->readedCode & decoder_ps->codeNeedle) == MASK_BIT_7)
                                        {
                                            decoder_ps->nodeTraveler = decoder_ps->nodeTraveler->rightChild;
                                        }
                                        else
                                        {
                                            decoder_ps->nodeTraveler = decoder_ps->nodeTraveler->leftChild;
                                        }
                                        
                                        decoder_ps->readedCode <<= 1U;
                                        decoder_ps->readedCodeBitsNum += 1U;
                                        break;
    }

}
