
#include "IT_Metrics.h"

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




extern  TGA_index IT_LinearUniformQuantization(
    const TGA_index data,
    const TGA_index quantizationStepsize
)
{
    TGA_index quantData = ( (TGA_index)(data / (quantizationStepsize)) * (TGA_index) (quantizationStepsize) 
                            + (TGA_index) (quantizationStepsize) / 2
                        );

    return quantData;
}

extern void IT_Mse(
    TGA_pxMap_aa pxMapIn,
    TGA_RGB_s** pxMapOut,
    TGA_index height,
    TGA_index width,
    TGA_index mode
)
{
    char *name_r = "(r)";
    char *name_g = "(g)";
    char *name_b = "(b)";
    char *name_empty = "";

    char *colorName =  NULL;
    if(1 == mode)
    {
        colorName = name_r;
    }
    else if (2 == mode)
    {
        colorName = name_g;
    }
    else if (3 == mode)
    {
        colorName = name_b;
    }
    else if (4 == mode)
    {
        colorName = name_empty;
    }

    double peak  = 0.0;
    double sigl  = 0.0;
    double noise = 0.0;
    double mse   = 0.0;

    TGA_index r1 = 0U;
    TGA_index r2 = 0U;

    TGA_RGB_s pxIn  = {};
    TGA_RGB_s pxOut = {};

    TGA_index wholePxFlag = 0U;

    TGA_index index_w = 0U;
    TGA_index index_h = height - 1U;
    TGA_index pixelsDataIndex = 0U;
    bool breakLoop = FALSE;
    if( mode < 4U)
    {
        while(index_h >= 0U && ! breakLoop)
        {
            index_w = 0U;
            while(index_w < width)
            {
                pxIn  = pxMapIn[index_h][index_w];
                pxOut = pxMapOut[index_h][index_w];

                if(1U == mode)
                {
                    r1 = pxIn.red;
                    r2 = pxOut.red;
                }
                else if(2U == mode)
                {
                    r1 = pxIn.green;
                    r2 = pxOut.green;
                }
                else if(3U == mode)
                {
                    r1 = pxIn.blue;
                    r2 = pxOut.blue;
                }
            
                index_w++;
            
                sigl += r1 * r2;
                noise += (r1 - r2) * (r1 - r2);
                if (peak < r1) {
                    peak = r1;
                }
            }

            if(0U == index_h)
            {
                breakLoop = TRUE;
            }
            index_h--;
        }
    }
    else
    {
        while(index_h >= 0U && ! breakLoop)
        {
            index_w = 0U;
            while(index_w < width)
            {
                pxIn  = pxMapIn[index_h][index_w];
                pxOut = pxMapOut[index_h][index_w];

                    r1 = pxIn.red;
                    r2 = pxOut.red;
                    sigl += r1 * r2;
                noise += (r1 - r2) * (r1 - r2);
                if (peak < r1) {
                    peak = r1;
                }
                
                    r1 = pxIn.green;
                    r2 = pxOut.green;
                    sigl += r1 * r2;
                noise += (r1 - r2) * (r1 - r2);
                if (peak < r1) {
                    peak = r1;
                }
               
                    r1 = pxIn.blue;
                    r2 = pxOut.blue;
                    sigl += r1 * r2;
                noise += (r1 - r2) * (r1 - r2);
                if (peak < r1) {
                    peak = r1;
                }
               
                index_w++;
            }

            if(0U == index_h)
            {
                breakLoop = TRUE;
            }
            index_h--;
        }
    }

    if(mode < 4U)
    {
        mse = noise / ((double)(width * height) / 3.0);
        printf("mse%s = %lf\n", colorName, mse);
        printf("SNR%s = %lf (%lf)dB\n", colorName, (sigl / noise), 10.0 * log10(sigl / noise));
    }
    else
    {
        mse = noise / (width * height);
        printf("mse%s = %lf\n", colorName, mse);
        printf("SNR%s = %lf (%lf)dB\n", colorName, (sigl / noise), 10.0 * log10(sigl / noise));
    }
}