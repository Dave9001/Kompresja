

#include "TGA_PxMap.h"

extern TGA_pxMap_aa TGA_CreateChMap(
    TGA_index width,
    TGA_index height
);
extern TGA_pxMap_aa TGA_CreatePxMap(
    TGA_index width,
    TGA_index height
);
extern TGA_byte const* const* TGA_ChannelMap(
    TGA_pxMap_roaa originalPxMap,
    TGA_byte (*Channel)(const TGA_RGB_s),
    TGA_index width,
    TGA_index height
);
extern inline TGA_byte TGA_RedCh(const TGA_RGB_s pixel);
extern inline TGA_byte TGA_GreenCh(const TGA_RGB_s pixel);
extern inline TGA_byte TGA_BlueCh(const TGA_RGB_s pixel);


extern TGA_pxMap_aa TGA_CreatePxMap(
    TGA_index width,
    TGA_index height
)
{
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

    return pixelMap;
}

extern TGA_pxMap_aa TGA_CreateChMap(
    TGA_index width,
    TGA_index height
)
{
    TGA_byte** channelMap = malloc(sizeof(*channelMap) * height);
    if(NULL == channelMap)
    {
        ERROR_PRINT("malloc failed");
    }

    TGA_index index = 0U;
    while(index < height)
    {
        channelMap[index] = malloc(sizeof(*(*channelMap)) * width);
        if(NULL == channelMap[index])
        {
            ERROR_PRINT("malloc failed");
        }
        index++;
    }

    return channelMap;
}

extern TGA_byte const* const* TGA_ChannelMap(
    TGA_pxMap_roaa originalPxMap,
    TGA_byte (*PxChannel)(const TGA_RGB_s),
    TGA_index width,
    TGA_index height
)
{
    TGA_byte** channelMap = TGA_CreateChMap(width, height);

    TGA_RGB_s pixel   = {};
    TGA_byte  channel = 0U;

    TGA_index hi = 0U; 
    TGA_index wi = 0U;
    while(hi < height)
    {
        wi = 0U;
        while(wi < width)
        {
            pixel   = originalPxMap[hi][wi];
            channel = PxChannel((const TGA_RGB_s)pixel);
            channelMap[hi][wi] = channel;

            wi++;
        }

        hi++;
    }

    return channelMap;
}


extern inline TGA_byte TGA_RedCh(const TGA_RGB_s pixel)
{
    return pixel.red;
}

extern inline TGA_byte TGA_GreenCh(const TGA_RGB_s pixel)
{
    return pixel.green;
}

extern inline TGA_byte TGA_BlueCh(const TGA_RGB_s pixel)
{
    return pixel.blue;
}