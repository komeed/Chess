//
// Created by Omeed on 2/7/26.
//

#include "bishop_rays.h"

static int col_of(int sq) { return sq % 8; }
static int row_of(int sq) { return sq / 8; }

bishop_rays bishop_rays_init() {
    bishop_rays rays;
    for (int sq = 0; sq < 64; sq++) {
        U64 ne = 0ULL, nw = 0ULL, se = 0ULL, sw = 0ULL;

        int c = col_of(sq);
        int r = row_of(sq);

        // NE: up-right
        for (int rf = r+1, ff = c+1; rf < 8 && ff < 8; rf++, ff++) {
            ne |= 1ULL << (rf*8 + ff);
        }

        // NW: up-left
        for (int rf = r+1, ff = c-1; rf < 8 && ff >= 0; rf++, ff--) {
            nw |= 1ULL << (rf*8 + ff);
        }

        // SE: down-right
        for (int rf = r-1, ff = c+1; rf >= 0 && ff < 8; rf--, ff++) {
            se |= 1ULL << (rf*8 + ff);
        }

        // SW: down-left
        for (int rf = r-1, ff = c-1; rf >= 0 && ff >= 0; rf--, ff--) {
            sw |= 1ULL << (rf*8 + ff);
        }

        rays.bishop_rays_NE[sq] = ne;
        rays.bishop_rays_NW[sq] = nw;
        rays.bishop_rays_SE[sq] = se;
        rays.bishop_rays_SW[sq] = sw;
    }
    return rays;
}