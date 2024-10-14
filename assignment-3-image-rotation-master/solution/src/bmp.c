#include "bmp.h"
#include "image.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int from_bmp( FILE* input, struct image* img ){
    struct bmp_header bmp_head;

    if (fread(&bmp_head, sizeof(bmp_head), 1, input) != 1){
        fprintf(stderr, "Check header\n");
        return 1;
    }

    if (bmp_head.bfType != 0x4D42){
        fprintf(stderr, "Wrong type of file\n");
        return 1;
    }

    if (bmp_head.biBitCount != 24){
        fprintf(stderr, "Check bits\n");
        return 1;
    }

    img->width = bmp_head.biWidth;
    img->height = bmp_head.biHeight;
    img->data = malloc(sizeof (struct pixel)*bmp_head.biWidth*bmp_head.biHeight);
    if (!img->data) {
        fprintf(stderr, "Check your headers\n");
        return 1;
    }

    for (int i = 0; i < img->height; i++) {
        if (fread(&img->data[i * img->width], sizeof(struct pixel), img->width, input) != img->width) {
            free(img->data);
            fprintf(stderr, "Check your header\n");
            return 1;
        }
        fseek(input, (uint8_t)((4 - (img->width * sizeof(struct pixel)) % 4) % 4), SEEK_CUR);
    }


    return 0;


}


int to_bmp( FILE* output, struct image const* img ){

    struct bmp_header bmp_head =
            {
                    .bfType = 0x4D42,
                    .bfileSize = sizeof(struct bmp_header) + img->width * img->height * sizeof(struct pixel) + img->height * (uint8_t)((4 - (img->width * sizeof(struct pixel)) % 4) % 4),
                    .bfReserved = 0,
                    .bOffBits = sizeof(struct bmp_header),
                    .biSize = 40,
                    .biWidth = img->width,
                    .biHeight = img->height,
                    .biPlanes = 1,
                    .biBitCount = 24,
                    .biCompression = 0,
                    .biSizeImage = img->width * img->height * sizeof(struct pixel),
                    .biXPelsPerMeter = 0,
                    .biYPelsPerMeter = 0,
                    .biClrUsed = 0,
                    .biClrImportant = 0 };

    if (fwrite(&bmp_head, sizeof(struct bmp_header), 1, output) != 1){
        fprintf(stderr, "Error in writing\n");
        return 1;
    }

    for (int i = 0; i < img->height; i++) {
        if (fwrite(&img->data[img->width*i], sizeof (struct pixel), img->width, output) != img->width){
            fprintf(stderr, "Error in writing\n");
            return 1;
        }
        fseek(output, (uint8_t)((4 - (img->width * sizeof(struct pixel)) % 4) % 4), SEEK_CUR);
    }
    free(img->data);

    return 0;

}
