#ifndef __TGA_READER_H__
#define __TGA_READER_H__

#include "TGA_Types.h"


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

#endif /*__TGA_READER_H__*/