#ifndef __IT_METRICS_H__
#define __IT_METRICS_H__

#include "IT_Types.h"

extern IT_real IT_PxMapMeanSqareError(
    IT_pxMatrix2d originalMatrix,
    IT_pxMatrix2d changedMatrix,
    IT_index height,
    IT_index width
);
extern IT_real IT_ChMapMeanSqareError(
    IT_chMatrix2d originalMatrix,
    IT_chMatrix2d changedMatrix,
    IT_index height,
    IT_index width
);




#endif /*__IT_METRICS_H__*/
