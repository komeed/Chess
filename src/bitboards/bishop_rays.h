//
// Created by Omeed on 2/7/26.
//

#ifndef BISHOP_RAYS_H
#define BISHOP_RAYS_H
#include <stdio.h>

typedef unsigned long long U64;

typedef struct bishop_rays {
    U64 bishop_rays_NE[64];
    U64 bishop_rays_NW[64];
    U64 bishop_rays_SE[64];
    U64 bishop_rays_SW[64];
} bishop_rays;

bishop_rays bishop_rays_init();

#endif //BISHOP_RAYS_H
