#include "TGA_Read.h"
#include "IO/IO.h"


#define TGA_EOF_CASE(msg, errorDesc, buffor) do{\
    if(FALSE == IO_StillReadingFromStream(msg, buffor->inputStreamPtr))\
    {\
        ERROR_PRINT(errorDesc);\
    }\
}while(FALSE)

extern TGA_index TGA_PxMapHeight(TGA_header_cs *const header_cpcs);
extern TGA_index TGA_PxMapWidth(TGA_header_cs *const header_cpcs);
extern TGA_header_s TGA_ReadHeader(IO_buffor_cs *const buffor_cpcs);
extern TGA_imgData_s TGA_ReadImgData(
    TGA_header_cs *const header_cpcs,
    IO_buffor_cs *const buffor_cpcs
);
extern TGA_RGB_s** TGA_PixelMap(
    TGA_header_cs *const header_cpcs,
    TGA_imgData_cs *const imgData_cpcs
);

static TGA_byte* TGA_ReadImageID(
    TGA_header_cs *const header_cpcs,
    IO_buffor_cs *const buffor_cpcs
);
static TGA_RGB_s* TGA_ReadColorMapData(
    TGA_header_cs *const header_cpcs,
    IO_buffor_cs *const buffor_cpcs
);
static TGA_RGB_s* TGA_ReadPixelsData(
    TGA_header_cs *const header_cpcs,
    IO_buffor_cs *const buffor_cpcs
);

/*int main(void)
{
    IO_buffor_s buffor_s     = IO_InitBuffor("testPics/example0.tga", "rb", NULL, NULL);

    TGA_header_cs fileHeader = TGA_ReadHeader(&buffor_s);
    TGA_imgData_cs imgData_s = TGA_ReadImgData(&fileHeader, &buffor_s);

    return 0;
}*/



extern TGA_index TGA_PxMapHeight(TGA_header_cs *const header_cpcs)
{
    TGA_index height = 0U;

    if(header_cpcs != NULL)
    {
        TGA_halfWordCaster_u caster;
        caster.twoByteArray[0] = header_cpcs->imgSpec_s.height_a[0];
        caster.twoByteArray[1] = header_cpcs->imgSpec_s.height_a[1];
        height = caster.halfWord;
    }

    return height;
}

extern TGA_index TGA_PxMapWidth(TGA_header_cs *const header_cpcs)
{
    TGA_index width = 0U;

    if(header_cpcs != NULL)
    {
        TGA_halfWordCaster_u caster;
        caster.twoByteArray[0] = header_cpcs->imgSpec_s.width_a[0];
        caster.twoByteArray[1] = header_cpcs->imgSpec_s.width_a[1];
        width = caster.halfWord;
    }

    return width;
}



extern TGA_RGB_s** TGA_PixelMap(
    TGA_header_cs *const header_cpcs,
    TGA_imgData_cs *const imgData_cpcs
)
{
    TGA_halfWordCaster_u caster;
    caster.twoByteArray[0] = header_cpcs->imgSpec_s.width_a[0];
    caster.twoByteArray[1] = header_cpcs->imgSpec_s.width_a[1];
    TGA_halfWord width     = caster.halfWord;

    caster.twoByteArray[0] = header_cpcs->imgSpec_s.height_a[0];
    caster.twoByteArray[1] = header_cpcs->imgSpec_s.height_a[1];
    TGA_halfWord height    = caster.halfWord;

    TGA_RGB_s** pixelMap = malloc(sizeof(*pixelMap) * height);
    if(NULL == pixelMap)
    {
        ERROR_PRINT("malloc failed");
    }

    TGA_index index = 0U;
    while(index < height)
    {
        pixelMap[index] = malloc(sizeof(*(*pixelMap)) * width);
        if(NULL == pixelMap[index])
        {
            ERROR_PRINT("malloc failed");
        }
        index++;
    }

    TGA_index index_w = 0U;
    TGA_index index_h = height - 1U;
    TGA_index pixelsDataIndex = 0U;
    bool breakLoop = FALSE;
    while(index_h >= 0U && ! breakLoop)
    {
        index_w = 0U;
        while(index_w < width)
        {
            pixelMap[index_h][index_w] = imgData_cpcs->pixelsData_a[pixelsDataIndex];
            index_w++;
            pixelsDataIndex++;
        }

        if(0U == index_h)
        {
            breakLoop = TRUE;
        }
        index_h--;
    }

    return pixelMap;
}


extern TGA_imgData_s TGA_ReadImgData(
    TGA_header_cs *const header_cpcs,
    IO_buffor_cs *const buffor_cpcs
)
{
    TGA_imgData_s imgData_s  = {};
    imgData_s.imgID_a        = TGA_ReadImageID(header_cpcs, buffor_cpcs);
    imgData_s.colorMapData_a = TGA_ReadColorMapData(header_cpcs, buffor_cpcs);
    imgData_s.pixelsData_a   = TGA_ReadPixelsData(header_cpcs, buffor_cpcs);

    return imgData_s;
}

extern TGA_header_s TGA_ReadHeader(IO_buffor_cs *const buffor_cpcs)
{
    TGA_header_s header_s = {};

    TGA_byte byteOfData  = 0U;
    TGA_index nBytesRead = 0U;
    while(nBytesRead < TGA_HEADER_SIZE)
    {
        byteOfData = IO_READ(buffor_cpcs->inputStreamPtr);
        TGA_EOF_CASE(byteOfData, "EOF occurrence in TGA header", buffor_cpcs);
        
        switch(nBytesRead)
        {
            case 0U:  header_s.idLength                            = byteOfData; break;

            case 1U:  header_s.colorMapType                        = byteOfData; break;

            case 2U:  header_s.imgType                             = byteOfData; break;

            case 3U:  header_s.colorMapSpec_s.firstEntryIndex_a[0] = byteOfData; break;
            case 4U:  header_s.colorMapSpec_s.firstEntryIndex_a[1] = byteOfData; break;
            case 5U:  header_s.colorMapSpec_s.colorMapLength_a[0]  = byteOfData; break;
            case 6U:  header_s.colorMapSpec_s.colorMapLength_a[1]  = byteOfData; break;
            case 7U:  header_s.colorMapSpec_s.colorMapEntrySize    = byteOfData; break;

            case 8U:  header_s.imgSpec_s.x_OriginOfImg_a[0]        = byteOfData; break;
            case 9U:  header_s.imgSpec_s.x_OriginOfImg_a[1]        = byteOfData; break;
            case 10U: header_s.imgSpec_s.y_OriginOfImg_a[0]        = byteOfData; break;
            case 11U: header_s.imgSpec_s.y_OriginOfImg_a[1]        = byteOfData; break;
            case 12U: header_s.imgSpec_s.width_a[0]                = byteOfData; break;
            case 13U: header_s.imgSpec_s.width_a[1]                = byteOfData; break;
            case 14U: header_s.imgSpec_s.height_a[0]               = byteOfData; break;
            case 15U: header_s.imgSpec_s.height_a[1]               = byteOfData; break;
            case 16U: header_s.imgSpec_s.pixelDepth                = byteOfData; break;
            case 17U: header_s.imgSpec_s.imgDesc_u.byte            = byteOfData; break;
        }

        nBytesRead++;
    }

    return header_s;
}

static TGA_RGB_s* TGA_ReadPixelsData(
    TGA_header_cs *const header_cpcs,
    IO_buffor_cs *const buffor_cpcs
)
{
    if(header_cpcs->imgSpec_s.pixelDepth != 24U)
    {
        ERROR_PRINT("program is not meant to handle\
                     other than 24b per pixel depth");
    }

    if(header_cpcs->imgSpec_s.imgDesc_u.bits_s.alphaChanel != 0U)
    {
        ERROR_PRINT("program is not meant to handle\
                     other than 0 alpha chanel");
    }

    TGA_halfWordCaster_u caster;
    caster.twoByteArray[0] = header_cpcs->imgSpec_s.width_a[0];
    caster.twoByteArray[1] = header_cpcs->imgSpec_s.width_a[1];
    TGA_halfWord width     = caster.halfWord;

    caster.twoByteArray[0] = header_cpcs->imgSpec_s.height_a[0];
    caster.twoByteArray[1] = header_cpcs->imgSpec_s.height_a[1];
    TGA_halfWord height    = caster.halfWord;

    TGA_index pixelsAmount   = width * height;

    TGA_RGB_s* pixelsData_a = NULL;
    pixelsData_a = malloc(sizeof(*pixelsData_a) * pixelsAmount);
    if(NULL == pixelsData_a && pixelsAmount != 0U)
    {
        ERROR_PRINT("malloc failed");
    }

    TGA_byte red, green, blue;
    TGA_index nthPixel    = 0U;
    while(nthPixel < pixelsAmount)
    {
       
        red   = IO_READ(buffor_cpcs->inputStreamPtr);
        green = IO_READ(buffor_cpcs->inputStreamPtr);
        blue  = IO_READ(buffor_cpcs->inputStreamPtr);

    
        TGA_EOF_CASE(red, "EOF in TGA pixels data", buffor_cpcs);
        TGA_EOF_CASE(green, "EOF in TGA pixels data", buffor_cpcs);
        TGA_EOF_CASE(blue, "EOF in TGA pixels data", buffor_cpcs);

        pixelsData_a[nthPixel].red   = red;
        pixelsData_a[nthPixel].green = green;
        pixelsData_a[nthPixel].blue  = blue;

        nthPixel++;
    }

    return pixelsData_a;
}

static TGA_RGB_s* TGA_ReadColorMapData(
    TGA_header_cs *const header_cpcs,
    IO_buffor_cs *const buffor_cpcs
)
{
    TGA_RGB_s* colorMapData_a = NULL;

    if(TGA_COLOR_MAP_INCLUDED == header_cpcs->colorMapType)
    {
        TGA_halfWordCaster_u caster;
        caster.twoByteArray[0] = header_cpcs->colorMapSpec_s.colorMapLength_a[0];
        caster.twoByteArray[1] = header_cpcs->colorMapSpec_s.colorMapLength_a[1];

        TGA_halfWord colorMapLen    = caster.halfWord;
        TGA_index colorMapEntrySize = header_cpcs->colorMapSpec_s.colorMapEntrySize;
        TGA_index colorMapDataSize  = colorMapLen * colorMapEntrySize;
        
        if(colorMapLen != 0U)
        {
            if(colorMapEntrySize != 24U)
            {
                ERROR_PRINT("There's only implemented 24B per (RGB) color map entry");
            }

            colorMapData_a = malloc(sizeof(*colorMapData_a) * colorMapDataSize);
            if(NULL == colorMapData_a)
            {
                ERROR_PRINT("malloc failed");
            }

            TGA_byte red, green, blue  = 0U;
            TGA_index index = 0U;
            while(index < colorMapDataSize)
            {
                red   = IO_READ(buffor_cpcs->inputStreamPtr);
                green = IO_READ(buffor_cpcs->inputStreamPtr);        
                blue  = IO_READ(buffor_cpcs->inputStreamPtr);

                TGA_EOF_CASE(red, "EOF in TGA color image data", buffor_cpcs);
                TGA_EOF_CASE(green, "EOF in TGA color image data", buffor_cpcs);
                TGA_EOF_CASE(blue, "EOF in TGA color image data", buffor_cpcs);

                colorMapData_a[index].red   = red;
                colorMapData_a[index].green = green;
                colorMapData_a[index].green = blue;

                index++;
            }
        }
    }

    return colorMapData_a;
}

static TGA_byte* TGA_ReadImageID(
    TGA_header_cs *const header_cpcs,
    IO_buffor_cs *const buffor_cpcs
)
{
    TGA_byte* imgID = NULL;
    
    if(0U != header_cpcs->idLength)
    {   
        TGA_byte byteOfData  = 0U;
        TGA_index imgIdSize = header_cpcs->idLength;

        imgID = malloc(sizeof(*imgID) * imgIdSize);

        if(NULL == imgID)
        {
            ERROR_PRINT("malloc failed");
        }

        TGA_index index = 0U;
        while(index < imgIdSize)
        {
            byteOfData = IO_READ(buffor_cpcs->inputStreamPtr);
            TGA_EOF_CASE(byteOfData, "EOF in TGA color image data", buffor_cpcs);

            imgID[index] = byteOfData;
            index++;
        }
    }

    return imgID;
}
