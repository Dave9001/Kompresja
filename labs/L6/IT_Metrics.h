#ifndef __IT_METRICS_H__
#define __IT_METRICS_H__

#include "IT_Types.h"

/*extern IT_real IT_PxMapMeanSqareError(
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
*/

extern TGA_index IT_LinearUniformQuantization(
    const TGA_index data,
    const TGA_index quantizationStepsize
);
extern void IT_Mse(
    TGA_pxMap_aa pxMapIn,
    TGA_RGB_s** pxMapOut,
    TGA_index height,
    TGA_index width,
    TGA_index mode
);


#endif /*__IT_METRICS_H__*/
