#include <stdlib.h>
#include <string.h>
#include "visvalingam.h"
#include "min_heap.h"
#include "geometry.h"

/**
 * @brief Sort resolutions in descending order
 *
 * Creates a new array containing the resolutions sorted from highest to lowest.
 * This ordering is important for the simplification process as we need to process
 * higher resolutions before lower ones.
 *
 * @param resolutions Array of target resolutions
 * @param num_resolutions Number of resolutions
 * @return int* Newly allocated array containing sorted resolutions
 */
static int* sort_resolutions(const int* resolutions, int num_resolutions) {
    int* sorted = malloc(num_resolutions * sizeof(int));
    memcpy(sorted, resolutions, num_resolutions * sizeof(int));

    // Simple insertion sort since num_resolutions is typically small
    for (int i = 0; i < num_resolutions; i++) {
        for (int j = i + 1; j < num_resolutions; j++) {
            if (sorted[i] < sorted[j]) {
                int temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }
    return sorted;
}

/**
 * @brief Initialize the vertex linkage arrays
 *
 * Sets up the circular linkage between vertices and marks all vertices as active.
 *
 * @param prev_vertex Array to store previous vertex indices
 * @param next_vertex Array to store next vertex indices
 * @param active Array to mark active vertices
 * @param num_points Number of vertices in the polygon
 */
static void initialize_vertex_linkage(int* prev_vertex, int* next_vertex,
                                    char* active, int num_points) {
    for (int i = 0; i < num_points; i++) {
        prev_vertex[i] = (i - 1 + num_points) % num_points;
        next_vertex[i] = (i + 1) % num_points;
        active[i] = 1;
    }
}

/**
 * @brief Calculate initial effective areas for all vertices
 *
 * Computes the initial triangle areas for each vertex and adds them to the heap.
 *
 * @param heap MinHeap to store vertex areas
 * @param areas Array to store area values
 * @param points_data Input polygon points
 * @param prev_vertex Previous vertex indices
 * @param next_vertex Next vertex indices
 * @param num_points Number of vertices
 */
static void calculate_initial_areas(MinHeap* heap, double* areas,
                                  const double* points_data,
                                  const int* prev_vertex, const int* next_vertex,
                                  int num_points) {
    for (int i = 0; i < num_points; i++) {
        double area = triangle_area(
            &points_data[2 * prev_vertex[i]],
            &points_data[2 * i],
            &points_data[2 * next_vertex[i]]
        );
        areas[i] = area;
        heap_push(heap, area, i);
    }
}

/**
 * @brief Create result array for a given resolution
 *
 * Allocates and initializes a new numpy array for storing the simplified polygon.
 *
 * @param target_vertices Number of vertices in simplified polygon
 * @return PyArrayObject* New numpy array or NULL on failure
 */
static PyArrayObject* create_result_array(int target_vertices) {
    npy_intp dims[2] = {target_vertices + 1, 2};  // +1 for closure point
    return (PyArrayObject*)PyArray_SimpleNew(2, dims, NPY_DOUBLE);
}

/**
 * @brief Clean up allocated resources
 *
 * Frees all memory allocated during the simplification process.
 *
 * @param prev_vertex Previous vertex indices array
 * @param next_vertex Next vertex indices array
 * @param active Active vertices array
 * @param areas Areas array
 * @param heap MinHeap structure
 * @param sorted_resolutions Sorted resolutions array
 */
static void cleanup_resources(int* prev_vertex, int* next_vertex, char* active,
                            double* areas, MinHeap* heap, int* sorted_resolutions) {
    free(prev_vertex);
    free(next_vertex);
    free(active);
    free(areas);
    heap_destroy(heap);
    free(sorted_resolutions);
}

PyObject* visvalingam_whyatt_multi_c(PyObject* self, PyObject* args) {
    PyArrayObject *points_obj, *resolutions_obj;

    // Parse input arguments
    if (!PyArg_ParseTuple(args, "O!O!", &PyArray_Type, &points_obj,
                         &PyArray_Type, &resolutions_obj))
        return NULL;

    // Validate input dimensions
    if (PyArray_NDIM(points_obj) != 2 || PyArray_DIM(points_obj, 1) != 2) {
        PyErr_SetString(PyExc_ValueError, "Points array must be of shape (n, 2)");
        return NULL;
    }

    if (PyArray_NDIM(resolutions_obj) != 1) {
        PyErr_SetString(PyExc_ValueError, "Resolutions must be 1-dimensional array");
        return NULL;
    }

    // Get input data
    int num_points = PyArray_DIM(points_obj, 0);
    int num_resolutions = PyArray_DIM(resolutions_obj, 0);
    int* resolutions = (int*)PyArray_DATA(resolutions_obj);
    double* points_data = (double*)PyArray_DATA(points_obj);

    // Check if input polygon is closed
    int is_closed = (points_data[0] == points_data[2*(num_points-1)] &&
                    points_data[1] == points_data[2*(num_points-1)+1]);

    if (is_closed) {
        num_points--;  // Work with unclosed polygon
    }

    // Validate resolutions
    for (int i = 0; i < num_resolutions; i++) {
        if (resolutions[i] >= num_points) {
            PyErr_SetString(PyExc_ValueError,
                          "Invalid resolution: must be < input vertices");
            return NULL;
        }
        else if (resolutions[i] < 3){
            char error_str[100];
            sprintf(error_str, "Invalid resolution: must be >= 3, is: %d", resolutions[i]);
            PyErr_SetString(PyExc_ValueError,
                          error_str);
            return NULL;
        }
    }

    // Allocate working memory
    int* prev_vertex = malloc(num_points * sizeof(int));
    int* next_vertex = malloc(num_points * sizeof(int));
    char* active = malloc(num_points * sizeof(char));
    double* areas = malloc(num_points * sizeof(double));

    if (!prev_vertex || !next_vertex || !active || !areas) {
        PyErr_NoMemory();
        cleanup_resources(prev_vertex, next_vertex, active, areas, NULL, NULL);
        return NULL;
    }

    // Initialize vertex linkage
    initialize_vertex_linkage(prev_vertex, next_vertex, active, num_points);

    // Initialize heap and calculate initial areas
    MinHeap* heap = heap_create(num_points);
    if (!heap) {
        PyErr_NoMemory();
        cleanup_resources(prev_vertex, next_vertex, active, areas, NULL, NULL);
        return NULL;
    }

    calculate_initial_areas(heap, areas, points_data, prev_vertex, next_vertex, num_points);

    // Create result list and sort resolutions
    PyObject* result_list = PyList_New(num_resolutions);
    if (!result_list) {
        cleanup_resources(prev_vertex, next_vertex, active, areas, heap, NULL);
        return NULL;
    }

    int* sorted_resolutions = sort_resolutions(resolutions, num_resolutions);
    if (!sorted_resolutions) {
        Py_DECREF(result_list);
        cleanup_resources(prev_vertex, next_vertex, active, areas, heap, NULL);
        return NULL;
    }

    // Main simplification loop
    int active_count = num_points;
    int res_idx = 0;

    while (res_idx < num_resolutions && active_count > 3) {
        int target = sorted_resolutions[res_idx];

        // Simplify until we reach target resolution
        while (active_count > target) {
            HeapItem min_item = heap_pop(heap);
            int vertex_idx = min_item.index;

            // Skip if vertex already removed or area outdated
            if (!active[vertex_idx] || min_item.area != areas[vertex_idx])
                continue;

            // Remove vertex
            active[vertex_idx] = 0;
            active_count--;

            // Update links
            int prev_idx = prev_vertex[vertex_idx];
            int next_idx = next_vertex[vertex_idx];
            next_vertex[prev_idx] = next_idx;
            prev_vertex[next_idx] = prev_idx;

            // Update areas of adjacent vertices
            for (int adj_idx = 0; adj_idx < 2; adj_idx++) {
                int idx = adj_idx == 0 ? prev_idx : next_idx;
                if (active[idx]) {
                    double new_area = triangle_area(
                        &points_data[2 * prev_vertex[idx]],
                        &points_data[2 * idx],
                        &points_data[2 * next_vertex[idx]]
                    );
                    areas[idx] = new_area;
                    heap_push(heap, new_area, idx);
                }
            }
        }

        // Create result array for current resolution
        PyArrayObject* result_obj = create_result_array(target);
        if (!result_obj) {
            Py_DECREF(result_list);
            cleanup_resources(prev_vertex, next_vertex, active, areas, heap, sorted_resolutions);
            return NULL;
        }

        // Find first active vertex
        int curr_idx = 0;
        while (!active[curr_idx]) curr_idx++;

        // Extract simplified polygon
        double* result_data = (double*)PyArray_DATA(result_obj);
        extract_simplified(points_data, next_vertex, active, curr_idx, target, result_data);

        // Add to result list at appropriate position
        for (int i = 0; i < num_resolutions; i++) {
            if (resolutions[i] == target) {
                PyList_SET_ITEM(result_list, i, (PyObject*)result_obj);
            }
        }

        res_idx++;
    }

    // Clean up
    cleanup_resources(prev_vertex, next_vertex, active, areas, heap, sorted_resolutions);

    return result_list;
}

// Module setup functions
static PyMethodDef VisvalingamMethods[] = {
    {"simplify_multi", visvalingam_whyatt_multi_c, METH_VARARGS,
     "Simplify polygon to multiple resolutions using Visvalingam-Whyatt algorithm"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef visvalingam_module = {
    PyModuleDef_HEAD_INIT,
    "visvalingam_c",
    "C implementation of Visvalingam-Whyatt polygon simplification algorithm",
    -1,
    VisvalingamMethods
};

PyMODINIT_FUNC PyInit_visvalingam_c(void) {
    import_array();
    return PyModule_Create(&visvalingam_module);
}