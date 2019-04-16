#include "image.h"
#include "utils.h"
#include <png.h>
#include <cstdlib>

namespace sereno
{
    uint8_t* getPNGRGBABytesFromFiles(const char* filePath, uint32_t* width, uint32_t* height)
    {
        int bitDepth, colorType;
        uint8_t* pixels        = NULL;
        png_bytep* rowPointers = NULL;

        FILE* pngFile = fopen(filePath, "rb");
        png_infop infoPtr;
        if(!pngFile)
        {
            ERROR("Could not read the file %s\n", filePath);
            return NULL;
        }

        png_structp pngPtr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if(!pngPtr)
            goto endReadPng;

        infoPtr = png_create_info_struct(pngPtr);
        if(!infoPtr)
        {
            png_destroy_read_struct(&pngPtr, NULL, NULL);
            goto endReadPng;
        }

        //Error handling
        if(setjmp(png_jmpbuf(pngPtr)))
        {
            ERROR("Failed to decode the PNG file %s\n", filePath);
            png_destroy_info_struct(pngPtr, &infoPtr);
            png_destroy_read_struct(&pngPtr, NULL, NULL);
            goto endReadPng;
        }

        //Read png information
        png_init_io(pngPtr, pngFile);
        png_read_info(pngPtr, infoPtr);
        png_get_IHDR(pngPtr, infoPtr, width, height, &bitDepth, &colorType, NULL, NULL, NULL);

        //Put the data into a 8 depth RGBA buffers
        if(colorType == PNG_COLOR_TYPE_GRAY ||
             colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(pngPtr);

        if (colorType == PNG_COLOR_TYPE_GRAY &&
            bitDepth < 8) 
            png_set_expand_gray_1_2_4_to_8(pngPtr);

        if(colorType == PNG_COLOR_TYPE_RGB ||
           colorType == PNG_COLOR_TYPE_GRAY ||
           colorType == PNG_COLOR_TYPE_PALETTE)
            png_set_filler(pngPtr, 0xFF, PNG_FILLER_AFTER);

        if(bitDepth == 16)
            png_set_strip_16(pngPtr);

        png_read_update_info(pngPtr, infoPtr);

        //Allocate the memory for the pixels and the one used for libpng
        pixels        = (uint8_t*)malloc(sizeof(uint8_t)*(*width)*(*height)*4);
        rowPointers = (png_bytep*)malloc(sizeof(png_bytep)*(*height));
        for(uint32_t i = 0; i < *height; i++)
            rowPointers[i] = pixels + (*height-1-i)*sizeof(uint8_t)*(*width)*4;

        png_read_image(pngPtr, rowPointers);

        //Free everything
        png_destroy_info_struct(pngPtr, &infoPtr);
        png_destroy_read_struct(&pngPtr, NULL, NULL);
        fclose(pngFile);
        free(rowPointers);

        return pixels;
    endReadPng:
        fclose(pngFile);
        if(pixels)
            free(pixels);
        if(rowPointers)
            free(rowPointers);
        return NULL;
    }
}
