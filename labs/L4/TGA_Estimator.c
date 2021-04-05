#include "TGA_Estimator.h"
#include "IO/IO.h"
#include <math.h>

extern void TGA_Estimator(void);

#define TGA_INC_COORDINATE (1U)
#define TGA_DEC_COORDINATE (0U)
#define TGA_ADD_RGB(sum, rgb1, rgb2) do{\
    sum.red   = rgb1.red   + rgb2.red;\
    sum.green = rgb1.green + rgb2.green;\
    sum.blue  = rgb1.blue  + rgb2.blue;\
}while(FALSE)
#define TGA_SUBTRACT_RGB(sub, rgb1, rgb2) do{\
    sub.red   = rgb1.red   - rgb2.red;\
    sub.green = rgb1.green - rgb2.green;\
    sub.blue  = rgb1.blue  - rgb2.blue;\
}while(FALSE)
#define TGA_DIV_BY_2_RGB(div, rgb1) do{\
    div.red   = rgb1.red   / 2;\
    div.green = rgb1.green / 2;\
    div.blue  = rgb1.blue  / 2;\
}while(FALSE)

typedef struct tag_TGA_NeighberPosition
{
    bool north;
    bool west;
    bool east;
    bool south;
}TGA_NeighberPosition;

typedef struct tag_TGA_Position
{
    TGA_index x;
    TGA_index y;
    TGA_index x_max;
    TGA_index y_max;
}TGA_Position;

typedef enum tag_TGA_coordinate_e
{
    TGA_north,
    TGA_west,
    TGA_south,
    TGA_east,
}TGA_coordinate_e;


static void TGA_RewritePixelMapTo2DArray(
    TGA_header_cs *const header_cpcs,
    TGA_imgData_cs *const imgData_cpcs,
    TGA_RGB_s * * *const pixelMap
);
static bool TGA_Coordinate(
    TGA_coordinate_e direction,
    TGA_index limit,
    TGA_index start,
    TGA_index *const neightCoordinate,
    TGA_RGB_s *const retRGB
);
static TGA_RGB_s TGA_Neighber(
    const TGA_Position originPosition,
    const TGA_NeighberPosition neighberPos,
    const TGA_RGB_s ***const pixelMap
);
static TGA_RGB_s TGA_Predicator_0(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
);
static TGA_RGB_s TGA_Predicator_1(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
);
static TGA_RGB_s TGA_Predicator_1(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
);
static TGA_RGB_s TGA_Predicator_2(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
);
static TGA_RGB_s TGA_Predicator_3(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
);
static TGA_RGB_s TGA_Predicator_4(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
);
static TGA_RGB_s TGA_Predicator_5(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
);
static TGA_RGB_s TGA_Predicator_6(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
);
static TGA_RGB_s TGA_Predicator_7(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
);
static TGA_RGB_s TGA_Predicator_new(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
);
static TGA_index TGA_MaxRgb(TGA_index rgb1, TGA_index rgb2);
static TGA_index TGA_MinRgb(TGA_index rgb1, TGA_index rgb2);

static void TGA_CreatePixelMapWithPredicator(
    TGA_RGB_s(*TGA_Predicator)(TGA_Position, const TGA_RGB_s ***const),
    const TGA_RGB_s ***const originPixelMap,
    TGA_RGB_s ***const pixelMap_pred,
    TGA_index pmWidth, 
    TGA_index pmHeight
);
static double TGA_CalcEntPM(
    const TGA_RGB_s***const pixelMap,
    TGA_index height,
    TGA_index width
);
static double TGA_CalcEntPM_red(
    const TGA_RGB_s***const pixelMap,
    TGA_index height,
    TGA_index width
);
static double TGA_CalcEntPM_green(
    const TGA_RGB_s***const pixelMap,
    TGA_index height,
    TGA_index width
);
static double TGA_CalcEntPM_blue(
    const TGA_RGB_s***const pixelMap,
    TGA_index height,
    TGA_index width
);


int main(void)
{
    TGA_Estimator();

    return 0;
}

extern void TGA_Estimator(void)
{
    IO_buffor_s buffor_s     = IO_InitBuffor(NULL, NULL, NULL, NULL);

    TGA_header_cs header     = TGA_ReadHeader(&buffor_s);
    TGA_imgData_cs imgData_s = TGA_ReadImgData(&header, &buffor_s);

    TGA_RGB_s *const *const originPixelMap = TGA_PixelMap(&header, &imgData_s);

    TGA_RGB_s *const *const pixelMap_pred_0   = TGA_PixelMap(&header, &imgData_s);
    TGA_RGB_s *const *const pixelMap_pred_1   = TGA_PixelMap(&header, &imgData_s);
    TGA_RGB_s *const *const pixelMap_pred_2   = TGA_PixelMap(&header, &imgData_s);
    TGA_RGB_s *const *const pixelMap_pred_3   = TGA_PixelMap(&header, &imgData_s);
    TGA_RGB_s *const *const pixelMap_pred_4   = TGA_PixelMap(&header, &imgData_s);
    TGA_RGB_s *const *const pixelMap_pred_5   = TGA_PixelMap(&header, &imgData_s);
    TGA_RGB_s *const *const pixelMap_pred_6   = TGA_PixelMap(&header, &imgData_s);
    TGA_RGB_s *const *const pixelMap_pred_7   = TGA_PixelMap(&header, &imgData_s);
    TGA_RGB_s *const *const pixelMap_pred_new = TGA_PixelMap(&header, &imgData_s);

    TGA_halfWordCaster_u caster;
    caster.twoByteArray[0] = header.imgSpec_s.width_a[0];
    caster.twoByteArray[1] = header.imgSpec_s.width_a[1];
    TGA_halfWord width     = caster.halfWord;

    caster.twoByteArray[0] = header.imgSpec_s.height_a[0];
    caster.twoByteArray[1] = header.imgSpec_s.height_a[1];
    TGA_halfWord height    = caster.halfWord;

    /*0 - none*/
    TGA_CreatePixelMapWithPredicator(
        TGA_Predicator_0, 
        (const TGA_RGB_s***const)&originPixelMap,
        (TGA_RGB_s***const)&pixelMap_pred_0,
        width, 
        height
    );
    /*1*/
    TGA_CreatePixelMapWithPredicator(
        TGA_Predicator_1, 
        (const TGA_RGB_s***const)&originPixelMap,
        (TGA_RGB_s***const)&pixelMap_pred_1,
        width, 
        height
    );
    /*2*/
    TGA_CreatePixelMapWithPredicator(
        TGA_Predicator_2, 
        (const TGA_RGB_s***const)&originPixelMap,
        (TGA_RGB_s***const)&pixelMap_pred_2,
        width, 
        height
    );
    /*3*/
    TGA_CreatePixelMapWithPredicator(
        TGA_Predicator_3, 
        (const TGA_RGB_s***const)&originPixelMap,
        (TGA_RGB_s***const)&pixelMap_pred_3,
        width, 
        height
    );
    /*4*/
    TGA_CreatePixelMapWithPredicator(
        TGA_Predicator_4, 
        (const TGA_RGB_s***const)&originPixelMap,
        (TGA_RGB_s***const)&pixelMap_pred_4,
        width, 
        height
    );
    /*5*/
    TGA_CreatePixelMapWithPredicator(
        TGA_Predicator_5, 
        (const TGA_RGB_s***const)&originPixelMap,
        (TGA_RGB_s***const)&pixelMap_pred_5,
        width, 
        height
    );
    /*6*/
    TGA_CreatePixelMapWithPredicator(
        TGA_Predicator_6, 
        (const TGA_RGB_s***const)&originPixelMap,
        (TGA_RGB_s***const)&pixelMap_pred_6,
        width, 
        height
    );
    /*7*/
    TGA_CreatePixelMapWithPredicator(
        TGA_Predicator_7, 
        (const TGA_RGB_s***const)&originPixelMap,
        (TGA_RGB_s***const)&pixelMap_pred_7,
        width, 
        height
    );
    /*new*/
    TGA_CreatePixelMapWithPredicator(
        TGA_Predicator_new, 
        (const TGA_RGB_s***const)&originPixelMap,
        (TGA_RGB_s***const)&pixelMap_pred_new,
        width, 
        height
    );

    /*calc entrophy for pic*/
    //printf("r: %x  g: %x  b: %x\n", pixelMap_pred_0[0][0].red, pixelMap_pred_0[0][0].green, pixelMap_pred_0[0][0].blue);
    double entropy_pred_0 = TGA_CalcEntPM(
        (const TGA_RGB_s***const)&pixelMap_pred_0,
        height,
        width
    );
    double entropy_pred_1 = TGA_CalcEntPM(
        (const TGA_RGB_s***const)&pixelMap_pred_1,
        height,
        width
    );
    double entropy_pred_2 = TGA_CalcEntPM(
        (const TGA_RGB_s***const)&pixelMap_pred_2,
        height,
        width
    );
    double entropy_pred_3 = TGA_CalcEntPM(
        (const TGA_RGB_s***const)&pixelMap_pred_3,
        height,
        width
    );
    double entropy_pred_4 = TGA_CalcEntPM(
        (const TGA_RGB_s***const)&pixelMap_pred_4,
        height,
        width
    );
    double entropy_pred_5 = TGA_CalcEntPM(
        (const TGA_RGB_s***const)&pixelMap_pred_5,
        height,
        width
    );
    double entropy_pred_6 = TGA_CalcEntPM(
        (const TGA_RGB_s***const)&pixelMap_pred_6,
        height,
        width
    );
    double entropy_pred_7 = TGA_CalcEntPM(
        (const TGA_RGB_s***const)&pixelMap_pred_7,
        height,
        width
    );
    double entropy_pred_new = TGA_CalcEntPM(
        (const TGA_RGB_s***const)&pixelMap_pred_new,
        height,
        width
    );

    printf("Whole pixel map\n");
    printf("Predicat\tEntropy\n");
    printf("none\t\t%lf\n\n", entropy_pred_0);
    printf("1\t\t%lf\n",    entropy_pred_1);
    printf("2\t\t%lf\n\n",    entropy_pred_2);
    printf("3\t\t%lf\n",    entropy_pred_3);
    printf("4\t\t%lf\n\n",    entropy_pred_4);
    printf("5\t\t%lf\n",    entropy_pred_5);
    printf("6\t\t%lf\n\n",    entropy_pred_6);
    printf("7\t\t%lf\n",    entropy_pred_7);
    printf("new\t\t%lf\n\n",  entropy_pred_new);

    double pmEnt[] = {
        entropy_pred_0,
        entropy_pred_1,
        entropy_pred_2,
        entropy_pred_3,
        entropy_pred_4,
        entropy_pred_5,
        entropy_pred_6,
        entropy_pred_7,
        entropy_pred_new
    };

    entropy_pred_0 = TGA_CalcEntPM_red(
        (const TGA_RGB_s***const)&pixelMap_pred_0,
        height,
        width
    );
    entropy_pred_1 = TGA_CalcEntPM_red(
        (const TGA_RGB_s***const)&pixelMap_pred_1,
        height,
        width
    );
    entropy_pred_2 = TGA_CalcEntPM_red(
        (const TGA_RGB_s***const)&pixelMap_pred_2,
        height,
        width
    );
    entropy_pred_3 = TGA_CalcEntPM_red(
        (const TGA_RGB_s***const)&pixelMap_pred_3,
        height,
        width
    );
    entropy_pred_4 = TGA_CalcEntPM_red(
        (const TGA_RGB_s***const)&pixelMap_pred_4,
        height,
        width
    );
    entropy_pred_5 = TGA_CalcEntPM_red(
        (const TGA_RGB_s***const)&pixelMap_pred_5,
        height,
        width
    );
    entropy_pred_6 = TGA_CalcEntPM_red(
        (const TGA_RGB_s***const)&pixelMap_pred_6,
        height,
        width
    );
    entropy_pred_7 = TGA_CalcEntPM_red(
        (const TGA_RGB_s***const)&pixelMap_pred_7,
        height,
        width
    );
    entropy_pred_new = TGA_CalcEntPM_red(
        (const TGA_RGB_s***const)&pixelMap_pred_new,
        height,
        width
    );

    printf("Red colour\n");
    printf("Predicat\tEntropy\n");
    printf("none\t\t%lf\n\n", entropy_pred_0);
    printf("1\t\t%lf\n",    entropy_pred_1);
    printf("2\t\t%lf\n\n",    entropy_pred_2);
    printf("3\t\t%lf\n",    entropy_pred_3);
    printf("4\t\t%lf\n\n",    entropy_pred_4);
    printf("5\t\t%lf\n",    entropy_pred_5);
    printf("6\t\t%lf\n\n",    entropy_pred_6);
    printf("7\t\t%lf\n",    entropy_pred_7);
    printf("new\t\t%lf\n",  entropy_pred_new);

    double redEnt[] = {
        entropy_pred_0,
        entropy_pred_1,
        entropy_pred_2,
        entropy_pred_3,
        entropy_pred_4,
        entropy_pred_5,
        entropy_pred_6,
        entropy_pred_7,
        entropy_pred_new
    };

    entropy_pred_0 = TGA_CalcEntPM_green(
        (const TGA_RGB_s***const)&pixelMap_pred_0,
        height,
        width
    );
    entropy_pred_1 = TGA_CalcEntPM_green(
        (const TGA_RGB_s***const)&pixelMap_pred_1,
        height,
        width
    );
    entropy_pred_2 = TGA_CalcEntPM_green(
        (const TGA_RGB_s***const)&pixelMap_pred_2,
        height,
        width
    );
    entropy_pred_3 = TGA_CalcEntPM_green(
        (const TGA_RGB_s***const)&pixelMap_pred_3,
        height,
        width
    );
    entropy_pred_4 = TGA_CalcEntPM_green(
        (const TGA_RGB_s***const)&pixelMap_pred_4,
        height,
        width
    );
    entropy_pred_5 = TGA_CalcEntPM_green(
        (const TGA_RGB_s***const)&pixelMap_pred_5,
        height,
        width
    );
    entropy_pred_6 = TGA_CalcEntPM_green(
        (const TGA_RGB_s***const)&pixelMap_pred_6,
        height,
        width
    );
    entropy_pred_7 = TGA_CalcEntPM_green(
        (const TGA_RGB_s***const)&pixelMap_pred_7,
        height,
        width
    );
    entropy_pred_new = TGA_CalcEntPM_green(
        (const TGA_RGB_s***const)&pixelMap_pred_new,
        height,
        width
    );

    printf("Green colour\n");
    printf("Predicat\tEntropy\n");
    printf("none\t\t%lf\n\n", entropy_pred_0);
    printf("1\t\t%lf\n",    entropy_pred_1);
    printf("2\t\t%lf\n\n",    entropy_pred_2);
    printf("3\t\t%lf\n",    entropy_pred_3);
    printf("4\t\t%lf\n\n",    entropy_pred_4);
    printf("5\t\t%lf\n",    entropy_pred_5);
    printf("6\t\t%lf\n\n",    entropy_pred_6);
    printf("7\t\t%lf\n",    entropy_pred_7);
    printf("new\t\t%lf\n",  entropy_pred_new);

    double greenEnt[] = {
        entropy_pred_0,
        entropy_pred_1,
        entropy_pred_2,
        entropy_pred_3,
        entropy_pred_4,
        entropy_pred_5,
        entropy_pred_6,
        entropy_pred_7,
        entropy_pred_new
    };

    entropy_pred_0 = TGA_CalcEntPM_blue(
        (const TGA_RGB_s***const)&pixelMap_pred_0,
        height,
        width
    );
    entropy_pred_1 = TGA_CalcEntPM_blue(
        (const TGA_RGB_s***const)&pixelMap_pred_1,
        height,
        width
    );
    entropy_pred_2 = TGA_CalcEntPM_blue(
        (const TGA_RGB_s***const)&pixelMap_pred_2,
        height,
        width
    );
    entropy_pred_3 = TGA_CalcEntPM_blue(
        (const TGA_RGB_s***const)&pixelMap_pred_3,
        height,
        width
    );
    entropy_pred_4 = TGA_CalcEntPM_blue(
        (const TGA_RGB_s***const)&pixelMap_pred_4,
        height,
        width
    );
    entropy_pred_5 = TGA_CalcEntPM_blue(
        (const TGA_RGB_s***const)&pixelMap_pred_5,
        height,
        width
    );
    entropy_pred_6 = TGA_CalcEntPM_blue(
        (const TGA_RGB_s***const)&pixelMap_pred_6,
        height,
        width
    );
    entropy_pred_7 = TGA_CalcEntPM_blue(
        (const TGA_RGB_s***const)&pixelMap_pred_7,
        height,
        width
    );
    entropy_pred_new = TGA_CalcEntPM_blue(
        (const TGA_RGB_s***const)&pixelMap_pred_new,
        height,
        width
    );

    printf("Blue colour\n");
    printf("Predicat\tEntropy\n");
    printf("none\t\t%lf\n\n", entropy_pred_0);
    printf("1\t\t%lf\n",    entropy_pred_1);
    printf("2\t\t%lf\n\n",    entropy_pred_2);
    printf("3\t\t%lf\n",    entropy_pred_3);
    printf("4\t\t%lf\n\n",    entropy_pred_4);
    printf("5\t\t%lf\n",    entropy_pred_5);
    printf("6\t\t%lf\n\n",    entropy_pred_6);
    printf("7\t\t%lf\n",    entropy_pred_7);
    printf("new\t\t%lf\n",  entropy_pred_new);

    double blueEnt[] = {
        entropy_pred_0,
        entropy_pred_1,
        entropy_pred_2,
        entropy_pred_3,
        entropy_pred_4,
        entropy_pred_5,
        entropy_pred_6,
        entropy_pred_7,
        entropy_pred_new
    };

    /*TGA_index x = 0U, y = 0U;
    while(y < height)
    {
        x = 0U;
        while(x < width)
        {
            printf("%x%x%x ",pixelMap_pred_3[y][x].red,pixelMap_pred_3[y][x].green,pixelMap_pred_3[y][x].blue);
            x++;
        }
        puts("");
        y++;
    }*/
    double min = pmEnt[0];
    TGA_index pmEntMinIndex = 0U;
    TGA_index index = 0U;
    while(index < 9U)
    {
        if(min > pmEnt[index])
        {
            pmEntMinIndex = index;
            min = pmEnt[index];

        }
        index++;
    }

    min = redEnt[0];
    TGA_index redEntMinIndex = 0U;
    index = 0U;
    while(index < 9U)
    {
        if(min > redEnt[index])
        {
            redEntMinIndex = index;
            min = redEnt[index];
        }
        index++;
    }

    min = greenEnt[0];
    TGA_index greenEntMinIndex = 0U;
    index = 0U;
    while(index < 9U)
    {
        if(min > greenEnt[index])
        {
            greenEntMinIndex = index;
            min = greenEnt[index];
        }
        index++;
    }

    min = blueEnt[0];
    TGA_index blueEntMinIndex = 0U;
    index = 0U;
    while(index < 9U)
    {
        if(min > blueEnt[index])
        {
            blueEntMinIndex = index;
            min = blueEnt[index];
        }
        index++;
    }


    printf("\n\tbest predicator\n");
    printf("Whole pixel map: ");
    switch(pmEntMinIndex)
    {
        case 0: printf("none\n"); break;
        case 1: printf("1\n");    break;
        case 2: printf("2\n");    break;
        case 3: printf("3\n");    break;
        case 4: printf("4\n");    break;
        case 5: printf("5\n");    break;
        case 6: printf("6\n");    break;
        case 7: printf("7\n");    break;
        case 8: printf("new\n");  break;

        default: puts("blad\n"); break;
    }

    printf("Red:             ");
    switch(redEntMinIndex)
    {
        case 0: printf("none\n"); break;
        case 1: printf("1\n");    break;
        case 2: printf("2\n");    break;
        case 3: printf("3\n");    break;
        case 4: printf("4\n");    break;
        case 5: printf("5\n");    break;
        case 6: printf("6\n");    break;
        case 7: printf("7\n");    break;
        case 8: printf("new\n");  break;

        default: puts("blad\n"); break;
    }

    printf("Green:           ");
    switch(greenEntMinIndex)
    {
        case 0: printf("none\n"); break;
        case 1: printf("1\n");    break;
        case 2: printf("2\n");    break;
        case 3: printf("3\n");    break;
        case 4: printf("4\n");    break;
        case 5: printf("5\n");    break;
        case 6: printf("6\n");    break;
        case 7: printf("7\n");    break;
        case 8: printf("new\n");  break;

        default: puts("blad\n"); break;
    }

    printf("Blue:            ");
    switch(blueEntMinIndex)
    {
        case 0: printf("none\n"); break;
        case 1: printf("1\n");    break;
        case 2: printf("2\n");    break;
        case 3: printf("3\n");    break;
        case 4: printf("4\n");    break;
        case 5: printf("5\n");    break;
        case 6: printf("6\n");    break;
        case 7: printf("7\n");    break;
        case 8: printf("new\n");  break;

        default: puts("blad\n"); break;
    }

}

static double TGA_CalcEntPM(
    const TGA_RGB_s***const pixelMap,
    TGA_index height,
    TGA_index width
)
{
    const TGA_index numOfDiffPossColors = 1U << (sizeof(TGA_byte) * 8U);
    TGA_index *occurrArray = malloc(sizeof(*occurrArray) * numOfDiffPossColors);
    if(NULL == occurrArray)
    {
        ERROR_PRINT("malloc failed");
    }

    TGA_index index = 0U;
    while(index < numOfDiffPossColors)
    {
        occurrArray[index] = 0U;
        index++;
    }

    TGA_index x = 0U;
    TGA_index y = 0U;
    TGA_RGB_s pixel = {};
    while(y < height)
    {
        x = 0U;
        while(x < width)
        {
            pixel = pixelMap[0][y][x];

            occurrArray[pixel.red]   += 1U;
            occurrArray[pixel.green] += 1U;
            occurrArray[pixel.blue]  += 1U;
            x++;
        }
        y++;
    }

    double prob = 0.0;
    double entropy = 0.0;
    double numOfPx = width * height;
    index = 0U;
    while(index < numOfDiffPossColors)
    {
        prob = (double)occurrArray[index] / (numOfPx * 3U);
        if(prob != 0.0)
        {
            entropy += prob * log2(prob);
        }
        index++;
    }

    if(entropy < 0.0)
    {
        entropy *= -1.0;
    }

    free(occurrArray);
    return entropy;
}



static double TGA_CalcEntPM_red(
    const TGA_RGB_s***const pixelMap,
    TGA_index height,
    TGA_index width
)
{
    const TGA_index numOfDiffPossColors = 1U << (sizeof(TGA_byte) * 8U);
    TGA_index *occurrArray = malloc(sizeof(*occurrArray) * numOfDiffPossColors);
    if(NULL == occurrArray)
    {
        ERROR_PRINT("malloc failed");
    }

    TGA_index index = 0U;
    while(index < numOfDiffPossColors)
    {
        occurrArray[index] = 0U;
        index++;
    }

    TGA_index x = 0U;
    TGA_index y = 0U;
    TGA_RGB_s pixel = {};
    while(y < height)
    {
        x = 0U;
        while(x < width)
        {
            pixel = pixelMap[0][y][x];

            occurrArray[pixel.red] += 1U;
            x++;
        }
        y++;
    }

    double prob = 0.0;
    double entropy = 0.0;
    double numOfPx = width * height;
    index = 0U;
    while(index < numOfDiffPossColors)
    {
        prob = (double)occurrArray[index] / (numOfPx);
        if(prob != 0.0)
        {
            entropy += prob * log2(prob);
        }
        index++;
    }

    if(entropy < 0.0)
    {
        entropy *= -1.0;
    }

    free(occurrArray);
    return entropy;
}

static double TGA_CalcEntPM_green(
    const TGA_RGB_s***const pixelMap,
    TGA_index height,
    TGA_index width
)
{
    const TGA_index numOfDiffPossColors = 1U << (sizeof(TGA_byte) * 8U);
    TGA_index *occurrArray = malloc(sizeof(*occurrArray) * numOfDiffPossColors);
    if(NULL == occurrArray)
    {
        ERROR_PRINT("malloc failed");
    }

    TGA_index index = 0U;
    while(index < numOfDiffPossColors)
    {
        occurrArray[index] = 0U;
        index++;
    }

    TGA_index x = 0U;
    TGA_index y = 0U;
    TGA_RGB_s pixel = {};
    while(y < height)
    {
        x = 0U;
        while(x < width)
        {
            pixel = pixelMap[0][y][x];

            occurrArray[pixel.green] += 1U;
            x++;
        }
        y++;
    }

    double prob = 0.0;
    double entropy = 0.0;
    double numOfPx = width * height;
    index = 0U;
    while(index < numOfDiffPossColors)
    {
        prob = (double)occurrArray[index] / (numOfPx);
        if(prob != 0.0)
        {
            entropy += prob * log2(prob);
        }
        index++;
    }

    if(entropy < 0.0)
    {
        entropy *= -1.0;
    }

    free(occurrArray);
    return entropy;
}

static double TGA_CalcEntPM_blue(
    const TGA_RGB_s***const pixelMap,
    TGA_index height,
    TGA_index width
)
{
    const TGA_index numOfDiffPossColors = 1U << (sizeof(TGA_byte) * 8U);
    TGA_index *occurrArray = malloc(sizeof(*occurrArray) * numOfDiffPossColors);
    if(NULL == occurrArray)
    {
        ERROR_PRINT("malloc failed");
    }

    TGA_index index = 0U;
    while(index < numOfDiffPossColors)
    {
        occurrArray[index] = 0U;
        index++;
    }

    TGA_index x = 0U;
    TGA_index y = 0U;
    TGA_RGB_s pixel = {};
    while(y < height)
    {
        x = 0U;
        while(x < width)
        {
            pixel = pixelMap[0][y][x];

            occurrArray[pixel.blue] += 1U;
            x++;
        }
        y++;
    }

    double prob = 0.0;
    double entropy = 0.0;
    double numOfPx = width * height;
    index = 0U;
    while(index < numOfDiffPossColors)
    {
        prob = (double)occurrArray[index] / (numOfPx);
        if(prob != 0.0)
        {
            entropy += prob * log2(prob);
        }
        index++;
    }

    if(entropy < 0.0)
    {
        entropy *= -1.0;
    }

    free(occurrArray);
    return entropy;
}



static void TGA_CreatePixelMapWithPredicator(
    TGA_RGB_s(*TGA_Predicator)(TGA_Position, const TGA_RGB_s ***const),
    const TGA_RGB_s ***const originPixelMap,
    TGA_RGB_s ***const pixelMap_pred,
    TGA_index pmWidth, 
    TGA_index pmHeight
)
{
    TGA_Position origPosition = {  
        .x = 0U,
        .y = 0U,
        .x_max = pmWidth,
        .y_max = pmHeight
    };

    TGA_index x = 0U;
    TGA_index y = 0U;
    TGA_index x_len = pmWidth;
    TGA_index y_len = pmHeight;

    origPosition.x = 0U;
    origPosition.y = 0U;

    while(y < y_len)
    {
        x = 0U;
        origPosition.y = y;
        while(x < x_len)
        {
            origPosition.x = x;
            pixelMap_pred[0][y][x] = TGA_Predicator(origPosition, originPixelMap);
            x++;
        }
        y++;
    }
}

static TGA_RGB_s TGA_Neighber(
    const TGA_Position originPosition,
    const TGA_NeighberPosition neighberPos,
    const TGA_RGB_s ***const pixelMap
)
{
    TGA_index x_origin = originPosition.x;
    TGA_index y_origin = originPosition.y;

    TGA_index x_max    = originPosition.x_max;
    TGA_index y_max    = originPosition.y_max;

    bool north = neighberPos.north;
    bool west  = neighberPos.west;
    bool east  = neighberPos.east;
    bool south = neighberPos.south;

    TGA_RGB_s retRGB   = {};
    TGA_index x_neigh  = x_origin;
    TGA_index y_neigh  = y_origin;

    /*in case to not to move: if u want neighber in S & N the you are the neighber
      in the matter of that plane */
    if(south && north)
    {
        north = FALSE;
        south = FALSE;
    }
     /*in case to not to move: if u want neighber in E & W the you are the neighber
       in the matter of that plane */
    if(east && west)
    {
        west = FALSE;
        east = FALSE;
    }
   
    bool notFrame = TRUE;
    if(north)
    {
        if(TGA_Coordinate(TGA_north, 0U, y_origin, &y_neigh, &retRGB))
        {
            retRGB = pixelMap[0][y_neigh][x_neigh];
        }
        else
        {
            notFrame = FALSE;
        }
    }

    if(west && notFrame)
    {
        if(TGA_Coordinate(TGA_west, 0U, x_origin, &x_neigh, &retRGB))
        {
            retRGB = pixelMap[0][y_neigh][x_neigh];
        }
        else
        {
            notFrame = FALSE;
        }
    }

    if(east && notFrame)
    {
        if(TGA_Coordinate(TGA_east, x_max, x_origin, &x_neigh, &retRGB))
        {
            retRGB = pixelMap[0][y_neigh][x_neigh];
        }
        else
        {
            notFrame = FALSE;
        }
    }

    if(south && notFrame)
    {
        if(TGA_Coordinate(TGA_south, y_max, y_origin, &y_neigh, &retRGB))
        {
            retRGB = pixelMap[0][y_neigh][x_neigh];
        }
        else
        {
            notFrame = FALSE;
        }
    }

    return retRGB;
}

static bool TGA_Coordinate(
    TGA_coordinate_e direction,
    TGA_index limit,
    TGA_index start,
    TGA_index *const neightCoordinate,
    TGA_RGB_s *const retRGB
)
{
    bool notFrame = TRUE;
    if(limit == *neightCoordinate)
    {
        retRGB->red   = 0U;
        retRGB->green = 0U;
        retRGB->blue  = 0U;
        notFrame      =  FALSE;
    }
    else
    {
        if(TGA_south == direction || TGA_east == direction)
        {
            if(*neightCoordinate != limit)
            {
                *neightCoordinate += 1U;
            }
            else
            {
                retRGB->red   = 0U;
                retRGB->green = 0U;
                retRGB->blue  = 0U;
                notFrame      = FALSE;
            }   
        }
        else
        {
            if(*neightCoordinate != limit)
            {
                *neightCoordinate -= 1U;
            }
            else
            {
                retRGB->red   = 0U;
                retRGB->green = 0U;
                retRGB->blue  = 0U;
                notFrame      = FALSE;
            }   
        }
    }
    return notFrame;
}

static TGA_RGB_s TGA_Predicator_0(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
)
{
    TGA_RGB_s retRGB = pixelMap[0][originPosition.y][originPosition.x];
     
    return retRGB;
}

/***************************************************
 * Kim child  north
 *    west      x
 *
 ***************************************************/
static TGA_RGB_s TGA_Predicator_1(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
)
{
    TGA_NeighberPosition neighberPos = {
        .west  = TRUE,
        .north = FALSE,
        .east  = FALSE,
        .south = FALSE
    };

    TGA_RGB_s retRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    return retRGB;
}
/***************************************************
 * north-west  north
 *    west      x
 *
 ***************************************************/
static TGA_RGB_s TGA_Predicator_2(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
)
{
    TGA_NeighberPosition neighberPos = {
        .west  = FALSE,
        .north = TRUE,
        .east  = FALSE,
        .south = FALSE
    };

    TGA_RGB_s retRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    return retRGB;
}

/***************************************************
 * north-west  north
 *    west      x
 *
 ***************************************************/
static TGA_RGB_s TGA_Predicator_3(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
)
{
    TGA_NeighberPosition neighberPos = {
        .west  = TRUE,
        .north = TRUE,
        .east  = FALSE,
        .south = FALSE
    };

    TGA_RGB_s retRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    return retRGB;
}


/***************************************************
 * north-west  north
 *    west      x
 *
 ***************************************************/
static TGA_RGB_s TGA_Predicator_4(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
)
{
    TGA_NeighberPosition neighberPos = {
        .west  = FALSE,
        .north = TRUE,
        .east  = FALSE,
        .south = FALSE
    };
    TGA_RGB_s northRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);

    //neighberPos = {
        neighberPos.west  = TRUE;
        neighberPos.north = FALSE;
        neighberPos.east  = FALSE;
        neighberPos.south = FALSE;
    //};
    TGA_RGB_s westRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    //neighberPos = {
        neighberPos.west  = TRUE;
        neighberPos.north = TRUE;
        neighberPos.east  = FALSE;
        neighberPos.south = FALSE;
    //};
    TGA_RGB_s northWestRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    TGA_RGB_s retRGB = {};
    TGA_ADD_RGB(retRGB, northRGB, westRGB);
    TGA_SUBTRACT_RGB(retRGB, retRGB, northWestRGB);

    return retRGB;
}

static TGA_RGB_s TGA_Predicator_5(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
)
{
    TGA_NeighberPosition neighberPos = {
        .west  = FALSE,
        .north = TRUE,
        .east  = FALSE,
        .south = FALSE
    };
    TGA_RGB_s northRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);

    //neighberPos = {
        neighberPos.west  = TRUE;
        neighberPos.north = FALSE;
        neighberPos.east  = FALSE;
        neighberPos.south = FALSE;
    //};
    TGA_RGB_s westRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    //neighberPos = {
        neighberPos.west  = TRUE;
        neighberPos.north = TRUE;
        neighberPos.east  = FALSE;
        neighberPos.south = FALSE;
    //};
    TGA_RGB_s northWestRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    TGA_RGB_s retRGB = {};
    TGA_SUBTRACT_RGB(retRGB, westRGB, northWestRGB);
    TGA_DIV_BY_2_RGB(retRGB, retRGB);
    TGA_ADD_RGB(retRGB, retRGB, northRGB);

    return retRGB;
}

static TGA_RGB_s TGA_Predicator_6(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
)
{
    TGA_NeighberPosition neighberPos = {
        .west  = FALSE,
        .north = TRUE,
        .east  = FALSE,
        .south = FALSE
    };
    TGA_RGB_s northRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);

    //neighberPos = {
        neighberPos.west  = TRUE;
        neighberPos.north = FALSE;
        neighberPos.east  = FALSE;
        neighberPos.south = FALSE;
   // };
    TGA_RGB_s westRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    //neighberPos = {
        neighberPos.west  = TRUE;
        neighberPos.north = TRUE;
        neighberPos.east  = FALSE;
        neighberPos.south = FALSE;
    //};
    TGA_RGB_s northWestRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    TGA_RGB_s retRGB = {};
    TGA_SUBTRACT_RGB(retRGB, northRGB, northWestRGB);
    TGA_DIV_BY_2_RGB(retRGB, retRGB);
    TGA_ADD_RGB(retRGB, retRGB, westRGB);

    return retRGB;
}

static TGA_RGB_s TGA_Predicator_7(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
)
{
    TGA_NeighberPosition neighberPos = {
        .west  = FALSE,
        .north = TRUE,
        .east  = FALSE,
        .south = FALSE
    };
    TGA_RGB_s northRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);

    //neighberPos = {
        neighberPos.west  = TRUE;
        neighberPos.north = FALSE;
        neighberPos.east  = FALSE;
        neighberPos.south = FALSE;
    //};
    TGA_RGB_s westRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    TGA_RGB_s retRGB = {};
    
    TGA_ADD_RGB(retRGB, northRGB, westRGB);
    TGA_DIV_BY_2_RGB(retRGB, retRGB);

    return retRGB;
}

static TGA_RGB_s TGA_Predicator_new(
    TGA_Position originPosition,
    const TGA_RGB_s ***const pixelMap
)
{
    TGA_NeighberPosition neighberPos = {
        .west  = FALSE,
        .north = TRUE,
        .east  = FALSE,
        .south = FALSE
    };
    TGA_RGB_s northRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);

    //neighberPos = {
        neighberPos.west  = TRUE;
        neighberPos.north = FALSE;
        neighberPos.east  = FALSE;
        neighberPos.south = FALSE;
    //};
    TGA_RGB_s westRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
     
    //neighberPos = {
        neighberPos.west  = TRUE;
        neighberPos.north = TRUE;
        neighberPos.east  = FALSE;
        neighberPos.south = FALSE;
    //};
    TGA_RGB_s northWestRGB = TGA_Neighber(originPosition, neighberPos, pixelMap);
    
    TGA_RGB_s retRGB = {};
    TGA_index westSumOfColors      = westRGB.red  + westRGB.green  + westRGB.blue;
    TGA_index northSumOfColors     = northRGB.red + northRGB.green + northRGB.blue;
    TGA_index northWestSumOfColors = northWestRGB.red + 
                                     northWestRGB.green + 
                                     northWestRGB.blue;
    if(northWestSumOfColors >= TGA_MaxRgb(westSumOfColors, northSumOfColors))
    {
        if(westSumOfColors > northSumOfColors)
        {
            retRGB = westRGB;
        }
        else
        {
            retRGB = northRGB;
        }
    }
    else if(northWestSumOfColors <= TGA_MaxRgb(westSumOfColors, northSumOfColors))
    {
        if(westSumOfColors < northSumOfColors)
        {
            retRGB = westRGB;
        }
        else
        {
            retRGB = northRGB;
        }
    }
    else
    {
        TGA_ADD_RGB(retRGB, northRGB, westRGB);
        TGA_SUBTRACT_RGB(retRGB, retRGB, northWestRGB);        
    }

    return retRGB;
}

static TGA_index TGA_MaxRgb(TGA_index rgb1, TGA_index rgb2)
{
    TGA_index ret = rgb2;
    if(rgb1 > rgb2)
    {
        ret = rgb1;
    }
    return ret;
}

static TGA_index TGA_MinRgb(TGA_index rgb1, TGA_index rgb2)
{
    TGA_index ret = rgb2;
    if(rgb1 < rgb2)
    {
        ret = rgb1;
    }
    return ret;
}


static void TGA_RewritePixelMapTo2DArray(
    TGA_header_cs *const header_cpcs,
    TGA_imgData_cs *const imgData_cpcs,
    TGA_RGB_s ***const pixelMap
)
{
    TGA_halfWordCaster_u caster;
    caster.twoByteArray[0] = header_cpcs->imgSpec_s.width_a[0];
    caster.twoByteArray[1] = header_cpcs->imgSpec_s.width_a[1];
    TGA_halfWord width     = caster.halfWord;

    caster.twoByteArray[0] = header_cpcs->imgSpec_s.height_a[0];
    caster.twoByteArray[1] = header_cpcs->imgSpec_s.height_a[1];
    TGA_halfWord height    = caster.halfWord;

    caster.twoByteArray[0] = header_cpcs->imgSpec_s.x_OriginOfImg_a[0];
    caster.twoByteArray[1] = header_cpcs->imgSpec_s.x_OriginOfImg_a[1];
    TGA_halfWord x_Origin  = caster.halfWord;

    caster.twoByteArray[0] = header_cpcs->imgSpec_s.y_OriginOfImg_a[0];
    caster.twoByteArray[1] = header_cpcs->imgSpec_s.y_OriginOfImg_a[1];
    TGA_halfWord y_Origin  = caster.halfWord;

    bool breakLoop;
    TGA_index index_w;
    TGA_index index_h;
    TGA_index counter;

    if(0U == x_Origin && 0U == y_Origin)
    {
        counter = 0U;
        index_w = 0U;
        index_h = 0U;
        while(index_h < height)
        {
            index_w = 0U;
            while(index_w < width)
            {
                *pixelMap[index_h][index_w] = imgData_cpcs->pixelsData_a[counter];
                index_w++;
            }
            index_h++;
        }
    }
    else if(0U == x_Origin && height == y_Origin)
    {
        breakLoop = FALSE;
        counter = 0U;
        index_w = 0U;
        index_h = height - 1U;
        while(index_h >= 0U && !breakLoop)
        {
            index_w = 0U;
            while(index_w < width)
            {
                *pixelMap[index_h][index_w] = imgData_cpcs->pixelsData_a[counter];
                index_w++;
            }

            if(0U == index_h)
            {
                breakLoop = TRUE;
            }
            index_h--;
        }
    }
    else if(width == x_Origin && 0U == y_Origin)
    {
        breakLoop = FALSE;
        counter = 0U;
        index_w = width - 1U;
        index_h = 0U;
        while(index_h < height)
        {
            breakLoop = FALSE;
            index_w = width - 1U;
            while(index_w >= 0U && !breakLoop)
            {
                *pixelMap[index_h][index_w] = imgData_cpcs->pixelsData_a[counter];

                if(0U == index_w)
                {
                    breakLoop = TRUE;
                }
                index_w--;
            }

            index_h++;
        }
    }
    else if(width == x_Origin && height == y_Origin)
    {
        breakLoop = FALSE;
        counter = 0U;
        index_w = width - 1U;
        index_h = height - 1U;
        while(index_h >= 0U)
        {
            breakLoop = FALSE;
            index_w = width - 1U;
            while(index_w >= 0U && !breakLoop)
            {
                *pixelMap[index_h][index_w] = imgData_cpcs->pixelsData_a[counter];

                if(0U == index_w)
                {
                    breakLoop = TRUE;
                }
                index_w--;
            }

            if(0U == index_h)
            {
                break;
            }

            index_h--;
        }
    }
    else
    {
        ERROR_PRINT("wrong origin position of pixel map");
    }

}


