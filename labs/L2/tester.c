#include "IO/IO_Types.h"
#include "IO/IO.h"


void testBufforInit(void)
{
    IO_buffor_s buffor_s = IO_InitBuffor("t2", "rb", "t1", "wb");
    IO_CloseIO(&buffor_s);  
}

void testReadWrite(void)
{
    IO_buffor_s buffor_s = IO_InitBuffor("test3.bin", "rb", "t1", "wb");
    IO_msg message;

    int k = 0;
    message = IO_Read(buffor_s.inputStreamPtr);
    while(IO_StillReadingFromStream(message, buffor_s.inputStreamPtr))
    {   printf("msg: %d\n",message);
        IO_WriteHard(message, &buffor_s);
        printf("%x",message);
        message = IO_Read(buffor_s.inputStreamPtr);
        if(k == 10)
            return;
        k++;
    }
    IO_CloseIO(&buffor_s);
}

void testReadWriteN(void)
{
    IO_buffor_s buffor_s = IO_InitBuffor("test3.bin", "rb", "t1", "wb");
    IO_msg message;
    IO_msg binBuff[16];
    message = IO_Read(buffor_s.inputStreamPtr);

    while(IO_StillReadingFromStream(message, buffor_s.inputStreamPtr))
    {   
        binBuff[2] = (message & 0x80U) != 0? 1U : 0U;
        binBuff[1] = (message & 0x40U) != 0? 1U : 0U;
        binBuff[0] = (message & 0x20U) != 0? 1U : 0U;
        IO_WriteN(binBuff, 3U, &buffor_s);
        binBuff[2] = (message & 0x10U) != 0? 1U : 0U;
        binBuff[1] = (message & 0x08U) != 0? 1U : 0U;
        binBuff[0] = (message & 0x04U) != 0? 1U : 0U;
        IO_WriteN(binBuff, 3U, &buffor_s);
        binBuff[0] = (message & 0x02U) != 0? 1U : 0U;
        binBuff[3] = (message & 0x01U) != 0? 1U : 0U;
        IO_WriteN(binBuff, 1U, &buffor_s);

        message = IO_Read(buffor_s.inputStreamPtr);
        if(IO_StillReadingFromStream(message, buffor_s.inputStreamPtr))
        {
            binBuff[2] = (message & 0x80U) != 0? 1U : 0U;
            binBuff[1] = (message & 0x40U) != 0? 1U : 0U;
            binBuff[0] = (message & 0x20U) != 0? 1U : 0U;
            IO_WriteN(binBuff, 4U, &buffor_s);
            binBuff[2] = (message & 0x10U) != 0? 1U : 0U;
            binBuff[1] = (message & 0x08U) != 0? 1U : 0U;
            binBuff[0] = (message & 0x04U) != 0? 1U : 0U;
            IO_WriteN(binBuff, 3U, &buffor_s);
            binBuff[1] = (message & 0x02U) != 0? 1U : 0U;
            binBuff[0] = (message & 0x01U) != 0? 1U : 0U;
            IO_WriteN(binBuff, 2U, &buffor_s);
        }
        else{
            binBuff[0] = binBuff[3];
            IO_WriteN(binBuff, 1U, &buffor_s);
            break;
        }
        message = IO_Read(buffor_s.inputStreamPtr);
    }
    IO_CloseIO(&buffor_s);
}


int main(void)
{
    //testBufforInit();
    //testReadWrite();
    testReadWriteN();
    return 0;
}
