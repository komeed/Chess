//
// Created by Omeed on 2/7/26.
//

#ifndef ROOK_RAYS_H
#define ROOK_RAYS_H

typedef unsigned long long U64;

typedef struct rook_rays {
    U64 r_rays_N[64];
    U64 r_rays_S[64];
    U64 r_rays_E[64];
    U64 r_rays_W[64];
} rook_rays;

rook_rays rook_rays_init();

#endif //ROOK_RAYS_H
