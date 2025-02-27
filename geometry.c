#include <math.h>
#include "geometry.h"

double triangle_area(const double* p1, const double* p2, const double* p3) {
    return fabs((p2[0] - p1[0]) * (p3[1] - p1[1]) -
                (p3[0] - p1[0]) * (p2[1] - p1[1])) / 2.0;
}

void extract_simplified(const double* points_data, const int* next_vertex,
                       const char* active, int curr_idx, int target_vertices,
                       double* result_data) {
    // Extract main vertices
    for (int i = 0; i < target_vertices; i++) {
        result_data[2 * i] = points_data[2 * curr_idx];
        result_data[2 * i + 1] = points_data[2 * curr_idx + 1];
        curr_idx = next_vertex[curr_idx];
    }

    // Add closure point
    result_data[2 * target_vertices] = result_data[0];
    result_data[2 * target_vertices + 1] = result_data[1];
}