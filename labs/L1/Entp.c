/**
 *  @Author: Self
 *  
 *  @Brief: 
 *  @Usage: 
 * 
 *  @Limitations: 
 * 
 *  @Violation:
 * 
 * @Date: 1.08.2018
 **/

/*******************************************************************************
 *                              INCLUDS
 ******************************************************************************/

/**
 *  @Brief: includs for types and declarations for binary search
 **/
#include "Entp.h"

/**
 *  @Brief: included for NULL and malloc declaration
 *  @Note: can be deleted when stdtypes powstanie
 **/
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*******************************************************************************
 *                              DECLARATIONS
 ******************************************************************************/
static Entp_metrics_t * * RetAllocCondProbabArray(void);
static Entp_metricsCollection_s * RetAllocMetricsCollection(void);
static Entp_metrics_t * RetAllocSymbolsProbaArr(const Entp_index_t sizeOfArray);
static Entp_occurr_t * RetAllocOccurrArray(const Entp_index_t sizeOfArray);
static Entp_occurr_t * * RetAllocCondOccurrArray(const Entp_index_t sizeOfArray);
static inline void InitOccurrArrWithValue(
    Entp_occurr_t* const restrict arrayToInit,
    const Entp_index_t arrayLength,
    const Entp_occurr_t initValue
);
static inline void InitProbabArrWithValue(
            Entp_metrics_t *const restrict arrayToInit,
            const Entp_index_t arrayLength,
            const Entp_metrics_t initValue
);

void CopyCondProbabToMetrics(
    Entp_metricsCollection_s *const restrict metrics,
    Entp_metrics_t *const *const restrict condProbab_aa
);

static void ReadAndCollectData(Entp_occurr_s *const restrict occurrCollection);
static inline FILE * OpenStreamInMode(
    const char *const restrict streamName, 
    const char *const restrict mode
);
static inline void CloseStream(FILE* restrict ptrToFile);


static Entp_metricsCollection_s * CalcMetrics(
    const Entp_occurr_s *const restrict occurrCollection_s
);
static void CalcSymbolProbab(
    Entp_metricsCollection_s *const restrict metrics,
    const Entp_occurr_s *const restrict occurrCollection_s
);
static void CalcCondSymbolProbab(
    Entp_metricsCollection_s *const restrict metrics,
    const Entp_occurr_s *const restrict occurrCollection_s
);
static void CalcEntropy( Entp_metricsCollection_s *const restrict metrics);
static void CalcCondEntropy( Entp_metricsCollection_s *const restrict metrics);

extern void Entp_Manager(void);
static Entp_occurr_s Initializing(const Entp_index_t sizeOfSymbolsPoll);

static void CleanMemory(
    Entp_metricsCollection_s* metrics_s,
    Entp_occurr_s* occurrCollection_s
);
static void PrintMetrics(const Entp_metricsCollection_s *const metrics);

/********************************************************************************
*                               EXTERN DEFINITIONS
*********************************************************************************/                               

extern void Entp_Manager(void)
{
    Entp_occurr_s occurrCollection_s = Initializing(ALPHABET_SIZE);
    
    ReadAndCollectData(&occurrCollection_s);

    Entp_metricsCollection_s* metrics_s = CalcMetrics(&occurrCollection_s);

    PrintMetrics(metrics_s);

    CleanMemory(metrics_s, &occurrCollection_s);
}

/********************************************************************************
*                               STATIC DEFINITIONS
*********************************************************************************/         

static Entp_metricsCollection_s * CalcMetrics(
    const Entp_occurr_s *const restrict occurrCollection_s
)
{
    Entp_metricsCollection_s * metrics = RetAllocMetricsCollection();

    CalcSymbolProbab(metrics, occurrCollection_s);
    CalcCondSymbolProbab(metrics, occurrCollection_s);
    
    CalcEntropy(metrics);
    CalcCondEntropy(metrics);

    return metrics;
}
/*
static int Comapre(const void * arg1, const void * arg2)
{
    int retVal = 0;
    if(((Entp_sort_s *)arg1)->probability > ((Entp_sort_s *)arg2)->probability)
    {
        retVal = 1;
    }
    else if(((Entp_sort_s *)arg1)->probability < ((Entp_sort_s *)arg2)->probability)
    {
        retVal = -1;
    }
    return retVal;
}
*/
static void CalcCondEntropy( Entp_metricsCollection_s *const restrict metrics)
{
    Entp_metrics_t pairPobab   = (Entp_metrics_t)0.0;
    Entp_metrics_t condProbab  = (Entp_metrics_t)0.0;
    Entp_metrics_t condEntropy = (Entp_metrics_t)0.0;
    Entp_metrics_t information = (Entp_metrics_t)0.0;
    Entp_index_t symbolAfter   = (Entp_index_t)0U;
    Entp_index_t symbol        = 0;
    while(symbol < ALPHABET_SIZE)
    {
        symbolAfter = 0;
        while(symbolAfter < ALPHABET_SIZE)
        {
            if(metrics->condSymbolProbab_aa[symbol][symbolAfter] > (Entp_metrics_t )0.0)
            {
                //pairPobab   = metrics->condSymbolProbab_aa[symbol][symbolAfter];
                pairPobab   = metrics->symbolsProbability_a[symbol] * metrics->symbolsProbability_a[symbolAfter]; 
                condProbab  = pairPobab / metrics->symbolsProbability_a[symbolAfter];
                if((Entp_metrics_t)0.0 != condProbab)
                {
                    information = log2l(condProbab);
                    condEntropy += pairPobab * information;
                }
            }
            symbolAfter++;
        }
        symbol++;
    }
    // for symbol== 0

    
    condEntropy *= (Entp_metrics_t)-1.0;

    //printf("COND ENTROPY: %.20Le\n", condEntropy);
    
    metrics->condEntropy = condEntropy;
}

static void CalcEntropy( Entp_metricsCollection_s *const restrict metrics)
{
    Entp_metrics_t entropy = (Entp_metrics_t)0.0;
    Entp_metrics_t information = (Entp_metrics_t)0.0;
    Entp_index_t symbol = ALPHABET_SIZE - 1;
    while(symbol > 0)
    {
        if(metrics->symbolsProbability_a[symbol] > (Entp_metrics_t )0.0)
        {
            information = ((Entp_metrics_t)-1.0 * log2l(metrics->symbolsProbability_a[symbol]));
            entropy     += information * metrics->symbolsProbability_a[symbol];
        }
       symbol--;
    }
    // for symbol== 0
    if(metrics->symbolsProbability_a[symbol] > (Entp_metrics_t )0.0)
    {
        information = ((Entp_metrics_t)-1.0 * log2l(metrics->symbolsProbability_a[symbol]));
        entropy     += information * metrics->symbolsProbability_a[symbol];
    }
    
    metrics->entropy = entropy;
}

static void CalcSymbolProbab(
    Entp_metricsCollection_s *const restrict metrics,
    const Entp_occurr_s *const restrict occurrCollection_s
)
{
    Entp_omega_t omega = 0.0;
    Entp_index_t i = 0U;
    while(i < ALPHABET_SIZE)
    {
        omega += occurrCollection_s->symbolsOccurrArr_a[i];
        i++;
    }

    metrics->omega = omega;

    i = 0U;
    while(i < ALPHABET_SIZE)
    {
        metrics->symbolsProbability_a[i] =
             occurrCollection_s->symbolsOccurrArr_a[i] / (Entp_metrics_t)omega;
        i++;
    }
}

static void CalcCondSymbolProbab(
    Entp_metricsCollection_s *const restrict metrics,
    const Entp_occurr_s *const restrict occurrCollection_s
)
{
    Entp_metrics_t * * condProbab_aa = RetAllocCondProbabArray();
    
    Entp_index_t symbol = (Entp_index_t)0U;
    Entp_index_t symbAfterSymbol = (Entp_index_t)0U;
    Entp_omega_t totalNumOfSymbAfterSymbol = (Entp_omega_t)0U;
    while(symbol < ALPHABET_SIZE)
    {
        totalNumOfSymbAfterSymbol = (Entp_omega_t)0U;
        symbAfterSymbol = (Entp_index_t)0U;
        while(symbAfterSymbol < ALPHABET_SIZE)
        {
            if( occurrCollection_s->symbolsCondOccurrArr_aa[symbol][symbAfterSymbol] 
                != (Entp_occurr_t)0U
            )
            {
                totalNumOfSymbAfterSymbol += occurrCollection_s->symbolsCondOccurrArr_aa[symbol][symbAfterSymbol];
            }
            symbAfterSymbol++;
        }
        
        symbAfterSymbol = (Entp_index_t)0U;
        while(symbAfterSymbol < ALPHABET_SIZE)
        {
           if(occurrCollection_s->symbolsCondOccurrArr_aa[symbol][symbAfterSymbol] != (Entp_occurr_t)0
                && totalNumOfSymbAfterSymbol > (Entp_omega_t)0
            )
            {
                condProbab_aa[symbol][symbAfterSymbol] =
                     (Entp_metrics_t)occurrCollection_s->symbolsCondOccurrArr_aa[symbol][symbAfterSymbol] / (Entp_metrics_t)totalNumOfSymbAfterSymbol;
            }
            symbAfterSymbol++;
        }

        symbol++;
    }

    CopyCondProbabToMetrics(metrics, condProbab_aa);
}

void CopyCondProbabToMetrics(Entp_metricsCollection_s *const restrict metrics,
    Entp_metrics_t *const *const restrict condProbab_aa)
{
    Entp_metricsCollection_s metricsCopy = {
        .symbolsProbability_a = metrics->symbolsProbability_a,
        .entropy = metrics->entropy,
        .omega = metrics->omega,
        .condSymbolProbab_aa = (Entp_metrics_t**)condProbab_aa
    };

    memcpy(metrics, &metricsCopy, sizeof(metricsCopy));
}

static Entp_metrics_t * * RetAllocCondProbabArray(void)
{
    Entp_metrics_t ** symbolCondProbab_aa = malloc(sizeof(Entp_metrics_t **) * ALPHABET_SIZE);
    if(NULL == symbolCondProbab_aa)
    { /* malloc failed */
        ERROR_PRINT("malloc FAILED to allocate memory");
    }

    const Entp_metrics_t initValue = (Entp_metrics_t)0.0;
    Entp_index_t i = 0U;
    for(i = 0U; i < ALPHABET_SIZE; i++)
    {
        symbolCondProbab_aa[i] = malloc(sizeof(**symbolCondProbab_aa) * ALPHABET_SIZE);
        if(NULL == symbolCondProbab_aa[i])
        { /* malloc failed */
            ERROR_PRINT("malloc FAILED to allocate mem. in FUNC:");
        }
        InitProbabArrWithValue(symbolCondProbab_aa[i], ALPHABET_SIZE, initValue);
    }

    return symbolCondProbab_aa;
}

static Entp_occurr_t * RetAllocOccurrArray(const Entp_index_t sizeOfArray)
{
    if(sizeOfArray > ARRAY_LENGTH_LIMIT)
    {   /* array length is to big  */
        ERROR_PRINT("given size of array wast TO BIG");
    }

    Entp_occurr_t* const symbolOccurr_a = malloc(sizeof(*symbolOccurr_a) * sizeOfArray);
    if(NULL == symbolOccurr_a)
    { /* malloc failed */
       ERROR_PRINT("malloc FAILED to allocate memory");
    }
    
    const Entp_occurr_t initValue = 0U;
    InitOccurrArrWithValue(symbolOccurr_a, sizeOfArray, initValue);
    
    return symbolOccurr_a;
}

static Entp_occurr_t * * RetAllocCondOccurrArray(const Entp_index_t sizeOfArray)
{
    if(sizeOfArray > ARRAY_LENGTH_LIMIT)
    {   /* array length is to big  */
        ERROR_PRINT("given size of array wast TO BIG");
    }

    Entp_occurr_t ** symbolCondOccurr_a = malloc(sizeof(Entp_occurr_t **) * sizeOfArray);
    if(NULL == symbolCondOccurr_a)
    { /* malloc failed */
        ERROR_PRINT("malloc FAILED to allocate memory");
    }

    const Entp_occurr_t initValue = 0U;
    Entp_index_t i = 0U;
    for(i = 0U; i < sizeOfArray; i++)
    {
        symbolCondOccurr_a[i] = malloc(sizeof(**symbolCondOccurr_a) * sizeOfArray);
        if(NULL == symbolCondOccurr_a[i])
        { /* malloc failed */
            ERROR_PRINT("malloc FAILED to allocate mem. in FUNC:");
        }
        InitOccurrArrWithValue(symbolCondOccurr_a[i], sizeOfArray, initValue);
    }

    return symbolCondOccurr_a;
}

static void ReadAndCollectData(Entp_occurr_s *const restrict occurrCollection_s)
{
    FILE * fileWithDataInput = OpenStreamInMode(STREAM_SOURCE, MODE);

    Entp_symbol_t prevSymbol = FIRST_SYMBOL;
    Entp_symbol_t symbol = getc(fileWithDataInput);
    while(symbol != (Entp_symbol_t)EOF)
    {   
        INC_1D_ARRAY_CELL_VAL(occurrCollection_s->symbolsOccurrArr_a, symbol);
        INC_2D_ARRAY_CELL_VAL(
            occurrCollection_s->symbolsCondOccurrArr_aa,
            prevSymbol, symbol
        );
        prevSymbol = symbol;
        symbol = getc(fileWithDataInput);
    }
    /* [prevSymbol][EOF] won't be added */

    CloseStream(fileWithDataInput);
}

static inline FILE * OpenStreamInMode(
    const char *const restrict streamName_p, 
    const char *const restrict mode_p
)
{
    int isSTDIN = strcmp(streamName_p, "stdin");
    FILE * ptrToFile = (STDIN_CPR == isSTDIN) ? stdin : fopen(streamName_p, mode_p);
    if(NULL == ptrToFile)
    {
        ERROR_PRINT("file with data input could not be opened");
    }

    return ptrToFile;
}

static inline void CloseStream(FILE* restrict ptrToFile_p)
{
    if(NULL == ptrToFile_p)
    {
        ERROR_PRINT("File to close have address NULL");
    }

    if(stdin != ptrToFile_p){
        fclose((FILE*)ptrToFile_p);
        ptrToFile_p = NULL;
    }
}

static Entp_occurr_s Initializing(const Entp_index_t sizeOfSymbolsPoll)
{
    Entp_occurr_t* symbolOccurr_p = RetAllocOccurrArray(sizeOfSymbolsPoll);
    Entp_occurr_t * * symbolCondOccurr_p = RetAllocCondOccurrArray(sizeOfSymbolsPoll);

    Entp_occurr_s occurrCollection = {.symbolsOccurrArr_a     = symbolOccurr_p,
                                      .symbolsCondOccurrArr_aa = symbolCondOccurr_p
                                     };

    return occurrCollection;
}

static inline void InitOccurrArrWithValue(
            Entp_occurr_t* const restrict arrayToInit,
            const Entp_index_t arrayLength,
            const Entp_occurr_t initValue
)
{
    Entp_index_t indexer = 0U;
    for(indexer = 0U; indexer < arrayLength; indexer++)
    {
        arrayToInit[indexer] = initValue;
    }
}

static inline void InitProbabArrWithValue(
            Entp_metrics_t *const restrict arrayToInit,
            const Entp_index_t arrayLength,
            const Entp_metrics_t initValue
)
{
    Entp_index_t indexer = 0U;
    for(indexer = 0U; indexer < arrayLength; indexer++)
    {
        arrayToInit[indexer] = initValue;
    }
}

static Entp_metrics_t * RetAllocSymbolsProbaArr(const Entp_index_t sizeOfArray)
{
    if(sizeOfArray > ARRAY_PROBAB_LENGTH_LIMIT)
    {   /* array length is to big  */
        ERROR_PRINT("given size of array wast TO BIG");
    }

    Entp_metrics_t* const symbolsProbab_a = malloc(sizeof(*symbolsProbab_a) * sizeOfArray);
    if(NULL == symbolsProbab_a)
    { /* malloc failed */
       ERROR_PRINT("malloc FAILED to allocate memory");
    }
    
    const Entp_occurr_t initValue = 0U;
    InitProbabArrWithValue(symbolsProbab_a, sizeOfArray, initValue);
    
    return symbolsProbab_a;
}

static Entp_metricsCollection_s * RetAllocMetricsCollection(void)
{
    Entp_metricsCollection_s  metricsForInit = {
        .symbolsProbability_a = RetAllocSymbolsProbaArr(ALPHABET_SIZE),
        .entropy = (Entp_metrics_t)0.0,
        .omega = (Entp_omega_t)0
    };

    Entp_metricsCollection_s * metrics = malloc(sizeof(metricsForInit));
    if(NULL == metrics)
    { /* malloc failed */
        ERROR_PRINT("malloc FAILED to allocate memory");
    }

    memcpy(metrics, &metricsForInit, sizeof(metricsForInit));

    return metrics;
}


static void PrintMetrics(const Entp_metricsCollection_s *const metrics_s)
{
    printf("Cond. Entropy:  %Le\n", metrics_s->condEntropy);
    printf("Entropy:        %Le\n", metrics_s->entropy);
    printf("Num of symbols: %ld\n", metrics_s->omega);
}

static void CleanMemory(
    Entp_metricsCollection_s* metrics_s,
    Entp_occurr_s* occurrCollection_s
)
{
    if(metrics_s == NULL)
    {
        ERROR_PRINT("argument metrics_s is NULL pointer");
    }

    if(metrics_s->symbolsProbability_a != NULL)
    {
        free(metrics_s->symbolsProbability_a);
        metrics_s->symbolsProbability_a = NULL;
    }

    Entp_index_t index = (Entp_index_t)0U;

    if(metrics_s->condSymbolProbab_aa != NULL)
    {
        index = (Entp_index_t)0U;
        while(index < ALPHABET_SIZE)
        {
            if(metrics_s->condSymbolProbab_aa[index] != NULL)
            {
                free(metrics_s->condSymbolProbab_aa[index]);
                metrics_s->condSymbolProbab_aa[index] = NULL;
            }
            index++;
        }
        free(metrics_s->condSymbolProbab_aa);
        metrics_s->condSymbolProbab_aa = NULL;
    }

    if(occurrCollection_s == NULL)
    {
        ERROR_PRINT("argument occurrCollection_s is NULL pointer");
    }

    if(occurrCollection_s->symbolsOccurrArr_a != NULL)
    {
        free(occurrCollection_s->symbolsOccurrArr_a);
        occurrCollection_s->symbolsOccurrArr_a = NULL;
    }

    if(occurrCollection_s->symbolsCondOccurrArr_aa != NULL)
    {
        index = (Entp_index_t)0U;
        while(index < ALPHABET_SIZE)
        {
            if(occurrCollection_s->symbolsCondOccurrArr_aa[index] != NULL)
            {
                free(occurrCollection_s->symbolsCondOccurrArr_aa[index]);
                occurrCollection_s->symbolsCondOccurrArr_aa[index] = NULL;
            }
            index++;
        }

        free(occurrCollection_s->symbolsCondOccurrArr_aa);
        occurrCollection_s->symbolsCondOccurrArr_aa[index] = NULL;
    }

}

