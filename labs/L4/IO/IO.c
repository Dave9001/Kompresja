#include "IO_Hidden_Types.h"
#include "IO.h"


#define IO_DEFAULT_READ_MODE ("r")
#define IO_DEFAULT_WRITE_MODE ("w")

extern IO_buffor_s IO_InitBuffor(
    IO_cPtrCStr inputFileName_cp,
    IO_cPtrCStr inputFileMode,
    IO_cPtrCStr outputFileName_cp,
    IO_cPtrCStr outputFileMode
);
extern inline void IO_WriteHard(
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

static inline void IO_Flush(IO_buffor_s *const buffor_cpcs);
static IO_FILEcp IO_InitStream(
    IO_cPtrCStr fileName_cp,
    IO_cPtrCStr mode_cp,
    IO_index stdStream
);


extern IO_buffor_s IO_InitBuffor(
    IO_cPtrCStr inputFileName_cp,
    IO_cPtrCStr inputFileMode,
    IO_cPtrCStr outputFileName_cp,
    IO_cPtrCStr outputFileMode
)
{
    IO_buffor_s buffor_s = {
        .inputStreamPtr = IO_InitStream(inputFileName_cp, inputFileMode, IO_STDIN),
        .outputStreamPtr = IO_InitStream(outputFileName_cp, outputFileMode, IO_STDOUT)
    };
    IO_buffor_s *buffor_ps = &buffor_s;

    IO_CLEAR_BUFFOR(buffor_ps);
    return buffor_s;
}

extern void IO_WriteHard(
    const IO_msg message,
    IO_buffor_s *const buffor_cps
)
{
    buffor_cps->message = message;
    buffor_cps->state_e = IO_BUFF_IS_FULL;
    buffor_cps->lastBitIndexInMsg = LSB_INDEX;
    IO_Flush(buffor_cps);
}

extern void IO_WriteN(
    const IO_msg binMsg_a[],
    const IO_index numBitsToWrite,
    IO_buffor_s *const buffor_cps
)
{
    if(numBitsToWrite > 0U)
    {
        IO_index binMsgIndex = numBitsToWrite - 1U;
        bool binMsgNotEnded = TRUE;
        while(binMsgNotEnded)
        {
            while(IO_WRITING == buffor_cps->state_e &&
                TRUE == binMsgNotEnded
            )
            {
                if(1U == binMsg_a[binMsgIndex])
                {
                    IO_WRITE_BIT(1U, buffor_cps);
                }
                else
                {
                    IO_WRITE_BIT(0U, buffor_cps);
                }
                
                if(0U != binMsgIndex)
                {
                    binMsgIndex--;
                }
                else
                {
                    binMsgNotEnded = FALSE;
                }
            }
            if(IO_BUFF_IS_FULL == buffor_cps->state_e)
            {
                IO_Flush(buffor_cps);
            }
        }
    }
}

extern bool IO_StillReadingFromStream(
    const IO_msg message,
    const IO_FILEcp streamPtr
)
{
    bool stillReadin = TRUE;
    if(IO_FEOF_STREAM_OPEN != feof(streamPtr))
    {
        stillReadin = FALSE;
    }
    else if(EOF == message)
    {
        stillReadin = FALSE;
    }

    return stillReadin;
}

extern void IO_CloseIO(IO_buffor_s *const buffor_cps)
{
    if(NULL != buffor_cps)
    {
        fclose(buffor_cps->inputStreamPtr);
        fclose(buffor_cps->outputStreamPtr);
        IO_CLEAR_BUFFOR(buffor_cps);
    }
    else
    {
        ERROR_PRINT("Buffor ptr is null");
    }
}

static inline void IO_Flush(IO_buffor_s *const buffor_cpcs)
{
    IO_WRITE(buffor_cpcs->message, buffor_cpcs->outputStreamPtr);
    IO_CLEAR_BUFFOR(buffor_cpcs);
}

static IO_FILEcp IO_InitStream(
    IO_cPtrCStr fileName_cp,
    IO_cPtrCStr mode_cp,
    IO_index stdStream
)
{
    FILE* stream = NULL;
    if(NULL == fileName_cp)
    {
        if(IO_STDIN == stdStream)
        {
            stream = stdin;
        }
        else
        {
            stream = stdout;
        }
    }
    else
    {
        if(NULL != mode_cp)
        {
            stream = fopen(fileName_cp, mode_cp);
        }
        else
        {
            if(IO_STDIN == stdStream)
            {
                stream = fopen(fileName_cp, IO_DEFAULT_READ_MODE);
            }
            else
            {
                stream = fopen(fileName_cp, IO_DEFAULT_WRITE_MODE);
            }
        }

        if(NULL == stream)
        {
            perror(fileName_cp);
            ERROR_PRINT("Stream ptr is NULL");
        }
    }

    FILE *const ret = (FILE *const)stream;
    return ret;
}


