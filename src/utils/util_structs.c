//
// Created by Omeed on 2/5/26.
//

#include "util_structs.h"

void compute_ndc_for_i8_point(i8_point point, size_int windowSize, float dl, float* x, float* y) { // dl is each square lengtgh
        *x   =  (float)point.x / (float)windowSize.width  * 2.0f - 1.0f;
        *y =  (float)point.y / (float)windowSize.height * 2.0f - 1.0f;
}