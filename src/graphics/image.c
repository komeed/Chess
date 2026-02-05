//
// Created by Omeed on 2/4/26.
//

#define STB_IMAGE_IMPLEMENTATION

#include "image.h"
#include "stb_image.h"

Image create_image(const char* path, Rectangle rect) {
    int width, height, channels;
    Image image = {0};
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (!data) {
        fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
        return image;
    }
    image.data = data;
    image.orig_width = width;
    image.orig_height = height;
    image.rect = rect;
    printf("Image loaded: %dx%d, %d channels\n", width, height, channels);

    return image;
}