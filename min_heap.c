#include <stdlib.h>
#include "min_heap.h"

/**
 * @brief Swap two heap items
 *
 * @param a Pointer to first item
 * @param b Pointer to second item
 */
static void swap_heap_items(HeapItem* a, HeapItem* b) {
    HeapItem temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Maintain heap property by moving an item down the heap
 *
 * @param heap Target heap
 * @param idx  Index of the item to move down
 */
static void heapify_down(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->size && heap->items[left].area < heap->items[smallest].area)
        smallest = left;

    if (right < heap->size && heap->items[right].area < heap->items[smallest].area)
        smallest = right;

    if (smallest != idx) {
        swap_heap_items(&heap->items[idx], &heap->items[smallest]);
        heapify_down(heap, smallest);
    }
}

MinHeap* heap_create(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->items = (HeapItem*)malloc(capacity * sizeof(HeapItem));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void heap_destroy(MinHeap* heap) {
    free(heap->items);
    free(heap);
}

void heap_push(MinHeap* heap, double area, int index) {
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;
        heap->items = realloc(heap->items, heap->capacity * sizeof(HeapItem));
    }

    int i = heap->size++;
    heap->items[i].area = area;
    heap->items[i].index = index;

    // Bubble up
    while (i > 0 && heap->items[(i - 1) / 2].area > heap->items[i].area) {
        swap_heap_items(&heap->items[i], &heap->items[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

HeapItem heap_pop(MinHeap* heap) {
    HeapItem root = heap->items[0];
    heap->items[0] = heap->items[--heap->size];
    heapify_down(heap, 0);
    return root;
}