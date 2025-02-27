# python-visvalingam

A high-performance C implementation of the Visvalingam-Whyatt algorithm for polygon simplification with Python bindings.

## Overview

This package provides an efficient implementation of the Visvalingam-Whyatt algorithm, which is widely used for line and polygon simplification in cartography, GIS, and data visualization. The algorithm simplifies a polygon by iteratively removing the point that creates the triangle with the smallest area.

Key features:
- Written in C for high performance
- Python bindings with NumPy integration
- Multi-resolution simplification in a single pass
- Handles both open and closed polygons
- Memory efficient implementation

## Installation

### Prerequisites
- Python 3.7+
- NumPy
- C compiler (GCC, Clang, or MSVC)

### Installing from source
```bash
git clone https://github.com/lukszi/python-visvalingam.git
cd python-visvalingam
python setup.py build_ext --inplace
pip install -e .
```

## Usage

```python
import numpy as np
import visvalingam_c

# Create a sample polygon (a square with 5 vertices, including closure)
polygon = np.array([
    [0, 0],
    [0, 10],
    [10, 10],
    [10, 0],
    [0, 0]  # Closure point
], dtype=np.float64)

# Define target resolutions (number of vertices in simplified polygon)
resolutions = np.array([3], dtype=np.int32)

# Simplify polygon
simplified = visvalingam_c.simplify_multi(polygon, resolutions)

print(simplified[0])  # Output simplified polygon with 3 vertices + closure
```

### Advanced Example

```python
import numpy as np
import visvalingam_c
import matplotlib.pyplot as plt

# Load a complex polygon from file
polygon = np.loadtxt("complex_polygon.csv", delimiter=",")

# Simplify to multiple resolutions in a single pass
resolutions = np.array([50, 20, 10], dtype=np.int32)
simplified_polygons = visvalingam_c.simplify_multi(polygon, resolutions)

# Plot original and simplified polygons
plt.figure(figsize=(15, 5))

plt.subplot(1, 4, 1)
plt.plot(polygon[:, 0], polygon[:, 1], 'k-')
plt.title(f"Original: {len(polygon)} vertices")

for i, (res, poly) in enumerate(zip(resolutions, simplified_polygons)):
    plt.subplot(1, 4, i+2)
    plt.plot(poly[:, 0], poly[:, 1], 'r-')
    plt.title(f"Simplified: {res} vertices")

plt.tight_layout()
plt.show()
```

## Algorithm Explanation

The Visvalingam-Whyatt algorithm works as follows:

1. For each vertex, calculate the "effective area" (the area of the triangle formed by the vertex and its two neighboring vertices)
2. Place all vertices in a min-heap data structure, ordered by their effective area
3. Iteratively remove the vertex with the smallest effective area
4. After each removal, recalculate the effective areas of the two adjacent vertices
5. Continue until the desired number of vertices remains

This implementation optimizes the process by:
- Using a custom min-heap for efficient vertex selection
- Maintaining a circular linked list structure to track active vertices
- Supporting multiple resolution targets in a single pass by processing them in descending order
- Avoiding redundant calculations when extracting simplified polygons

## API Reference

### `visvalingam_c.simplify_multi(points, resolutions)`

Simplifies a polygon to multiple resolution levels in a single pass.

**Parameters:**
- **points** (*numpy.ndarray*): Input polygon as an array of shape (n, 2) containing x, y coordinates
- **resolutions** (*numpy.ndarray*): Target resolutions as a 1D array of integers, each representing the number of vertices to retain

**Returns:**
- **list**: List of simplified polygons as numpy arrays, one for each target resolution

**Notes:**
- The polygon can be either open or closed. If closed (first and last points are identical), the function handles it appropriately.
- Each resolution must be at least 3 and less than the number of input vertices.
- The returned polygons always include a closure point (first point repeated at the end).
