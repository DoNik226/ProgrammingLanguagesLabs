#include "bmp.h"
#include "image.h"
#include "rotate.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc!=4){
        printf("Error input");
        return 1;
    }
    struct image img;
    int angel = atoi(argv[3]);
    FILE* input_file = fopen(argv[1], "rb");
    FILE* output_file = fopen(argv[2], "wb");

    if (input_file == NULL || output_file == NULL) {
        printf("Error opening files.\n");
        return 1;
    }

    if (from_bmp(input_file, &img) != 0) {
        return 1;
    }

    img  = rotate(img, angel);

    if (to_bmp(output_file, &img) != 0) {
        return 1;
    }

    fclose(input_file);
    fclose(output_file);

    return 0;
}
