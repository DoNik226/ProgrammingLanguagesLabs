#include "image.h"
#include "rotate.h"
#include <stdint.h>
#include <stdlib.h>

struct image rotate(struct image img, int angel){
    struct image img1;
    img1.data= malloc(sizeof(struct pixel) * img.width * img.height);
    if (angel==90 || angel==-270){
        img1.height=img.width;
        img1.width=img.height;
        for (int i=0; i<img.width; i++){
            for (int j=0; j<img.height; j++){
                img1.data[img.height*i+j]= img.data[j*img.width+(img.width-1-i)];
            }
        }
        for (int i=0; i<img.height*img.width; i++){
            img.data[i]=img1.data[i];
        }
        img.width=img1.width;
        img.height=img1.height;
    }
    else if (angel==180 || angel==-180){
        img1.height=img.height;
        img1.width=img.width;
        for (int i=0; i<img.width; i++){
            for (int j=0; j<img.height; j++){
                img1.data[img.height*i+j]= img.data[(img.height)*(img.width-i-1)+ (img.height-j-1)];
            }
        }
        for (int i=0; i<img.height*img.width; i++){
            img.data[i]=img1.data[i];
        }
    }
    else if (angel==-90 || angel==270){
        img1.height=img.width;
        img1.width=img.height;
        for (int i=0; i<img.width; i++){
            for (int j=0; j<img.height; j++){
                img1.data[img.height*i+j]= img.data[(img.height-j-1)*img.width+i];
            }
        }
        for (int i=0; i<img.height*img.width; i++){
            img.data[i]=img1.data[i];
        }
        img.width=img1.width;
        img.height=img1.height;
    }
    free(img1.data);
    return img;
}
