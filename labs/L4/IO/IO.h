#ifndef __IO_H__
#define __IO_H__

#include "IO_Types.h"
#include <stdio.h>

#define IO_READ(streamPtr) (IO_msg)getc(streamPtr)
#define IO_WRITE(message_cp, streamPtr) (void)putc(message_cp, streamPtr)

unsigned long long int NumOfSended; // delete this, only for lab expected output

extern IO_buffor_s IO_InitBuffor(
    IO_cPtrCStr inputFileName_cp,
    IO_cPtrCStr inputFileMode,
    IO_cPtrCStr outputFileName_cp,
    IO_cPtrCStr outputFileMode
);
extern void IO_WriteHard(
    const IO_msg message,
    IO_buffor_s *const buffor_cps
);
extern void IO_WriteN(
    const IO_msg binMsg_a[],
    const IO_index numBitsToWrite,
    IO_buffor_s *const buffor_cpcs
);
extern bool IO_StillReadingFromStream(
    const IO_msg message,
    const IO_FILEcp streamPtr
);
extern void IO_CloseIO(IO_buffor_s *const buffor_cps);


#endif /*__IO_H__*/