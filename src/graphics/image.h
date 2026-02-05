//
// Created by Omeed on 2/4/26.
//

#ifndef IMAGE_H
#define IMAGE_H

#include "rectangle.h"

typedef struct {
    Rectangle rect;
    int orig_width;
    int orig_height;
    unsigned char *data;
} Image;

Image create_image(const char* path, Rectangle rect);

#endif //IMAGE_H