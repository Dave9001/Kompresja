#ifndef __TGA_TYPES_H__
#define __TGA_TYPES_H__


#include "IO/IO_Types.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>


#ifndef __FALSE__
#define __FALSE__
    #define FALSE (0U)  /* by exception that 0 is eq. to false */
#endif

#ifndef __TRUE__
#define __TRUE__
    #define TRUE (!FALSE)  /* logical negation of false is true which is DECRISING wanted value*/
#endif

#define TGA_NUMBER_OF_BITS_IN_BYTE (8U)

#define TGA_HEADER_SIZE (18U)

#define TGA_COLOR_MAP_SPEC_LEN (5U)
#define TGA_IMG_SPEC_LEN (10U)

#define TGA_COLOR_MAP_SPEC_LAST_INDEX (TGA_COLOR_MAP_SPEC_LEN - 1U)
#define TGA_IMG_SPEC_LAST_INDEX (TGA_IMG_SPEC_LEN - 1U)



#define ERROR_PRINT(msg)  do{\
            printf("Error: ");perror("");puts("");\
            printf("%s in FUNCTION: %s\n", msg, __func__);\
            printf("FILE: %s in LINE: %d\n", __FILE__, __LINE__);\
            exit(EXIT_FAILURE);\
        }while(FALSE)



typedef IO_msg TGA_byte;
typedef uint32_t TGA_index;
typedef uint16_t TGA_halfWord;

typedef enum tag_TGA_colorMapType_e
{
    TGA_COLOR_MAP_NOT_INCLUDED = 0U,
    TGA_COLOR_MAP_INCLUDED     = 1U
}TGA_colorMapType_e;

typedef enum tag_TGA_imgType_e
{
    TGA_NO_IMG_DATA_INCLUDED          = 0U,
    TGA_UNCOMPRESSED_COLOR_MAPPED     = 1U,
    TGA_UNCOMPRESSED_TRUE_COLOR       = 2U,
    TGA_UNCOMPRESSED_BLACK_N_WHITE    = 3U,
    TGA_RUN_LEN_ENCODED_COLOR_MAPPED  = 9U,
    TGA_RUN_LEN_ENCODED_TRUE_COLOR    = 10U,
    TGA_RUN_LEN_ENCODED_BLACK_N_WHITE = 11U
}TGA_imgType_e;

typedef struct tag_TGA_colorMapSpec_s
{
    TGA_byte firstEntryIndex_a[2];
    TGA_byte colorMapLength_a[2];
    TGA_byte colorMapEntrySize;
}TGA_colorMapSpec_s;

typedef struct tag_TGA_imgDescriptor_s
{
    TGA_byte alphaChanel: 4;
    TGA_byte unused     : 2;
    TGA_byte imgOrigin  : 2;
}TGA_imgDesc_s;

typedef union
{
    TGA_byte      byte;
    TGA_imgDesc_s bits_s;
}TGA_imgDesc_u;

typedef struct tag_TGA_imageSpec_s
{
    TGA_byte      x_OriginOfImg_a[2];
    TGA_byte      y_OriginOfImg_a[2];
    TGA_byte      width_a[2];
    TGA_byte      height_a[2];
    TGA_byte      pixelDepth;
    TGA_imgDesc_u imgDesc_u;
}TGA_imageSpec_s;

typedef struct tag_TGA_fileHeader_s
{
    TGA_byte            idLength;
    TGA_colorMapType_e  colorMapType;
    TGA_imgType_e       imgType;
    TGA_colorMapSpec_s  colorMapSpec_s;
    TGA_imageSpec_s     imgSpec_s;
}TGA_header_s;
typedef const TGA_header_s TGA_header_cs;


typedef struct tag_TGA_RGB_s
{
    TGA_byte red;
    TGA_byte green;
    TGA_byte blue;
}TGA_RGB_s;

typedef struct tag_TGA_imgData_s
{
    TGA_byte*  imgID_a;
    TGA_RGB_s* colorMapData_a;
    TGA_RGB_s* pixelsData_a;
}TGA_imgData_s;
typedef const TGA_imgData_s TGA_imgData_cs;


typedef union
{
    TGA_byte twoByteArray[2];
    TGA_halfWord halfWord;
}TGA_halfWordCaster_u;

#endif /*__TGA_TYPES_H__*/