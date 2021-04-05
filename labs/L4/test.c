#include "TGA_Types.h"




int main(void)
{

    TGA_imgDescriptor_u imgDesc_u = {};

    imgDesc_u.bits_s.unused = 0x1U;
    printf("%x\n",imgDesc_u.bits_s.unused);
    printf("%x\n",imgDesc_u.byte & 0xF0);

    return 0;
}
