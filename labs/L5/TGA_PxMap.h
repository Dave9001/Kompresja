#ifndef __TGA_PXMAP_H__
#define __TGA_PXMAP_H__


#include "TGA_Types.h"

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




#endif/*__TGA_PXMAP_H__*/