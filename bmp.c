#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "data_type.h"
#include "QR_Encode.h"

/*
https://blog.csdn.net/nibiewuxuanze/article/details/78805763
按小端字节序来存储，且宽度数据必须成4字节对齐。
图像数据区也不例外，按每行图像的数据字节，按4字节对齐。

图像数据按行倒序存放，先存储最后一行图像数据，然后依次存放，直到第一行数据。
这样设计，可能是为了从文件尾部往前读的时候，能够直接顺序读出图像数据吧。
*/
typedef union {
    uint8_t bytes[4];
    uint32_t value;
}LITTLE;

/*
 * @fileName: bmp file name: test.bmp
 * @width   : bmp pixel width: 32bit
 * @height  : bmp pixel width: 32bit
 * @color   : R[8]/G[8]/B[8]
 * @note    : BMP is l endian mode
 */
int bmp_gen_test(char *fileName, uint32_t width, uint32_t height, uint32_t color)
{
    FILE *fp;
    uint32_t i, j;
    LITTLE l_width, l_height, l_bfSize, l_biSizeImage;

    uint8_t r = color >> 16;
    uint8_t g = color >> 8;
    uint8_t b = color;
    uint32_t width_r  =  (width * 24 / 8 + 3) / 4 * 4;
    uint32_t bfSize = width_r * height + 54 + 2;
    uint32_t biSizeImage = width_r * height;

    l_width.value = width;
    l_height.value = height;
    l_bfSize.value = bfSize;
    l_biSizeImage.value = biSizeImage;

    /* BMP file format: www.cnblogs.com/wainiwann/p/7086844.html */
    uint8_t bmp_head_map[54] = {
        /* bmp file header: 14 byte */
        0x42, 0x4d,
        // bmp pixel size: width * height * 3 + 54
        l_bfSize.bytes[0], l_bfSize.bytes[1], l_bfSize.bytes[2], l_bfSize.bytes[3],
        0, 0 , 0, 0,
        54, 0 , 0, 0,    /* 14+40=54 */

        /* bmp map info: 40 byte */
        40, 0, 0, 0,
        //width
        l_width.bytes[0], l_width.bytes[1], l_width.bytes[2], l_width.bytes[3],
        //height
        l_height.bytes[0], l_height.bytes[1], l_height.bytes[2], l_height.bytes[3],
        1, 0,
        24, 00,             /* 24 bit: R[8]/G[8]/B[8] */

        0, 0, 0, 0,     //biCompression:0
//        0, 0, 0, 0,     //biSizeImage锛欰2 00 00 00=162
        l_biSizeImage.bytes[0], l_biSizeImage.bytes[1], l_biSizeImage.bytes[2], l_biSizeImage.bytes[3],
        0, 0, 0, 0,     //biXPelsPerMeter: 60 0F 00 00
        0, 0, 0, 0,     //biYPelsPerMeter
        0, 0, 0, 0,     //biClrUsed
        0, 0, 0, 0      //biClrImportant
    };

    /* write in binary format */
    fp = fopen(fileName, "wb+");
    if(fp == NULL)
    {
        printf("%s: file create failed!\n", fileName);
        return -1;
    }

    printf("%s: file create success!\n", fileName);

    fwrite(bmp_head_map, sizeof(bmp_head_map), 1, fp);

    // for(i = 0; i < height; i++) {
    //     for(j = 0; j < width; j++)
    //         fprintf(fp, "%c%c%c", b, g, r); /* BGR */
    // //4 byte align
    //     for(j = 0; j < width_r-width*3; j++)
    //         fprintf(fp, "%c", 0);
    // }

    EncodeData("http:163.com");
    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++)
        {
            if(m_byModuleData[i][j] == 1)
                fprintf(fp, "%c%c%c", 0xFF, 0xFF, 0xFF);
            else
                fprintf(fp, "%c%c%c", 0, 0, 0);
        }
        for(j = 0; j < width_r-width*3; j++)
            fprintf(fp, "%c", 0);
    }
    

    fprintf(fp, "%c%c", 0, 0); //PhotoShop two byte "0"

    if(fclose(fp))
    {
        printf("file close failed!\n");
        return -1;
    }
    fp = NULL;

    printf("width: %d\n", width);
    printf("height: %d\n", height);
    printf("R:%d, G:%d, B:%d or #%06x\n", r, g, b, color);

    return 0;
}

int main(int argc, char *argv[])
{
    int ret;
    char bmpName[200];

    char *name = "test";
    uint32_t width = MAX_MODULESIZE;
    uint32_t height = MAX_MODULESIZE;
    // uint32_t width = 50;
    // uint32_t height = 50;
    uint32_t color = 0xFFFF;

    /* generate bmp file name */
    sprintf(bmpName, "%s_%d_%d_0x%06x.bmp", name, width, height, color);
    printf("bmpName: %s\n", bmpName);

    ret = bmp_gen_test(bmpName, width, height, color);

    return 0;
}

