#include "UQ_UniformQuantizer.h"
#include "IT_Metrics.h"
#include "TGA_Read.h"
#include "TGA_PxMap.h"
#include "IO/IO.h"
#include <math.h>

static bool readParam(
    int argc,
    char** argv,
    char** inputPicName,
    char** outputPicName,
    int* redChannelSize,
    int* greenChannelSize,  
    int* blueChannelSize
);
static void writeHeader(
    TGA_header_cs *const header_cpcs,
    IO_buffor_cs * buffor_cpcs
);
static TGA_index power(TGA_byte, TGA_byte);

int main(int argc, char** argv)
{
    char* inputPicName  = NULL;
    char* outputPicName = NULL;

    TGA_index redChannelSize   = 0;
    TGA_index greenChannelSize = 0;
    TGA_index blueChannelSize  = 0;

    bool dataCorrect =  readParam(
                                argc,
                                argv,
                                &inputPicName,
                                &outputPicName,
                                &redChannelSize,
                                &greenChannelSize,  
                                &blueChannelSize
                        );

    if(dataCorrect)
    {
        IO_buffor_s buffor_s     = IO_InitBuffor(inputPicName, "rb", outputPicName, "wb");

        TGA_header_cs header     = TGA_ReadHeader(&buffor_s);
        TGA_imgData_cs imgData_s = TGA_ReadImgData(&header, &buffor_s);

        TGA_index height         = TGA_PxMapHeight(&header);
        TGA_index width          = TGA_PxMapWidth(&header);

        TGA_pxMap_aa originPxMap = TGA_PixelMap(&header, &imgData_s);
        TGA_RGB_s ** modQtPxMap  = TGA_PixelMap(&header, &imgData_s);
        
        /*
        TGA_byte const* const* redChMap   =  TGA_ChannelMap(
                                                (TGA_pxMap_roaa)originPxMap,
                                                TGA_RedCh,
                                                width,
                                                height
                                            );
        TGA_byte const* const* greenChMap =  TGA_ChannelMap(
                                                (TGA_pxMap_roaa)originPxMap,
                                                TGA_GreenCh,
                                                width,
                                                height
                                            );
        TGA_byte const* const* blueChMap  =  TGA_ChannelMap(
                                                (TGA_pxMap_roaa)originPxMap,
                                                TGA_BlueCh,
                                                width,
                                                height
                                            );
        
        TGA_pxMap_aa quantizedPxMap       = UQ_Converter(
                                            originPxMap,
                                            redChannelSize,
                                            greenChannelSize,  
                                            blueChannelSize
                                        );

        IT_real mse      = IT_PxMapMse(
                            originPxMap,
                            quantizedPxMap,
                            height,
                            width
                        );
        
        IT_real mseRed   = IT_RedChMapMse(
                            originPxMap,
                            quantizedPxMap,
                            height,
                            width
                        );

        IT_real mseGreen = IT_GreenChMapMse(
                            originPxMap,
                            quantizedPxMap,
                            height,
                            width
                        );

        IT_real mseBlue  = IT_BlueChMapMse(
                            originPxMap,
                            quantizedPxMap,
                            height,
                            width
                        );
        */

        TGA_byte tail[26] = {};
        TGA_index i = 0U;
        while(i < 26)
        {
            tail[i] = IO_READ(buffor_s.inputStreamPtr);
            i++;
        }

        writeHeader(&header, &buffor_s);

        TGA_index redChannelQtStep   = (TGA_index) ( 256U / (TGA_index)pow(2.0, redChannelSize));
        TGA_index greenChannelQtStep = (TGA_index) ( 256U / (TGA_index)pow(2.0, greenChannelSize));
        TGA_index blueChannelQtStep  = (TGA_index) ( 256U / (TGA_index)pow(2.0, blueChannelSize));
        
        TGA_index numbOfPx = width * height;
        TGA_RGB_s pixel    = {};
        TGA_index red       = 0U;
        TGA_index green     = 0U;
        TGA_index blue      = 0U;
    
        //printf("h: %d w: %d\n",height, width);

        TGA_index index_w = 0U;
        TGA_index index_h = height - 1U;
        TGA_index pixelsDataIndex = 0U;
        bool breakLoop = FALSE;
        while(index_h >= 0U && ! breakLoop)
        {
            index_w = 0U;
            while(index_w < width)
            {
                pixel = originPxMap[index_h][index_w];
                //printf("red: %d\n",red);

                red   = pixel.red;
                green = pixel.green;
                blue  = pixel.blue;

                //printf("%x %x %x",red, green, blue);

                red   = IT_LinearUniformQuantization(red,   redChannelQtStep);
                green = IT_LinearUniformQuantization(green, greenChannelQtStep);
                blue  = IT_LinearUniformQuantization(blue,  blueChannelQtStep);

                pixel.red   = red;
                pixel.green = green;
                pixel.blue  = blue;

                modQtPxMap[index_h][index_w] = pixel;

                //printf("r: %d\n",red );
                //printf("g: %d\n",green );
                //printf("b: %d\n",blue );
                
                IO_WriteHard((TGA_byte)red,   &buffor_s);
                IO_WriteHard((TGA_byte)green, &buffor_s);
                IO_WriteHard((TGA_byte)blue,  &buffor_s);

                
                index_w++;
            }

            if(0U == index_h)
            {
                breakLoop = TRUE;
            }
            index_h--;
        }

        i = 0U;
        while(i < 26)
        {
            IO_WriteHard(tail[i],  &buffor_s);
            i++;
        }

        fclose(buffor_s.inputStreamPtr);
        fclose(buffor_s.outputStreamPtr);

        TGA_index mode = 1U;
        while(mode <= 4U)
        {
            IT_Mse(
                originPxMap,
                modQtPxMap,
                height,
                width,
                mode
            );
            mode++;
        }
    }

    

    return EXIT_SUCCESS;
}

TGA_index power(TGA_byte base, TGA_byte exp)
{
    TGA_index ret = base;
    if(0U == base)
    {
        ret = 1U;
    }
    else
    {
        TGA_index expf = 1;
    while(expf < exp)
    {
        ret *= base;
        expf++;
    }
    printf("ret: %d\n",ret);
    }

    
    return ret;
}


static bool readParam(
    int argc,
    char** argv,
    char** inputPicName,
    char** outputPicName,
    int* redChannelSize,
    int* greenChannelSize,  
    int* blueChannelSize
)
{
    bool ret = TRUE;

    if(5U + 1U != argc) /*+1U is the program name which is always and also by default argv[0]*/
    {
        puts("1Expecting 5 arguments");
        puts("input.tga                         [string]");
        puts("output.tga                        [string]");
        puts("No. of bit for   red   channel    [unisgned int]");
        puts("No. of bit for  green  channel    [unisgned int]");
        puts("No. of bit for   blue  channel    [unisgned int]");

        ret = FALSE;
    }
    else
    {
        char* tag_inputPicName  = argv[1];
        char* tag_outputPicName = argv[2];

        int tag_redChannelSize   = atoi(argv[3]);
        int tag_greenChannelSize = atoi(argv[4]);
        int tag_blueChannelSize  = atoi(argv[5]);

        if( (0U == tag_redChannelSize   && argv[3][0] != '0') ||
            (0U == tag_greenChannelSize && argv[4][0] != '0') ||
            (0U == tag_blueChannelSize  && argv[5][0] != '0') ||
            ('-' == argv[3][0]) ||
            ('-' == argv[4][0]) ||
            ('-' == argv[5][0])
        )
        {
            puts("2Expecting 5 arguments");
            puts("1: input.tga                         [string]");
            puts("2: output.tga                        [string]");
            puts("3: No. of bit for   red   channel    [unsigned int] <= 8");
            puts("4: No. of bit for  green  channel    [unsigned int] <= 8");
            puts("5: No. of bit for   blue  channel    [unsigned int] <= 8");

            ret = FALSE;
        }
        else
        {
            *inputPicName = tag_inputPicName;
            *outputPicName = tag_outputPicName;

            *redChannelSize = tag_redChannelSize;
            *greenChannelSize = tag_greenChannelSize;
            *blueChannelSize = tag_blueChannelSize;
        }

        if( tag_redChannelSize   > 8U || 
            tag_greenChannelSize > 8U ||
            tag_blueChannelSize  > 8U 
        )
        {
            puts("2Expecting 5 arguments");
            puts("1: input.tga                         [string]");
            puts("2: output.tga                        [string]");
            puts("3: No. of bit for   red   channel    [unsigned int] <= 8");
            puts("4: No. of bit for  green  channel    [unsigned int] <= 8");
            puts("5: No. of bit for   blue  channel    [unsigned int] <= 8");

            ret = FALSE;
        }



    }

    return ret;
}

static void writeHeader(
    TGA_header_cs *const header_cpcs,
    IO_buffor_cs * buffor_cpcs
)
{
    TGA_byte byteOfData   = 0U;
    TGA_index nBytesWrote = 0U;
    while(nBytesWrote < TGA_HEADER_SIZE)
    {       
        switch(nBytesWrote)
        {
            case 0U:  byteOfData = header_cpcs->idLength; break;

            case 1U:  byteOfData = header_cpcs->colorMapType; break;

            case 2U:  byteOfData = header_cpcs->imgType; break;

            case 3U:  byteOfData = header_cpcs->colorMapSpec_s.firstEntryIndex_a[0]; break;
            case 4U:  byteOfData = header_cpcs->colorMapSpec_s.firstEntryIndex_a[1]; break;
            case 5U:  byteOfData = header_cpcs->colorMapSpec_s.colorMapLength_a[0]; break;
            case 6U:  byteOfData = header_cpcs->colorMapSpec_s.colorMapLength_a[1]; break;
            case 7U:  byteOfData = header_cpcs->colorMapSpec_s.colorMapEntrySize; break;

            case 8U:  byteOfData = header_cpcs->imgSpec_s.x_OriginOfImg_a[0]; break;
            case 9U:  byteOfData = header_cpcs->imgSpec_s.x_OriginOfImg_a[1]; break;
            case 10U: byteOfData = header_cpcs->imgSpec_s.y_OriginOfImg_a[0]; break;
            case 11U: byteOfData = header_cpcs->imgSpec_s.y_OriginOfImg_a[1]; break;
            case 12U: byteOfData = header_cpcs->imgSpec_s.width_a[0]; break;
            case 13U: byteOfData = header_cpcs->imgSpec_s.width_a[1]; break;
            case 14U: byteOfData = header_cpcs->imgSpec_s.height_a[0]; break;
            case 15U: byteOfData = header_cpcs->imgSpec_s.height_a[1]; break;
            case 16U: byteOfData = header_cpcs->imgSpec_s.pixelDepth; break;
            case 17U: byteOfData = header_cpcs->imgSpec_s.imgDesc_u.byte; break;
        }

        IO_WriteHard(byteOfData, buffor_cpcs);

        nBytesWrote++;
    }


}

