/**
 * @file geometry.h
 * @brief Geometric calculations for polygon simplification
 *
 * This header defines geometric functions used in the Visvalingam-Whyatt
 * algorithm, including area calculations and polygon handling.
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

/**
 * @brief Calculate the area of a triangle formed by three points
 *
 * Uses the shoelace formula to calculate the signed area of a triangle.
 * The absolute value is returned.
 *
 * @param p1 Pointer to first point (x,y coordinates)
 * @param p2 Pointer to second point
 * @param p3 Pointer to third point
 * @return double Absolute area of the triangle
 */
double triangle_area(const double* p1, const double* p2, const double* p3);

/**
 * @brief Extract simplified polygon vertices at a given resolution
 *
 * Extracts vertices from the working polygon structure into a result array,
 * ensuring proper closure of the polygon.
 *
 * @param points_data Source points array
 * @param next_vertex Array of next vertex indices
 * @param active     Array indicating which vertices are active
 * @param curr_idx   Starting vertex index
 * @param target_vertices Number of vertices to extract
 * @param result_data Destination array for results
 */
void extract_simplified(const double* points_data, const int* next_vertex,
                       const char* active, int curr_idx, int target_vertices,
                       double* result_data);

#endif /* GEOMETRY_H */