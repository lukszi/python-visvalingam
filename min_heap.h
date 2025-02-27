/**
 * @file min_heap.h
 * @brief Minimum heap implementation for polygon simplification
 *
 * This header defines a minimum heap data structure specialized for the
 * Visvalingam-Whyatt algorithm. The heap maintains vertex indices ordered
 * by their effective areas.
 */

#ifndef MIN_HEAP_H
#define MIN_HEAP_H

/**
 * @struct HeapItem
 * @brief Represents a single item in the minimum heap
 *
 * @param area   The effective area of the triangle at this vertex
 * @param index  The index of the vertex in the original polygon
 */
typedef struct {
    double area;
    int index;
} HeapItem;

/**
 * @struct MinHeap
 * @brief Minimum heap data structure
 *
 * @param items    Array of heap items
 * @param size     Current number of items in the heap
 * @param capacity Maximum number of items the heap can hold
 */
typedef struct {
    HeapItem* items;
    int size;
    int capacity;
} MinHeap;

/**
 * @brief Initialize a new minimum heap
 *
 * @param capacity Initial capacity of the heap
 * @return MinHeap* Pointer to the newly created heap
 */
MinHeap* heap_create(int capacity);

/**
 * @brief Free all memory associated with the heap
 *
 * @param heap Pointer to the heap to destroy
 */
void heap_destroy(MinHeap* heap);

/**
 * @brief Push a new item onto the heap
 *
 * @param heap  Target heap
 * @param area  Area value for the new item
 * @param index Vertex index for the new item
 */
void heap_push(MinHeap* heap, double area, int index);

/**
 * @brief Remove and return the minimum item from the heap
 *
 * @param heap Target heap
 * @return HeapItem The item with the smallest area
 */
HeapItem heap_pop(MinHeap* heap);

#endif /* MIN_HEAP_H */