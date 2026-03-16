//
// Created by Omeed on 2/7/26.
//

#include "rook_rays.h"

static int file_of(int sq) { return sq % 8; }
static int rank_of(int sq) { return sq / 8; }

rook_rays rook_rays_init() {
    rook_rays rays;
    for (int sq = 0; sq < 64; sq++) {
        U64 north = 0ULL, south = 0ULL, east = 0ULL, west = 0ULL;
        int f = file_of(sq);
        int r = rank_of(sq);

        // North (up)
        for (int rf = r + 1; rf < 8; rf++) {
            north |= 1ULL << (rf * 8 + f);
        }

        // South (down)
        for (int rf = r - 1; rf >= 0; rf--) {
            south |= 1ULL << (rf * 8 + f);
        }

        // East (right)
        for (int ff = f + 1; ff < 8; ff++) {
            east |= 1ULL << (r * 8 + ff);
        }

        // West (left)
        for (int ff = f - 1; ff >= 0; ff--) {
            west |= 1ULL << (r * 8 + ff);
        }

        rays.r_rays_N[sq] = north;
        rays.r_rays_E[sq] = east;
        rays.r_rays_S[sq] = west;
        rays.r_rays_W[sq] = south;
    }
    return rays;
}