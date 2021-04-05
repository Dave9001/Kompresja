/**
 *  @Author: Self
 *  
 *  @Brief: File include standard types used in BinS files 
 *          And additional typedefs for array and index of array
 * 
 *  @Usage: It have to be include to all files using BinS functions,
 *          macros, or enums or any other element declared/ defined in BinS.h 
 * 
 *  @Limitations:   Please note that user can give floating point number
 *                  for indexing array, which is undesirable
 *  
 * 
 * @Date: 28.08.2018
 **/

#ifndef _Entp_TYPES_H_
#define _Entp_TYPES_H_

/*******************************************************************************
 *                              INCLUDS
 ******************************************************************************/


#include <stdint.h>
#include <inttypes.h>
#include <float.h>



/*******************************************************************************
 *                              DEFINE
 ******************************************************************************/

#ifndef _FALSE_
#define _FALSE_
    #define FALSE (0U)  /* by exception that 0 is eq. to false */
#endif

#ifndef _TRUE_
#define _TRUE_
    #define TRUE (!FALSE)  /* logical negation of false is true */
#endif

                          

/*******************************************************************************
 *                              TYPEDEF
 ******************************************************************************/

#ifndef _Entp_TYPE_SYMBOL_
#define _Entp_TYPE_SYMBOL_
    typedef uint8_t Entp_symbol_t;   /* type for storing letter/character */
#endif

#ifndef _Entp_TYPE_OCCURRENCE_
#define _Entp_TYPE_OCCURRENCE_
    typedef uint32_t Entp_occurr_t;   /* type for counting occurrence */
#endif

#ifndef _Entp_TYPE_ARRAY_
#define _Entp_TYPE_ARRAY_
    typedef Entp_symbol_t * Entp_letterArrayType_t;    /* type of arrays of letters */
#endif

#ifndef _Entp_TYPE_INDEX_
#define _Entp_TYPE_INDEX_
    typedef uint32_t Entp_index_t;   /* type for indexing */
#endif

#ifndef _Entp_TYPE_ENTROPY_
#define _Entp_TYPE_ENTROPY_
    typedef long double Entp_metrics_t;   /* type for indexing */
#endif

#ifndef _Entp_TYPE_OMEGA_
#define _Entp_TYPE_OMEGA_
    typedef uint64_t Entp_omega_t;   /* type for indexing */
#endif

typedef struct tag_Entp_occurr_s
{
    Entp_occurr_t* symbolsOccurrArr_a;
    Entp_occurr_t**symbolsCondOccurrArr_aa;
}Entp_occurr_s;

/*typedef struct tag_Entp_sort_s
{
    Entp_symbol_t  symbol;
    Entp_metrics_t probability;
}Entp_sort_s;
*/
typedef struct tag_Entp_metricsCollection_s
{
    Entp_metrics_t* symbolsProbability_a;
    Entp_metrics_t  entropy;
    Entp_metrics_t  condEntropy;
    Entp_omega_t    omega;
    Entp_metrics_t** condSymbolProbab_aa;
}Entp_metricsCollection_s;

/*******************************************************************************
 *                              ENUMS
 ******************************************************************************/



#endif /* _Entp_TYPES_H_ */